#!/usr/bin/env python3
"""Sample an explicitly selected process tree; no external dependencies.

The CSV contains resident memory / working set, not unique physical memory.
It intentionally records no command lines, environment variables or file names.
"""

from __future__ import annotations

import argparse
import csv
from datetime import datetime, timezone
import json
import math
from pathlib import Path
import platform
import subprocess
import sys
import time


def windows_snapshot() -> dict[int, tuple[int, int, str]]:
    command = (
        "Get-CimInstance Win32_Process | ForEach-Object { "
        "$created = if ($_.CreationDate) { $_.CreationDate.ToUniversalTime().ToString('o') } else { '' }; "
        "[PSCustomObject]@{pid=$_.ProcessId; ppid=$_.ParentProcessId; "
        "rss=[uint64]$_.WorkingSetSize; started=$created} "
        "} | ConvertTo-Json -Compress"
    )
    result = subprocess.run(
        ["powershell.exe", "-NoProfile", "-NonInteractive", "-Command", command],
        check=True, capture_output=True, text=True, timeout=15,
    )
    rows = json.loads(result.stdout)
    if isinstance(rows, dict):
        rows = [rows]
    return {
        int(row["pid"]): (int(row["ppid"]), int(row["rss"]), row["started"])
        for row in rows
    }


def unix_snapshot() -> dict[int, tuple[int, int, str]]:
    if sys.platform.startswith("linux"):
        result: dict[int, tuple[int, int, str]] = {}
        for entry in Path("/proc").iterdir():
            if not entry.name.isdecimal():
                continue
            try:
                stat = (entry / "stat").read_text()
                fields = stat[stat.rfind(")") + 2:].split()
                # fields[0] is /proc/PID/stat field 3 (state).
                parent = int(fields[1])
                started = fields[19]
                status = (entry / "status").read_text().splitlines()
                rss = next(int(line.split()[1]) * 1024 for line in status
                           if line.startswith("VmRSS:"))
                result[int(entry.name)] = (parent, rss, started)
            except (OSError, ValueError, StopIteration, IndexError):
                # A process can disappear while its metadata is being read.
                continue
        return result

    result = subprocess.run(
        ["ps", "-axo", "pid=,ppid=,rss=,lstart="],
        check=True, capture_output=True, text=True, timeout=15,
    )
    rows = {}
    for line in result.stdout.splitlines():
        fields = line.split(maxsplit=3)
        if len(fields) == 4:
            rows[int(fields[0])] = (int(fields[1]), int(fields[2]) * 1024, fields[3])
    return rows


def process_tree(root: int, rows: dict[int, tuple[int, int, str]]) -> set[int]:
    found = {root}
    while True:
        children = {pid for pid, (parent, _, _) in rows.items() if parent in found}
        enlarged = found | children
        if enlarged == found:
            return found
        found = enlarged


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--pid", required=True, type=int, help="Application process ID")
    parser.add_argument("--duration", type=float, default=300, help="Seconds (default: 300)")
    parser.add_argument("--interval", type=float, default=2, help="Seconds (default: 2)")
    parser.add_argument("--out", type=Path, required=True, help="New CSV path; never overwrites")
    args = parser.parse_args()
    if (args.pid <= 0 or not math.isfinite(args.duration) or args.duration <= 0
            or not math.isfinite(args.interval) or args.interval < 0.25):
        parser.error("pid and duration must be positive; interval must be at least 0.25 seconds")

    snapshot = windows_snapshot if sys.platform == "win32" else unix_snapshot
    if sys.platform not in {"win32", "darwin"} and not sys.platform.startswith("linux"):
        parser.error("supported platforms: Windows, macOS, Linux")
    rows = snapshot()
    if args.pid not in rows:
        parser.error("the selected process does not exist or its memory metadata is unavailable")
    identity = rows[args.pid][2]
    start = time.monotonic()
    metric = "working_set_bytes" if sys.platform == "win32" else "rss_bytes"
    try:
        output = args.out.open("x", encoding="utf-8", newline="")
    except OSError as exc:
        parser.error(str(exc))
    print(f"Sampling PID {args.pid} and descendants to {args.out}; metric={metric}")
    print("Tree sum may count shared pages more than once; compare within the same OS.")
    try:
        with output:
            writer = csv.writer(output)
            writer.writerow(["utc_time", "elapsed_seconds", "os", "metric", "root_pid",
                             "process_count", "root_bytes", "tree_sum_bytes"])
            while True:
                if args.pid not in rows or rows[args.pid][2] != identity:
                    print("Selected process exited (or PID was reused); sampling stopped.")
                    break
                selected = process_tree(args.pid, rows)
                elapsed = time.monotonic() - start
                writer.writerow([
                    datetime.now(timezone.utc).isoformat(timespec="seconds"),
                    round(elapsed, 3), platform.system(), metric, args.pid, len(selected),
                    rows[args.pid][1], sum(rows[pid][1] for pid in selected),
                ])
                output.flush()
                remaining = args.duration - (time.monotonic() - start)
                if remaining <= 0:
                    break
                time.sleep(min(args.interval, remaining))
                rows = snapshot()
    except KeyboardInterrupt:
        print("Sampling stopped; collected rows are saved.")
    except (OSError, subprocess.SubprocessError, ValueError) as exc:
        print(f"Sampling failed: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

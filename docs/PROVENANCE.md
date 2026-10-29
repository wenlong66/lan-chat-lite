# Source provenance and protocol notes

## Upstream

- Project: LAN Messenger — https://github.com/lanmessenger/lanmessenger
- Baseline: tag `v1.3.9`
- Commit: `a4d364a384d5c8c615f443c062a77c00ca4412b2`
- License: original GNU GPL v3 text retained in `/LICENSE`; original source headers retained.
- LanChat Lite is an independent experimental derivative, not an upstream release.

The repository starts from this fixed baseline. Existing upstream build/documentation files describe the original application unless the Lite README or Chinese getting-started guide explicitly supersedes them. The upstream UI-test agent and old `lmm` mock daemon remain optional development tools and are not included in the production target by default.

## Initial Lite changes

1. Minimal interface defaults, separate application identity/configuration, upstream update UI disabled.
2. Clipboard image confirmation and temporary PNG attachments, with pixel/byte/cache limits.
3. RAII file ownership, bounded network queues, atomic destination commit and progress timeouts.
4. Receiver-confirmed file completion with a streaming SHA-256 receipt.
5. Regression tests, multi-platform build workflow and reproducible testing instructions.

## File transfer v2

The existing file offer/accept metadata and TCP routing header lead to a dedicated file socket. The receiver writes `START2\n`; the sender then streams exactly the advertised number of raw bytes. After atomic commit, the receiver writes `DONE2 <lowercase SHA-256 hex>\n`. The sender reports completion only after checking this receipt against its streaming hash. Control data can be fragmented; limits and timeouts apply. The hash provides consistency detection, not authentication or encryption. Empty files still use the handshake and receipt.

This protocol requires Lite peers. Do not claim compatibility with the original file-transfer protocol, resumability, TLS, or authenticated device pairing. Text/discovery formats remain inherited. All higher-level peer identity and untrusted-LAN security limitations need a separate design before broader deployment.

## Binary names

Current Windows/macOS builds use `LanChatLite.exe` and `LanChat Lite.app` (bundle executable `LanChat Lite`). Display names, Windows version resources, macOS bundle metadata and packaging scripts use the Lite identity. The configuration keys and directories remain unchanged; Linux retains `lan-messenger`. Earlier alpha archives retain their original names.

## Lite artwork and UI

The SVG artwork in `lmc/src/resources/lite-icons/` was created for LanChat Lite in October 2026. The PNG, ICO and ICNS variants are generated from those original vectors, with no external icon pack or image service. These assets and `scripts/render-icons.cpp` are licensed GPL-3.0-or-later. The upstream artwork and copyright notices remain in the repository. See [branding notes](BRANDING.zh-CN.md) for regeneration and platform mapping.

## Inline screenshot messages (October 2026)

Clipboard screenshot offers remain `filetype=normal` and add optional
`contenttype=image/png` metadata. Updated peers auto-accept these PNG offers up to
65 MiB and render a thumbnail only after local commit; unmarked files keep the
existing file interaction. Older Lite receivers ignore the extra field and offer
the PNG as a normal file. START2/DONE2 and SHA-256 semantics are unchanged.

`gui/widgets/chatimage.{h,cpp}` is original GPL-3.0-or-later code. Its Qt text object
uses a shared 8 MiB thumbnail cache, PNG-only decoding, a 16 MiPixel / 64 MiB
decoded-image bound, and a 320×240 preview size. It follows the Qt
[custom text object API](https://doc.qt.io/qt-6/qtextobjectinterface.html), with no
third-party image code copied. Completed sent images are retained in `Sent Images`
under the configured receive directory; stored chat logs retain local paths.

## Direct LAN sockets (October 2026)

Discovery UDP sockets, the TCP listener, and outgoing chat/file sockets explicitly
use `QNetworkProxy::NoProxy`. This keeps LAN traffic direct when application/system
proxies change and avoids the Qt 6.8.3 SOCKS5 UDP disconnect crash reproduced by
`tests/unit/tst_lannetwork.cpp`. The local SOCKS5 test endpoint is original
GPL-3.0-or-later code; no Qt implementation was copied or patched. Socket proxy
selection follows the Qt 6.8 [QAbstractSocket](https://doc.qt.io/qt-6.8/qabstractsocket.html#setProxy)
and [QTcpServer](https://doc.qt.io/qt-6.8/qtcpserver.html#setProxy) APIs.

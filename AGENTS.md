# LanChat Lite development

Read README.md, docs/ROADMAP.zh-CN.md and docs/TEST_PLAN.zh-CN.md before changes.

## Scope

- C++17 and Qt 6 Widgets. Main targets: Windows x64, macOS arm64 and x86_64.
- Keep one-to-one chat, clipboard images and direct file transfers simple.
- No cloud account, external relay, telemetry, AI feature, chat bot or auto-update service without a new user requirement.
- Keep original license/copyright notices. Record upstream code provenance in docs/PROVENANCE.md.

## Implementation

- Use RAII/QObject ownership. Bound queues and caches; file size must not determine memory consumption.
- Never load whole files into memory or use Base64 for file payloads.
- TCP reads can be fragmented or coalesced. Define framing limits and test both cases.
- Report a successful file send only after the receiver has committed the file and returned a matching content hash.
- Preserve an existing destination on receive failure or cancellation.
- Keep networking/disk event handling bounded so large transfers do not monopolize the GUI loop.
- Do not call legacy file mock tests a validation of the new START2/DONE2 protocol.
- Do not change OS firewall, security settings, global Git configuration or user settings automatically.

## Verification

Build Release and run CTest for meaningful changes. For transfer ownership changes also use ASan/UBSan where available:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DLMC_VERSION=0.1.0-alpha
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure --timeout 90
```

The `LMC_ENABLE_SANITIZERS` option enables development sanitizers for GCC/Clang. `LMC_BUILD_TEST_AGENT` and `LMC_BUILD_LMM` are OFF by default. The default production executable must remain agent-free.

Record commands, environment and failures honestly in docs/VALIDATION.zh-CN.md. A Linux loopback result or CI build is not evidence of real Windows↔Mac throughput, sleep recovery or 24-hour memory behavior. 50 MB and 80 MB/s are targets, not promises.

## Publishing

Never commit build outputs, received files, chat histories, private keys generated at runtime, credentials, .env files or machine-specific paths. Existing keys under tests/data/crypto are synthetic upstream test fixtures. Use reviewed changes and the CI workflow; do not enable scheduled automation or automatic release publication without a user request.

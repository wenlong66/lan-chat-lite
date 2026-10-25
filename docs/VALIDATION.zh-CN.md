# 初始验证记录

日期：2026-10-25。版本：**0.1.0-alpha**。

本记录对应 LanChat Lite 首版源代码，基于上游 LAN Messenger
`v1.3.9` / `a4d364a384d5c8c615f443c062a77c00ca4412b2`。
首个应用提交为 `d989c34bf8f9884d8438df33e3a020c3dd68d390`；以下完整 CI 对应
`5642680df3bb9073a6106a60cc6f75a18a2c7f2b`，其应用 C++ 源码相同，仅修正两处打包脚本。

## GitHub Actions 跨平台验证：全部通过

[第二轮完整运行 40196244267](https://github.com/wenlong66/lan-chat-lite/actions/runs/40196244267)
于 2026-10-25 13:20 UTC 完成，最终状态为 `completed / success`。

| 平台 / runner | 自动测试 | 打包与启动检查 |
|---|---|---|
| Windows x64 / windows-2022 / Qt 6.8.3 | 3/3 套件通过，8.36 秒 | windeployqt、清理开发 Qt 环境后启动、ZIP 与 SHA-256 上传均通过 |
| Apple Silicon / macos-15 / Qt 6.8.3 | 3/3 套件通过，9.14 秒 | 架构检查、ad-hoc 签名、清理 Qt/DYLD 环境后启动、ZIP 与 SHA-256 上传均通过 |
| Intel Mac / macos-15-intel / Qt 6.8.3 | 3/3 套件通过，12.42 秒 | 架构检查、ad-hoc 签名、清理 Qt/DYLD 环境后启动、ZIP 与 SHA-256 上传均通过 |
| Linux / ubuntu-24.04 / 发行版 Qt 6 | 3/3 套件通过，4.55 秒 | 本地布局打包与 xvfb 启动检查通过 |

已核对四个 job 的最终状态、逐步结果、完整日志和实际上传的 artifacts。
这些 CTest 耗时用于记录执行情况，不是文件传输性能成绩。

### 同一提交的可下载测试包

| 平台 | Artifact | 归档大小 |
|---|---|---:|
| Windows x64 | [LanChat-Lite-windows-x64](https://github.com/wenlong66/lan-chat-lite/actions/runs/40196244267/artifacts/12600392437) | 61,392,240 bytes |
| Apple Silicon Mac | [LanChat-Lite-macos-arm64](https://github.com/wenlong66/lan-chat-lite/actions/runs/40196244267/artifacts/12600031634) | 47,004,175 bytes |
| Intel Mac | [LanChat-Lite-macos-x86_64](https://github.com/wenlong66/lan-chat-lite/actions/runs/40196244267/artifacts/12600371893) | 46,781,081 bytes |

三份 artifact 均来自提交 `5642680df3bb9073a6106a60cc6f75a18a2c7f2b`，内含应用 ZIP 和
对应 `.sha256` 文件，当前到期日为 2026-11-08 UTC。下载需要 GitHub 登录。
过期后可在本仓库 Actions 中手动运行工作流或构建源码。上表大小不是运行内存。

首轮运行中，四个平台的编译与 CTest 已通过，但 Mac 的 `lipo` 参数顺序和 Windows
烟测的 `SYSTEMROOT` 环境变量大小写使打包步骤失败；两处均已修复并完成整轮复验。
Windows 包附有 `vc_redist.x64.exe`，新电脑可能仍需安装 Microsoft VC 运行库；
GitHub runner 已有系统运行库，干净实体机测试仍待进行。

## 本地 Linux 验证

环境：Ubuntu 24.04 x86_64、GCC 13.3.0、CMake 3.28.3、Qt 6.4.2、OpenSSL 3.0.13。
这是开发容器中的 Linux 测试；TCP 测试通过真实 loopback socket 运行。

| 检查 | 结果 | 说明 |
|---|---|---|
| Release 全量与最终增量构建 | 通过 | C++17 / Qt Widgets；未编入 GUI 测试代理 |
| `crypto` | 通过 | 16 个功能/数据用例；QtTest 合计 18 行含初始化和清理 |
| `clipboardimage` | 通过 | 6 个功能用例；QtTest 合计 8 行含初始化和清理 |
| `filetransfer` | 通过 | 22 个功能/数据用例；QtTest 合计 24 行含初始化和清理 |
| CTest 合计 | 3/3 套件通过 | 44 个实际用例；最后一次 Release CTest 用时约 3.34 秒 |
| 文件/消息传输 ASan + UBSan | 通过 | 同一组 22 个用例，开启泄漏检查；约 2.58 秒，无 sanitizer 报告 |
| `package-local` | 通过 | 生成 Linux 运行目录；仍依赖系统 Qt 库 |
| Linux 包启动/退出烟测 | 通过 | Xvfb 虚拟显示，`/loopback /noconfig /quit`，退出码 0 |
| 内存采样脚本短时自测 | 通过 | Linux 真实 PID、CSV 输出与拒绝覆盖检查 |
| CI 静态检查 | 通过 | YAML、Bash/Python 语法、三平台测试与打包流程检查 |

文件测试覆盖空文件、64 KiB 与 512 KiB 两侧边界、分片握手/回执、慢接收方背压、
错误摘要、未确认断开、老协议超时、取消保护原文件、不可写目标、源尺寸变化、
连续 24 次传输后对象与 socket 回收、长 UTF-8 路由头、超长数据与文字消息分片/粘包。

## 可复现命令

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DLMC_VERSION=0.1.0-alpha -DLMC_LITE_BUILD=ON \
  -DLMC_BUILD_LMM=OFF -DLMC_BUILD_TEST_AGENT=OFF
cmake --build build --parallel 4
ctest --test-dir build --output-on-failure --no-tests=error --timeout 90
cmake --build build --target package-local
(cd build/release && timeout 30 xvfb-run -a ./lan-messenger.sh /loopback /noconfig /quit)

cmake -S . -B build-asan -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DLMC_ENABLE_SANITIZERS=ON -DLMC_LITE_BUILD=ON \
  -DLMC_BUILD_LMM=OFF -DLMC_BUILD_TEST_AGENT=OFF
cmake --build build-asan --target tst_filetransfer --parallel 4
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 \
  ctest --test-dir build-asan -R '^filetransfer$' \
  --output-on-failure --no-tests=error --timeout 90
```

完整本地测试日志由 CTest 写入各构建目录的 `Testing/Temporary/LastTest.log`。
CI 运行后也会上传相应日志 artifact。构建目录与临时测试文件不提交进仓库。

## 观察到的警告

- 上游文字加密实现调用了 OpenSSL 3 标为 deprecated 的 RSA API；编译仍成功，迁移 EVP API 是后续任务。
- Linux 包烟测出现继承资源的 PNG 色彩配置和空 pixmap 警告，未导致此次启动失败。
- 容器缺少 XKB 开发组件，CMake 给出提示但成功生成构建；未将虚拟显示结果当成真实桌面验收。

## 必须另行验证

上述远程任务在各自 runner 上独立执行，网络回归使用本机 TCP 回环连接。
真实 Windows 与 Mac 之间的发现、互传、性能和长期运行仍需两台电脑验证。

下面这些项目仍需要两台真实电脑按 [测试计划](TEST_PLAN.zh-CN.md) 完成：

- Windows ↔ Apple Silicon/Intel Mac 的发现、聊天、图片与双向文件互传。
- 没有 Qt 开发环境的 Windows/Mac 上启动下载包，以及系统权限、防火墙和签名提示。
- 千兆有线与 Wi-Fi 的吞吐量、双向大文件哈希、CPU 与内存峰值。
- 托盘常驻、批量传输后的内存趋势、24 小时稳定性与睡眠恢复。
- 实际磁盘满、设备掉线、异常退出与超大文件的行为。

**50 MB 常驻与千兆有线 80 MB/s 是目标，没有在本记录中测得。**
自动测试没有发现错误不代表整个应用不存在漏洞或泄漏。文件传输目前仍是明文 TCP，
只在可信局域网测试；流式摘要校验不提供端点身份认证，也不提供被修改源文件的稳定快照。

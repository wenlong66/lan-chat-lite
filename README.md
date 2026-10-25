# LanChat Lite

**Windows 与 macOS 之间的局域网单聊、截图和文件直传。**

[![Build and test](https://github.com/wenlong66/lan-chat-lite/actions/workflows/ci.yml/badge.svg)](https://github.com/wenlong66/lan-chat-lite/actions/workflows/ci.yml)
[![License: GPL-3.0](https://img.shields.io/badge/License-GPL--3.0-blue.svg)](LICENSE)

当前阶段：**0.1.0-alpha 开发预览版**。C++17 / Qt 6 Widgets，无账号、无中心服务器。目标是小界面、低常驻占用和可靠的大文件传输；50 MB 常驻和千兆有线 80 MB/s 是验收目标，尚不是 Windows/Mac 实测成绩。

## 本版做了什么

- 保留局域网自动发现、一对一文字聊天、拖拽文件、本地记录与托盘功能。
- 默认隐藏广播、公共聊天室等入口，简化聊天工具栏。
- Ctrl/Cmd+V 或右键粘贴截图，预览确认后发送 PNG 附件，保留未发送的文字草稿。
- 文件按 64 KiB 分块，单传输 socket 缓冲队列限制为 512 KiB；修复上游文件缓冲与文件对象的生命周期问题。
- 接收端先写临时文件，提交成功后返回 SHA-256 摘要；发送端核对摘要后才显示完成。
- 中断、取消、读写错误和超时不报成功；失败接收不覆盖原有目标文件。
- 增加 QtTest 回归测试、Windows/macOS/Linux CI 和可复现的内存采样工具。

## 如何开始测试

1. 阅读 [开始使用与开发](docs/GETTING_STARTED.zh-CN.md)。
2. 在 [Actions](https://github.com/wenlong66/lan-chat-lite/actions) 中选择一个通过检查的提交，下载 Windows x64 与相应 Mac 架构的构建包。没有成功运行就没有可用安装包，CI 配置本身不是构建通过证明。
3. 两台机器运行同一提交的 LanChat Lite，先测文字、图片和小文件，再按 [测试计划](docs/TEST_PLAN.zh-CN.md) 测大文件、双向速度和长期内存。
4. 将系统、网络、提交 SHA、测量方法和结果记入测试记录，再决定是否进入日常使用。

测试包暂沿用上游可执行文件名 `lmc.exe` / `LAN-Messenger.app`；界面标题与配置目录已区分为 LanChat Lite。现有上游界面以英文等语言为主，完整简体中文界面仍待补齐。

## 开发构建

要求 C++17、CMake 3.16+、Qt 6（Core / Gui / Widgets / Network / Xml / LinguistTools / Test；Unix 还需要 Multimedia）、OpenSSL 开发库。参考 Windows/macOS Qt 版本为 6.8.3，Linux 使用 Ubuntu 24.04 的 Qt 6.4.2。

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DLMC_VERSION=0.1.0-alpha -DLMC_LITE_BUILD=ON
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure --timeout 90
cmake --build build --target package-local
```

完整依赖、Windows/MSVC、Mac 打包和防火墙说明见 [入门文档](docs/GETTING_STARTED.zh-CN.md)。生产包不包含上游 GUI 测试代理；需要开发该代理时显式开启 `LMC_BUILD_TEST_AGENT`。

## 当前边界

- **文件传输是明文 TCP，没有 TLS、设备身份认证或断点续传。只在可信局域网中测试。** SHA-256 用于内容一致性校验，不提供身份验证。
- 文件使用新的 `START2` / `DONE2` 完成确认，双方必须运行 LanChat Lite；不承诺与原版 LAN Messenger 文件互通。
- 图片目前作为 PNG 附件接收，尚未提供接收后的聊天气泡内预览。
- 截图限制为 16 MiPixels / 64 MiB 解码数据；超限图片请作为普通文件发送。临时截图正常完成或退出时清理；崩溃可能留下临时文件。
- Windows/Mac 安装包没有商业签名或 Apple 公证。自动测试通过不替代真实 Windows↔Mac 网络测试。
- 精简模式先隐藏非必要入口，上游相应代码仍在。更深入删减须由实际内存剖析结果决定。

## 计划与来源

- [测试计划](docs/TEST_PLAN.zh-CN.md)
- [开发路线图](docs/ROADMAP.zh-CN.md)
- [初始验证记录](docs/VALIDATION.zh-CN.md)
- [来源与协议变更](docs/PROVENANCE.md)
- [Codex 开发规则](AGENTS.md)

本项目从 [LAN Messenger](https://github.com/lanmessenger/lanmessenger) **v1.3.9**，提交 `a4d364a384d5c8c615f443c062a77c00ca4412b2` 派生。保留原作者版权声明和 [GNU GPL v3](LICENSE)；新增源文件按 GPL-3.0-or-later 许可。它不是上游官方发布版。

## English

LanChat Lite is an experimental C++/Qt desktop messenger for trusted local networks. It focuses on one-to-one text, clipboard screenshots and bounded streaming file transfers between Windows and macOS. This alpha is derived from LAN Messenger v1.3.9. File payloads are not encrypted; both endpoints must use LanChat Lite. Performance figures are targets until measured on real devices.

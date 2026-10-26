# LanChat Lite

**Windows 与 macOS 之间的局域网单聊、截图和文件直传。**

[![Build and test](https://github.com/wenlong66/lan-chat-lite/actions/workflows/ci.yml/badge.svg)](https://github.com/wenlong66/lan-chat-lite/actions/workflows/ci.yml)
[![License: GPL-3.0](https://img.shields.io/badge/License-GPL--3.0-blue.svg)](LICENSE)

当前阶段：**0.1.0-alpha 开发预览版**。C++17 / Qt 6 Widgets，无账号、无中心服务器。目标是小界面、低常驻占用和可靠的大文件传输；50 MB 常驻和千兆有线 80 MB/s 是验收目标，尚不是 Windows/Mac 实测成绩。

> **当前仍为测试版。** 本地 `fix1` 已修复同名文件接收与图标临时文件问题，并取消 Lite 联系人分组、保留旧分组数据。Windows Release 构建及 5/5 套 CTest 通过；未完成本轮跨平台 CI 或真实 Windows↔Mac 验收。此前未带 `fix1` 的免安装包和旧 Actions 包仍有上述三项风险，不能因源码已修复就当作修复版使用。详见[验证记录](docs/VALIDATION.zh-CN.md)。

## 本版做了什么

- 保留局域网自动发现、一对一文字聊天、拖拽文件、本地记录与托盘功能。
- 默认隐藏广播、公共聊天室等入口，简化聊天工具栏。
- Ctrl/Cmd+V 或右键粘贴截图，预览确认后发送 PNG 附件，保留未发送的文字草稿。
- 文件按 64 KiB 分块，单传输 socket 缓冲队列限制为 512 KiB；修复上游文件缓冲与文件对象的生命周期问题。
- 接收端先写临时文件，提交成功后返回 SHA-256 摘要；发送端核对摘要后才显示完成。
- 中断、取消、读写错误和超时不报成功；失败接收不覆盖原有目标文件。
- 同名接收自动编号为 `文件名_1.ext`、`文件名_2.ext`，同时避开尚未完成的接收任务；图标探测使用独立临时目录。
- Lite 联系人直接平铺，不再提供分组或迁移分组数据，已有分组文件保留不动。
- 增加 QtTest 回归测试、Windows/macOS/Linux CI 和可复现的内存采样工具。

## 如何开始测试

1. 阅读 [开始使用与开发](docs/GETTING_STARTED.zh-CN.md)。
2. 在 [Actions](https://github.com/wenlong66/lan-chat-lite/actions) 中选择一个通过检查的提交，下载 Windows x64 与相应 Mac 架构的构建包。没有成功运行就没有可用安装包，CI 配置本身不是构建通过证明。
3. 两台机器运行同一提交的 LanChat Lite，先测文字、图片和小文件，再按 [测试计划](docs/TEST_PLAN.zh-CN.md) 测大文件、双向速度和长期内存。
4. 将系统、网络、提交 SHA、测量方法和结果记入测试记录，再决定是否进入日常使用。

当前源码已统一 Windows/macOS 的 LanChat Lite 名称与双气泡图标：新构建分别运行 `LanChatLite.exe` / `LanChat Lite.app`。联系人列表、聊天工具栏和输入区采用更新后的布局；原有配置目录保持不变。此前 `fix1`、`zh1`、`zh2` 包仍保留旧文件名与界面，需重新构建或下载对应新提交的产物。源码现已提供简体中文界面（包括截图和传输提示、Qt 标准对话框），可使用本地 `zh1` 中文包或重新构建；此前的 `fix1` 包不会自动变为中文版。未保存语言选择时按系统区域选择可用语言，不支持的区域回退英文；可在首选项中选择“简体中文”或 English，保存后立即切换并保留选择。原作者署名、致谢与许可证保留原文。

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

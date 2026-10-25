# LanChat Lite：开始使用与开发

版本定位：**0.1.0-alpha 开发预览版**。目标是 Windows 与 macOS 在同一局域网内发现设备、单聊、粘贴图片和传文件。代码从 LAN Messenger `v1.3.9`（`a4d364a384d5c8c615f443c062a77c00ca4412b2`）派生，保留 GPL 开源许可证与原作者署名。

## 1. 先取得两端的测试包

打开本仓库的 **Actions → Build and test LanChat Lite**，选择对应提交且全部必要检查成功的一次运行。在页面底部 Artifacts 下载：

| 电脑 | 下载产物 |
|---|---|
| Windows 10/11 x64 | `LanChat-Lite-windows-x64` |
| Apple Silicon Mac（M 系列） | `LanChat-Lite-macos-arm64` |
| Intel Mac | `LanChat-Lite-macos-x86_64` |

Artifacts 是开发构建，有保留期限；下载通常需要登录 GitHub。若对应运行未完成或失败，不存在可用产物，应先查看失败日志。仓库中的 CI 配置不代表 Windows/Mac 构建已经实际通过。

下载 Artifact 后先解压外层归档，再解压其中的 ZIP；同时提供 `.sha256` 校验值。Windows 包中运行 `lmc.exe`；Mac 包中的应用暂沿用 `LAN-Messenger.app` 文件名，界面为 LanChat Lite，保留文件名可减少上游打包路径变更。请将 Mac 应用放到独立测试目录，不覆盖已有的原版 LAN Messenger。

Windows 首次启动若提示缺少 `VCRUNTIME` / `MSVCP`，先安装包内的 `vc_redist.x64.exe`（Microsoft Visual C++ x64 运行库），再启动应用。Qt 的部署工具在本次构建中附带这个安装程序；GitHub 的 Windows runner 已有 VC 运行库，因此包启动烟测仍不能代替全新 Windows 系统的验证。

测试包没有付费代码签名或 Apple 公证；Mac 打包步骤仅做运行所需的 ad-hoc 签名。如系统阻止首次打开，可在核对来源后按系统的“隐私与安全性”页面提示允许该应用。不要全局关闭系统安全检查。

## 2. 两台真实电脑的第一次测试

1. **两端使用同一次提交构建的 LanChat Lite**，接入同一路由器下可互通的网络。先用有线网络更容易定位问题。
2. 首次启动分别设置可区分的昵称，例如 `Windows-PC` 与 `Mac-mini`。
3. 允许应用访问本地网络；Windows 防火墙只对实际使用的专用网络开放应用。Mac 若出现“本地网络”权限提示，允许后重新启动应用。
4. 等待设备列表出现对方，双击对方打开一对一会话。
5. 双向各发送一条中文、英文与 emoji；截图后在输入区按 Ctrl+V / Cmd+V，确认预览后发送，接收端接受图片文件。
6. 拖入一个普通文件，接收端选择接受。核对两端完成状态，并比较文件 SHA-256。
7. 最后按 [测试计划](TEST_PLAN.zh-CN.md) 依次做大文件、批量文件和长时间常驻测试。

本版图片走 PNG 文件附件，不承诺微信式的聊天内原图预览。默认精简模式隐藏广播和公共聊天室等入口；这些上游实现仍保留在代码中。

### 发现不到对方怎么办

- 确认不是访客 Wi-Fi、开启了 AP/客户端隔离的网络或不同 VLAN；同一路由器名称不代表设备一定能互访。
- 暂时检查 VPN/TUN 是否接管了局域网路由，确保允许访问本地网段。
- 应用默认 UDP 发现端口为 **50000**；TCP 默认尝试 **50000**，占用时可能使用自动分配端口。优先按程序配置防火墙规则。
- 先在系统中确认双方的局域网 IP 与可达性，再检查应用设置。端口配置详见继承的 [INI 配置文档](ini-file.md)。
- 两端系统睡眠时无法正常收取消息或文件；测试期间保持唤醒。

## 3. 当前协议与使用边界

- 文件完成确认使用 Lite 的 `START2\n` / `DONE2 <SHA-256>\n` 握手。接收端提交临时文件成功后发送摘要，发送端核对流式摘要后才报告完成。**本版本只承诺 LanChat Lite ↔ LanChat Lite 文件互传**；原版 LAN Messenger 的文件协议不能作为兼容性目标。
- 发现与文字聊天沿用上游实现；当前未把原版混用列为验收通过项。
- 文件内容仍是**未加密的局域网 TCP 传输**，只在可信局域网中使用；上游的文字消息加密不等于文件也已加密。SHA-256 回执用于传输一致性核对，不提供端点身份认证或针对恶意篡改的保护。
- 当前没有断点续传。取消或中断后重新发送；失败不能显示为完成。
- 常驻约 50 MB、千兆有线＋SSD 大文件至少 80 MB/s 都是**待测目标**，不是已有成绩。

## 4. 从源码构建

先克隆本仓库，进入根目录。默认使用 Release 构建。Windows/macOS 的参考 Qt 版本为 **6.8.3**；Linux CI 使用 Ubuntu 24.04 的发行版 Qt 6。完整步骤以 [CI 工作流](../.github/workflows/ci.yml) 为准。

### Linux：本地开发与回归测试

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake ninja-build libssl-dev \
  qt6-base-dev qt6-base-dev-tools qt6-tools-dev qt6-tools-dev-tools \
  qt6-l10n-tools qt6-translations-l10n qt6-multimedia-dev libgl1-mesa-dev xvfb xauth
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DLMC_VERSION=0.1.0-alpha -DLMC_LITE_BUILD=ON \
  -DLMC_BUILD_LMM=OFF -DLMC_BUILD_TEST_AGENT=OFF -DBUILD_TESTING=ON
cmake --build build --parallel 2
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure --no-tests=error --timeout 90
cmake --build build --target package-local
```

Linux 运行文件在 `build/release`，需要系统 Qt 库。它是开发验证平台，本轮用户测试重点仍为 Windows/macOS。

### Windows：MSVC＋Qt

准备 Visual Studio 2022 C++ 桌面开发工具、CMake、Ninja、Qt 6.8.3 `msvc2022_64`、OpenSSL x64 开发库。可通过 vcpkg 安装 `openssl:x64-windows`；CI 会把其 include/lib/bin 复制到仓库的 `openssl/` 目录。构建产物和依赖目录不要提交到 Git。

在配置好 MSVC 环境的 PowerShell 中，先按实际安装路径填写 Qt 和 vcpkg。以下将 OpenSSL 头文件、import library 和 DLL 放入构建使用的 `openssl/` 目录：

```powershell
$lanChatQt = 'C:/Qt/6.8.3/msvc2022_64'
$lanChatVcpkg = 'C:/dev/vcpkg'
& "$lanChatVcpkg/vcpkg.exe" install openssl:x64-windows
if ($LASTEXITCODE -ne 0) { throw 'OpenSSL build failed' }
$lanChatSsl = "$lanChatVcpkg/installed/x64-windows"
New-Item -ItemType Directory -Force openssl/include, openssl/lib, openssl/bin | Out-Null
Copy-Item -Recurse -Force "$lanChatSsl/include/openssl" openssl/include/
Copy-Item "$lanChatSsl/lib/libcrypto.lib", "$lanChatSsl/lib/libssl.lib" openssl/lib/
Copy-Item "$lanChatSsl/bin/libcrypto-*.dll", "$lanChatSsl/bin/libssl-*.dll" openssl/bin/
$env:PATH = "$lanChatQt/bin;$pwd/openssl/bin;$env:PATH"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release `
  "-DCMAKE_PREFIX_PATH=$lanChatQt" "-DOPENSSL_ROOT_DIR=$pwd/openssl" `
  -DLMC_VERSION=0.1.0-alpha -DLMC_LITE_BUILD=ON `
  -DLMC_BUILD_LMM=OFF -DLMC_BUILD_TEST_AGENT=OFF -DBUILD_TESTING=ON
cmake --build build --parallel 2
$env:QT_QPA_PLATFORM = 'offscreen'
ctest --test-dir build --output-on-failure --no-tests=error --timeout 90
Remove-Item Env:QT_QPA_PLATFORM
cmake --build build --target package-local
& "$lanChatQt/bin/windeployqt.exe" --release build/release/lmc.exe
```

### macOS：Apple clang＋Qt

准备 Xcode Command Line Tools、CMake、Ninja、Qt 6.8.3（包含 Multimedia、ShaderTools、ImageFormats）和 OpenSSL。以下按 Qt 的常见安装路径示例，需按实际目录修改：

```bash
brew install cmake ninja openssl@3
LCHAT_QT="$HOME/Qt/6.8.3/macos"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$LCHAT_QT" \
  -DOPENSSL_ROOT_DIR="$(brew --prefix openssl@3)" \
  -DLMC_VERSION=0.1.0-alpha -DLMC_LITE_BUILD=ON \
  -DLMC_BUILD_LMM=OFF -DLMC_BUILD_TEST_AGENT=OFF -DBUILD_TESTING=ON \
  -DLMC_RELEASE_DIR="$PWD/lmc/release"
cmake --build build --parallel 2
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure --no-tests=error --timeout 90
cmake --build build --target package-local
cd lmc/setup/mac
QTDIR="$LCHAT_QT" bash ./stage-mac-app
```

此脚本会将 Qt 依赖放入 `.app` 并做本地 ad-hoc 签名。最终可运行的包是 `lmc/setup/mac/LAN-Messenger.app`。在对应架构的 Mac 上构建即可；首版不生成 Universal 合并包。

## 5. 如何继续交给 Codex 开发

先让 Codex 阅读根 README、[测试计划](TEST_PLAN.zh-CN.md) 和 [路线图](ROADMAP.zh-CN.md)，选一个任务完成代码、对应回归测试和记录，再提交。不要把“编译成功”写成“Windows 与 Mac 的真实局域网已通过”。测试记录必须写系统版本、提交 SHA、Qt 版本、网络连接方式、文件大小、双方哈希及失败日志。

参考来源：[上游稳定版](https://github.com/lanmessenger/lanmessenger/releases/tag/v1.3.9)、[Qt Windows 部署](https://doc.qt.io/qt-6/windows-deployment.html)、[Qt macOS 部署](https://doc.qt.io/qt-6/macos-deployment.html)。

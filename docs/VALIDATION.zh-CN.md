# 初始验证记录

初始日期：2026-10-25。版本：**0.1.0-alpha**。各阶段证据按日期保留；最新本地 `fix1` 修复与验证见末尾 2026-10-26 小节，不能将旧 CI 成绩套用到新修改。

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

## 2026-10-25 Windows 免安装交付复核：暂缓交付

本次检查环境为 Windows 11 Pro x64（10.0.22631）、Windows PowerShell 5.1、Python 3.11。
起始 HEAD 为 `99ece6dca6dc65f7774bf1d9aa59cdbb4673eaca`；应用 C++ 与上述成功 CI
提交 `5642680` 一致。本轮只调整打包配置与文档，没有修复或重新编译应用 C++。

### 源码核实的三项阻塞

这些结论来自受限路径的源码检查，**未执行数据丢失动态复现**，不是全面安全审计。

1. **并发同名接收会覆盖已成功接收的文件。** 第一个 `report.bin` 尚未完成时，接受
   第二个同名文件；`getFreeFileName()` 只检查最终文件是否存在，没有预留活动任务的
   目标路径。`QSaveFile` 提交前目标可以不存在，两任务选中同一路径，后提交者替换
   前者，两个任务却都可能得到匹配摘要并显示成功。
   依据：[文件命名](../lmc/src/messaging/filemessagingproc.cpp#L467)、
   [接收目标选择](../lmc/src/messaging/filemessagingproc.cpp#L326)、
   [文件提交](../lmc/src/network/netstreamer.cpp#L337)。
2. **获取接收文件图标会误删临时目录同名文件。** 接收目标尚不存在时，
   `getIcon()` 对临时目录中的同名路径直接 `WriteOnly` 打开并删除，没有排他创建。
   例如临时目录原有可写 `notes.txt`，接收同名附件会先截断并删除它，取消传输也无法恢复。
   依据：[创建接收条目](../lmc/src/gui/files/transferwindow.cpp#L98)、
   [图标实现](../lmc/src/gui/files/transferwindow.cpp#L348)。
3. **配置迁移会反复清空联系人分组。** 保存的显示版本 `0.1.0-alpha` 低于上游迁移
   门槛 `1.2.25`，每次启动重跑分组迁移，从 preferences 的已移除字段读取空数组并
   覆盖 `group.cfg`；创建分组并正常退出后，下次启动仍可能清空分组及联系人映射。
   依据：[迁移入口](../lmc/src/core/settings.cpp#L52)、
   [分组迁移](../lmc/src/core/settings.cpp#L377)、
   [保存分组及版本](../lmc/src/messaging/messaging.cpp#L206)。

现有传输套件直接连接 sender/receiver，没有覆盖完整 offer/accept 命名与接收 GUI 路径，
因此旧 CI 3/3 通过不能排除上述问题。应先补复现回归并修复，再重新构建完整交付包。

### 本轮已执行与未执行

| 检查 | 结果与边界 |
|---|---|
| GitHub CI 状态与日志复核 | 运行 `40196244267` 四个平台 success；下载 Windows 原始 CTest 日志，crypto、clipboardimage、filetransfer 均为 Test Passed |
| 本地重新 Release 构建 / CTest | **未执行**；本机未发现可用 Qt SDK 和 CMake/Ninja，复用同应用源码的 CI 产物，不冒充本地构建通过 |
| 原始 Windows ZIP 校验 | SHA-256 与随包校验值一致：`dd62bc9094d6b320ebcb867e8d4661e413ee5f03b6bb77230987019c90441fb7` |
| app-local CRT 来源与校验 | 微软 Visual Studio 官方 14.44.35211 X64 base VSIX；包 SHA-256 为 `4aaf54db0bfc9435f7c3660e1a00237a4b556042bfeea64bde44c2e0194e6ee5`；10 个 Release CRT DLL 全为 x64，微软 Authenticode 均 Valid |
| 免安装运行目录 staging | 已完成；保留 CI 原 EXE 和 Qt/OpenSSL，附加 app-local CRT，未纳入原 VC 安装程序；未发现开发测试 executable |
| 清理开发 Qt 环境后的启动/退出 | `/loopback /noconfig /quit` 退出码 0；**事后发现该命令会修改当前账户设置，不能作为安全的日常账户烟测** |
| 可见 GUI 与菜单交互 | **未通过验证**；三次检查未找到可见主窗口，后核实默认 AutoShow=false，仅有托盘；不能记录为 GUI 通过，也不能单凭此认定崩溃 |
| 修改后的 CI 配置 | YAML、Python 语法、Windows PowerShell 步骤解析通过；新增 app-local CRT 文件存在检查；**尚未实际重跑 CI** |
| 最终绿色 ZIP / 发布 | **暂停**；只有排查用解压目录，没有生成新的最终交付 ZIP，没有提交、推送或发布 Release |

CRT 包来自 [微软官方下载](https://download.visualstudio.microsoft.com/download/pr/45d3b8dd-bced-4b37-9974-142f748d710c/4aaf54db0bfc9435f7c3660e1a00237a4b556042bfeea64bde44c2e0194e6ee5/Microsoft.VC.14.44.17.14.CRT.Redist.X64.base.vsix)，
依据微软 Visual Studio 17 release channel manifest 的摘要核验。未安装系统运行库，未从
System32 抽取 DLL 充当可再分发来源。

本轮命令和辅助脚本位于被 Git 忽略的 `artifacts/`；它们不是交付内容：

```text
gh run view 40196244267 --repo wenlong66/lan-chat-lite --json headSha,status,conclusion,jobs,url
gh run download 40196244267 --repo wenlong66/lan-chat-lite --name LanChat-Lite-windows-x64 --name test-log-windows-x64 --dir artifacts/ci-40196244267
python artifacts/download-ci.py
python artifacts/stage-portable.py
powershell.exe -NoProfile -File artifacts/verify-portable.ps1 -AppDir dist/LanChat-Lite-0.1.0-alpha-windows-x64-portable
git diff --check
```

**不要在日常账户重跑上述 GUI 辅助脚本或 `/noconfig` 命令。** 后续复现只能用独立
测试账户或可丢弃虚拟机，并先备份本应用配置。`/config=` 只是导入设置，不隔离 profile。

### 本机烟测副作用与失败记录

- `/noconfig` 实际在 `lmc.cpp:364–368` 删除本应用配置及头像，并调用
  `settingsChanged()`；`settings.cpp:249–256` 随后写入或删除当前账户的登录启动项。
  `/quit` 在该步骤之后处理，不能防止副作用。正常启动也默认开启登录自启动。
- 本轮已执行这些命令；事后只读确认 HKCU Run 下的 `LanChat Lite` 值指向本次
  `dist/LanChat-Lite-0.1.0-alpha-windows-x64-portable/lmc.exe`。没有启动前快照，
  不能确认是否覆盖过已有设置、头像或启动项旧值，也不能承诺自动还原。
  发现后已停止运行应用。经用户明确确认，仅删除该指向本次排查 EXE 的登录启动项：
  删除前再次核对路径一致，删除后确认该值不存在。没有恢复未知的旧值，也没有清理
  其他启动项、用户配置或聊天记录；此前烟测对配置的影响仍无法凭现有证据还原。
- 首次 Bash 的 PATH 混入 Windows 分号格式，`git`/`ls` 不可用，PowerShell 工具探测也
  首次失败；改为进程内明确 PATH 或绝对工具路径后完成，不修改系统 PATH。
- `gh run download` 长时间未完成，手动停止；改用有大小与摘要校验的分段下载并完成。
  后者第一次顺序下载较慢也被停止，保留已下载部分后续传完成；不是构建或应用测试失败。
- 静态配置检查首次命令的 `$crt` 被 Bash 展开，导致断言失败；修正检查命令后通过。
- 三次 GUI 辅助检查均因没有可见主窗口而失败；未进行真实文件收发，也未完成托盘交互
  或界面截图验收。没有残留本轮测试进程。

后续修复顺序：先修复三项数据安全问题并覆盖回归；在隔离环境重做 Release / CTest /
打包启动检查；最后交给两台真实设备验收。

### 补充：绿色版仅指免安装，保留原有行为

用户随后明确，本次“绿色版”只要求不安装即可直接运行，不要求数据随包便携，其他
行为保持不变。因此不新增 profile、启动器或配置重定向，也不改默认托盘及登录自启动。
仅对原 CI 应用补齐 app-local CRT 和说明文件，制作 **原功能免安装测试包**；本节补充
不撤销前述缺陷、历史验证失败及副作用记录，不将其宣称为已修复或适合日常使用的版本。

本次打包不再启动应用，不执行 `/noconfig`，不改用户配置或注册表。随包说明明确三项
数据风险、完整解压运行方式、原有用户目录和启动行为；所有生成文件仍留在被 Git
忽略的 `dist/`、`artifacts/`，不提交或发布。

**本地免安装测试 ZIP 已生成**，替代前述“暂停生成 ZIP”的阶段性状态；正式版与数据
安全验收仍未通过。本次命令为 `python artifacts/pack-portable.py`，结果如下：

| 项目 | 结果 |
|---|---|
| 产物 | [LanChat-Lite-0.1.0-alpha-windows-x64-portable.zip](../dist/LanChat-Lite-0.1.0-alpha-windows-x64-portable.zip) |
| ZIP 大小 | 35,773,082 bytes，约 35.8 MB / 34.1 MiB |
| 随包文件 | 147 个；唯一 EXE 为原 CI 的 `lmc.exe`，不含安装程序或开发测试代理 |
| 原应用一致性 | 原 CI 文件逐一 SHA-256 一致，仅不纳入 VC 安装程序、更新入门文档，另附说明与验证记录快照 |
| 新增 CRT 一致性 | 10 个 DLL 与已核验的微软官方包逐一 SHA-256 一致 |
| 架构 | 所有随包 EXE / DLL 均通过 PE Windows x64 检查 |
| ZIP 完整性 | CRC 检查通过；完整解压到临时目录后，147 个文件逐一 SHA-256 与打包目录一致 |
| 本次实际执行应用 / 构建 / CTest | 均未执行；沿用前述原 CI 的证据，不新增 GUI 或干净系统运行通过声明 |

ZIP SHA-256（外部同名 `.sha256` 文件已生成）：

```text
4be6ba6344d4fe7d54f11f805add474072fc37b025e956685edd1ada49261b1d
```

原应用 EXE SHA-256：

```text
3617b8b9b6f86abe24b5afd472c7080b1a4b1631814e806f6d541582b495e1dc
```

包内验证记录为打包时快照，不包含 ZIP 自身的最终摘要；最终摘要以外部校验文件及本节
为准。没有修复上述三项缺陷，没有修改应用默认配置、数据目录或登录自启动代码。

## 2026-10-26：三项修复与 Windows fix1 验证

环境：Windows 11 Pro x64（10.0.22631），MSVC 19.39.33519 / Windows SDK
10.0.22621.0，Qt 6.8.3 MSVC2022 x64，OpenSSL 3.5.4，CMake 4.4.4，Ninja
1.13.2.git.kitware.jobserver-pipe-1。基于 `99ece6dca6dc65f7774bf1d9aa59cdbb4673eaca`
的本地未提交修改，**不是旧 CI 产物重新命名**。包名 `fix1` 用于区分本地修复构建，应用
显示版本仍为 `0.1.0-alpha`。本轮没有提交、推送、发布或执行远程 CI。

### 修复范围

1. 接收普通文件时同时检查磁盘上的名字和 `fileList` 中活动接收的目标路径；同名依次
   保存为 `name.ext`、`name_1.ext`、`name_2.ext`。处理无扩展名、点文件、中文名和
   Windows 大小写碰撞。复用已有传输列表，不改变 START2/DONE2 协议。
2. 附件图标探测在 `lanchat-lite-icons-XXXXXX` 独立临时目录中排他创建文件，使用
   `QTemporaryDir` 自动清理；创建失败时记录错误并返回通用图标，不再打开或删除公共
   临时目录里的同名文件。
3. Lite 不再使用联系人分组：列表平铺，分组操作隐藏且禁用，禁止组内拖动，不再读写
   分组或展开状态。跳过与 Lite 版本序列不兼容的上游迁移，保留旧 preferences 分组
   字段及 `group.cfg`。联系人选择、状态、排序、删除及内部广播消费者一并适配。

语言、默认 AutoShow=false、登录自启动和原有数据目录保持不变。中文由另一个会话
独立处理，不在本次包中。没有对外部程序抢占接收路径、多进程收件或不可信局域网作新增保证。

### 实际执行结果

| 检查 | 结果与范围 |
|---|---|
| Release 编译 | 通过，`LMC_LITE_BUILD=ON`，`LMC_BUILD_TEST_AGENT=OFF`，`LMC_BUILD_LMM=OFF` |
| 最终 CTest | **5/5 通过，24.07 秒**：crypto、clipboardimage、filetransfer、filereceive、contactgroups |
| 新增 filereceive | 9 个功能/数据用例通过；QtTest 11 行含初始化和清理。两次真实接受分配独立目标，再用两个 QSaveFile 提交不同内容；原文件与临时哨兵内容保留 |
| 新增 contactgroups | 9 个功能/数据用例通过；QtTest 11 行含初始化和清理。四种旧版本设置连续迁移两次按字节不变；平铺联系人、隐藏分组入口、选择及广播消费者通过 |
| 修复前对照 | 隔离源码副本只还原原命名、图标及迁移实现，保留回归。filereceive 出现 8 个断言失败，包含同路径分配及测试自己创建的临时哨兵被删除；单独运行迁移测试的四个数据行均失败 |
| 依赖部署 | `cmake --install` 与 `windeployqt --release --no-compiler-runtime` 成功；另附匹配的 OpenSSL 3.5.4 和已核验的 10 个微软 app-local CRT DLL，无需执行安装程序 |
| 生产 EXE 受限运行 | **帮助窗口检查通过**：仅传 `/?`，截图确认内容正常，向该测试窗口发送 Enter 后退出码 0；未启动 lmcCore 或发送 IPC 指令 |
| 运行依赖来源 | 移除进程级开发 Qt 环境、PATH 仅保留包目录和 Windows；实际进程中的 Qt Core/Gui/Widgets、qwindows、OpenSSL crypto、三个核心 CRT 均来自新包目录 |
| 日常账户状态 | 帮助窗口检查前后，本应用 preferences、头像、group.cfg、传输历史的存在性/摘要及登录启动项值一致；未执行删除、重置或恢复操作 |
| 完整 GUI / 网络 | **未执行本轮生产主窗口与真实收发验收**；离屏组件测试和帮助窗口不等于完整启动、托盘或真实 Windows↔Mac 验收 |
| 新 CI / sanitizer | 本轮均未执行；MSVC 环境没有运行项目 GCC/Clang ASan/UBSan，不能沿用此前旧版本 sanitizer 成绩 |

回归的配置和收件路径均为测试自己创建的临时目录；联系人测试还隔离 AppLocalDataLocation。
测试不启动 lmcCore，不调用登录自启动同步，也不使用真实聊天记录。新 filereceive 是接受
阶段命名、真实 QSaveFile 和图标回归，不冒充完整跨机器 START2/DONE2 验收。

### 命令与证据

依赖放在 Git 忽略目录，通过进程内 MSVC 环境使用，没有安装系统 Qt/CRT 或修改全局 PATH。
下面依赖路径均相对于仓库，命令在 MSVC x64 开发环境运行：

```text
cmake -S . -B build-fix1 -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=artifacts/qt-sdk/6.8.3/msvc2022_64 -DOPENSSL_ROOT_DIR=artifacts/build-tools/openssl-3.5.4 -DLMC_VERSION=0.1.0-alpha -DLMC_LITE_BUILD=ON -DLMC_BUILD_LMM=OFF -DLMC_BUILD_TEST_AGENT=OFF -DBUILD_TESTING=ON
cmake --build build-fix1 --parallel 2
ctest --test-dir build-fix1 --output-on-failure --no-tests=error --timeout 90
cmake --install build-fix1 --config Release --prefix dist/LanChat-Lite-0.1.0-alpha-fix1-windows-x64-portable
windeployqt --release --no-compiler-runtime dist/LanChat-Lite-0.1.0-alpha-fix1-windows-x64-portable/lmc.exe
powershell.exe -NoProfile -File artifacts/smoke-fix1-help.ps1 -Runtime dist/LanChat-Lite-0.1.0-alpha-fix1-windows-x64-portable
```

本地证据（不提交、不随包发布）：

- 初次构建与失败：`artifacts/fix1-release-ctest.log`。
- 最终 Release/CTest/部署：`artifacts/fix1-final-build-stage.log`。
- 新增用例明细：`artifacts/fix1-filereceive.txt`、`artifacts/fix1-contactgroups.txt`。
- 修复前断言：`artifacts/fix1-baseline-filereceive.txt`、`artifacts/fix1-baseline-contactgroups.txt`。
- 帮助窗口：`artifacts/fix1-help-smoke.json`、`artifacts/fix1-help-window.png`，截图已实际查看。
- 工具准备：`artifacts/build-tools/setup-events.log`、`artifacts/build-tools/installed-paths-and-packages.json`。

### 失败、恢复及限制

- 首次常规 `cmake` 调用因工具不在 PATH 返回 127；随后在忽略目录补齐工具链，使用已有
  MSVC 构建成功。CMake wheel 下载慢曾中止并分段续传，完整 SHA-256 通过；三个 Qt
  归档也与官方 SHA-256 一致。
- 首次 Release 编译成功，但 CTest 为 4/5：联系人测试使用显示文字 `Available` / `Busy`
  充当协议状态码，导致状态索引为 -1，在启用提示时崩溃。测试改用已有 `statusCode`
  常量后全套通过；没有把此测试夹具错误伪称为已定位的生产启动崩溃。对照全套也曾
  触发该夹具问题，因此迁移缺陷的动态证据取自单独运行的迁移数据行。
- 一次内联 cmd 构建辅助命令因引号转义失败，打印诊断又遇到 GBK 编码错误；该次没有
  编译测试。改用独立 `.cmd` 后成功，未隐瞒或用失败输出宣称通过。
- CMake 提示可选 Vulkan headers 缺失和 CMP0177；未阻止构建。联系人离屏 QtTest 有
  字体目录、空 pixmap 和 propagateSizeHints 警告；这些测试不验证完整图标/排版外观。
- 获取依赖许可证时遇到已存在的空暂存目录及 raw 下载连接关闭；保留目录，改用上游
  固定 tag 的公共 API 取得 Qt/OpenSSL 许可证，没有改写旧包。
- 本轮没有使用 `/noconfig`、`/nohistory`、`/unsync`，没有终止用户正在运行的旧包，
  也没有修改防火墙、安全策略或登录启动项。此前已记录的历史烟测副作用仍不能还原。

新运行目录为 `dist/LanChat-Lite-0.1.0-alpha-fix1-windows-x64-portable`，没有覆盖旧程序。
用户随后授权删除旧包；已核对旧 ZIP 摘要和目录清单，并删除旧 ZIP 与 `.sha256`。
打包时旧 EXE 仍在运行，因此当时暂留解压目录；用户随后确认已退出并授权必要时退出。
再次检查确认旧进程已结束，核对目录仅含原有 147 个包文件且无额外数据或重解析点后，
已删除旧解压目录。删除前后新 EXE 与新 ZIP 摘要一致；未改登录启动项或用户数据。
新 EXE SHA-256：

```text
baacd9886389507df59171cb0eaedc17ed7abc844a002b92d3c533090cdde206
```

ZIP 核验结果在打包后追加；包内验证记录为打包时快照，最终 ZIP 摘要以外部 `.sha256`
及下面的结果为准。真实双机互传、干净 Windows、睡眠恢复、24 小时稳定性和性能仍待测。

### fix1 ZIP 最终核验

执行 `python artifacts/pack-fix1.py` 成功：

- [免安装 ZIP](../dist/LanChat-Lite-0.1.0-alpha-fix1-windows-x64-portable.zip)：
  **28,739,547 bytes，约 28.7 MB / 27.4 MiB，147 个文件**。
- 唯一 EXE 为本轮构建的 `lmc.exe`，与受测 EXE SHA-256 相同；没有测试代理或安装程序。
- 随包 EXE/DLL 均为 PE Windows x64；运行文件与部署清单逐一相符。
- ZIP CRC 通过，完整解压到独立临时目录后 147 个文件逐一 SHA-256 一致。
- [外部校验文件](../dist/LanChat-Lite-0.1.0-alpha-fix1-windows-x64-portable.sha256) 已生成；
  结果另存本地 `artifacts/fix1-package-result.json`。

ZIP SHA-256：

```text
da033948c76689073c1f94c35a680f0dd11885cdb7ceb9cde5ac3d95b9d5ac04
```

## 2026-10-26：简体中文（本地 zh1）

在上述 fix1 工作树基础上增加简体中文，保留三项修复及其回归，不覆盖已交付的 fix1 包。
先在独立工作树完成语言补丁，再以 `git apply --check` 检查并移回共享工作树；当时尚未提交、推送或发布，后续主干整合前复验见下文。

- 新增 `translations/lmc_zh_CN.ts`，当前源码提取 **384 条 / 31 个上下文**；主界面、设置、聊天、截图、传输、帮助和启动错误均有译文。
- 补标原先漏译的截图错误、字节单位、传输进度/时间单位、文件过滤器及主题预览；版权、许可证与致谢保留原文。
- 未保存选择时按系统区域选择已部署语言，不支持的区域/缺失应用语言包回退英文。首选项中的中文名称为“简体中文”；明确选择 English 时仍写入设置，避免下次被系统中文默认覆盖。
- 独立 `lupdate` 提取合并后的当前源码，与中文 catalog 的 `(context, source, comment)` 集合完全一致：384 对 384，无缺失或过时词条；XML、占位符、HTML/href、快捷键、空白、换行及 CLI 参数检查通过。
- 新增 `translations` QtTest，隔离 INI 设置并开启 Qt 测试数据模式；不启动 lmcCore、不调用自启动同步，不触碰日常配置/历史。

环境：Windows 11 x64、MSVC 14.39、Qt 6.8.3、OpenSSL 3.5.4、CMake 4.4.4、Ninja，Release；依赖仍位于忽略目录，未改全局 PATH 或安装系统运行库。

在进程级 MSVC 环境运行，依赖路径按本机实际位置填写：

```text
lupdate lmc/src lmcapp/src -no-obsolete -ts translations/lmc_zh_CN.ts
cmake -S . -B build-zh -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=<Qt SDK> -DOPENSSL_ROOT_DIR=<OpenSSL SDK> -DLMC_VERSION=0.1.0-alpha -DLMC_LITE_BUILD=ON -DLMC_BUILD_LMM=OFF -DLMC_BUILD_TEST_AGENT=OFF -DBUILD_TESTING=ON
cmake --build build-zh --parallel 2
ctest --test-dir build-zh --output-on-failure --no-tests=error --timeout 90
```

Release 构建通过；首次全部 CTest **6/6 通过，14.64 秒**。最终源码增量重建后再次全部通过，**6/6，37.23 秒**：crypto、clipboardimage、filetransfer、filereceive、translations、contactgroups；不是沿用 fix1 的成绩。中文 `lrelease` 输出为 **384 finished / 0 unfinished**，并部署了 `qtbase_zh_CN.qm`。

`translations` 覆盖缺失语言包、区域名规范化、系统默认/已保存选择、不支持区域英文回退、全部 TS 到 QM 的逐条一致性及占位符、中文截图错误/字节单位/Qt 取消按钮、设置窗口切换和英文选择持久化。证据位于忽略目录 `artifacts/zh-release-ctest.log`、`artifacts/zh-final-build-stage.log`、`artifacts/zh-translations.txt`；源码集合核对输入为 `artifacts/zh-current-source.ts`。

初始直接调用 SDK `lupdate` 失败（DLL 加载失败 / `0xC0000135`），原因是精简 SDK 缺少 `Qt6Qml.dll`；两次原生 cmd 引号尝试也未成功。随后仅在独立工作树的忽略目录补齐官方 Qt 归档中的该 DLL，官方 SHA-1 核验通过，`lupdate 6.8.3` 正常运行；未改原 SDK。生产程序本身不新增 QML 依赖。

CMake 有既有的可选 Vulkan headers/CMP0177 警告，离屏语言测试有字体目录及主题预览 `QFont::setPixelSize(-1)` 警告；未导致构建或断言失败，也不把这些测试当作中文字体/排版外观验收。

另外通过 `cmake --install build-zh` 与 `windeployqt --release --no-compiler-runtime` 暂存到独立目录 `dist/LanChat-Lite-0.1.0-alpha-zh1-windows-x64-portable`；仅复用 fix1 包中一致的 Qt/OpenSSL/app-local CRT 和许可证，不运行安装程序、不覆盖 fix1。压缩包核验结果在生成后追加，随包文档为打包前快照。

本轮未执行 macOS 构建、跨系统实体机互传、原生中文排版、长时间常驻或 sanitizer；这些不能由离屏语言组件测试替代。现有其他语言包仍按原有覆盖范围回退源文，未在本轮重新翻译。

### zh1 Windows 包核验

执行 `python artifacts/pack-zh.py` 成功，结果保存于 `artifacts/zh-package-result.json`：

- [中文版免安装 ZIP](../dist/LanChat-Lite-0.1.0-alpha-zh1-windows-x64-portable.zip)：**28,802,500 bytes，149 个文件**，与 fix1 分开保留。
- 包内 EXE 与本轮受测构建 SHA-256 一致；两个中文 QM 与构建输出一致。唯一 EXE 为 `lmc.exe`，无开发代理或安装程序；EXE/DLL 均为 Windows x64。
- ZIP CRC 通过，完整解压后逐文件 SHA-256 一致；[外部校验文件](../dist/LanChat-Lite-0.1.0-alpha-zh1-windows-x64-portable.sha256) 已生成。
- fix1 ZIP 打包前后仍为上文 `da033948…d9d5ac04`，未覆盖或改变；本轮没有启动生产主窗口、改写用户登录启动项、重置配置或终止用户程序。

中文包 EXE SHA-256：

```text
7f4547c9a8101bacb7f948d8638c28a453043d7c1880df571acced46af4e8d7a
```

中文包 ZIP SHA-256：

```text
75b124462b2b59bc83d5467f10b36c2b49e025031976e6a0a7abc592b16980f9
```

## 2026-10-26：主干整合前复验

按用户要求整合上述 zh1/fix1 源码、测试、中文词条、CI 和文档；整合基线为 `99ece6d`，集成分支为 `integrate/zh1`。本次范围仅本地提交与主干合并，不包括远端推送或发布。

- 按仓库 `.clang-format` 格式化本次修改的 C++ 行及三个新增回归测试；`git diff --check` 通过。
- 只读提交范围检查通过：25 个已跟踪文件修改、4 个新增源码文件，共 **29 个路径**；未发现新增凭据、运行时私有数据、二进制或硬编码的本机绝对路径。此检查不替代完整代码审查。
- 环境仍为上述 Windows 11 x64、MSVC 14.39、Qt 6.8.3、OpenSSL 3.5.4；复用 `build-zh` 的 Release 配置，`LMC_LITE_BUILD=ON`、`BUILD_TESTING=ON`、`LMC_BUILD_LMM=OFF`、`LMC_BUILD_TEST_AGENT=OFF`。

在进程级 MSVC 环境重新执行：

```text
cmake --build build-zh --parallel 2
ctest --test-dir build-zh --output-on-failure --no-tests=error --timeout 90
```

增量重建成功，CTest **6/6 通过，19.20 秒**：crypto、clipboardimage、filetransfer、filereceive、translations、contactgroups；构建和测试命令退出码均为 0。日志位于忽略目录 `artifacts/merge-main-release-ctest.log`，不是复用先前打包时的测试结果。

本次没有重新打包或覆盖已交付的 fix1/zh1 ZIP；构建输出、依赖、诊断文件和发行包不入库。未新增 macOS、真实跨机网络、中文原生排版、长时间常驻或 sanitizer 验证，上述限制仍然有效。

## 2026-10-26：主干中文免安装包（zh2）

按用户要求，从已合并的主干提交 `9245b33d3cf383dcef8fc35bbc94a08c2f460d41` 新建独立 Release 构建目录 `build-zh2` 和 zh2 发行目录，不覆盖既有 fix1/zh1 包。本次不修改应用源码、不提交、推送或发布。

环境：Windows 11 x64、MSVC 19.39.33519（工具集 14.39）、Qt 6.8.3、OpenSSL 3.5.4、CMake 4.4.4、Ninja；仅使用进程级工具链环境。实际依赖路径由本机忽略目录的构建脚本提供，未更改全局 PATH、系统运行库、防火墙或安全设置。

```text
cmake -S . -B build-zh2 -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=<Qt SDK> -DOPENSSL_ROOT_DIR=<OpenSSL SDK> -DLMC_VERSION=0.1.0-alpha -DLMC_LITE_BUILD=ON -DLMC_BUILD_LMM=OFF -DLMC_BUILD_TEST_AGENT=OFF -DBUILD_TESTING=ON
cmake --build build-zh2 --parallel 2
ctest --test-dir build-zh2 --output-on-failure --no-tests=error --timeout 90
cmake --install build-zh2 --config Release --prefix dist/LanChat-Lite-0.1.0-alpha-zh2-windows-x64-portable
windeployqt --release --no-compiler-runtime dist/LanChat-Lite-0.1.0-alpha-zh2-windows-x64-portable/lmc.exe
```

全新 Release 构建、CTest、安装暂存及 Qt 部署均退出 0；本轮 CTest **6/6 通过，13.75 秒**：crypto、clipboardimage、filetransfer、filereceive、translations、contactgroups。中文 `lrelease` 为 **384 finished / 0 unfinished**，同时部署 `qtbase_zh_CN.qm`。生产构建的 `LMC_BUILD_TEST_AGENT=OFF` 已核对。日志：`artifacts/zh2-release-ctest-stage.log`；本机入口：`artifacts/package-zh2.cmd`，均不入库。

仍有既有的可选 Vulkan headers/CMP0177 配置提示；其他区域中缺少对应 Qt 标准翻译的五种语言仍按既有机制保留英文，不影响已部署的简体中文。未为本次打包改动这些行为。

与此前一致，本轮自动测试不等于真实 Windows↔Mac 互传、完整原生中文排版、干净 Windows 机器、睡眠恢复、24 小时常驻、吞吐量或 sanitizer 验收。后续压缩包及受限帮助窗口检查结果在生成后追加；随包验证文档为压缩前快照，最终校验以外部 `.sha256` 为准。

### zh2 Windows 包及受限启动核验

执行 `python artifacts/pack-zh2.py` 成功，结果保存在忽略目录 `artifacts/zh2-package-result.json`：

- [新中文免安装 ZIP](../dist/LanChat-Lite-0.1.0-alpha-zh2-windows-x64-portable.zip)：**28,804,038 bytes，149 个文件**；[外部 SHA-256](../dist/LanChat-Lite-0.1.0-alpha-zh2-windows-x64-portable.sha256) 已生成。
- EXE 与本轮受测构建逐字节哈希一致；应用及 Qt 标准简体中文 QM 与本轮构建输出一致。唯一 EXE 为 `lmc.exe`，EXE/DLL 均为 x64，无测试代理或安装程序。
- app-local Qt/OpenSSL/VC CRT 沿用已核验的 fix1 对应版本；复用前逐个根目录 DLL 与已校验 fix1 ZIP 比较 SHA-256 一致，Qt 部署生成的同名 DLL 也与复用来源核对一致。保留原许可证、版权和来源说明。
- ZIP CRC 及完整解压逐文件 SHA-256 通过。既有 fix1 和 zh1 ZIP 的 SHA-256 在打包前后均未改变，未覆盖其运行目录。

随后在进程内清除 Qt 开发环境变量、限制 PATH 为包目录与 Windows 系统目录，通过复用的 `artifacts/smoke-zh2-help.ps1` 对新包执行 **`lmc.exe /?`**：

- 已事先检查当前启动链：帮助路径读取语言设置并安装翻译，不保存设置；在 IPC 发送和 lmcCore 初始化前返回。不使用 `/noconfig`、`/nohistory`、`/unsync` 或完整主窗口启动。
- 帮助窗口实际显示简体中文与“确定”按钮，已打开截图检查；向该测试进程窗口发送 Enter 后正常关闭，退出码 **0**。
- 实际加载的 Qt Core/Gui/Widgets、qwindows、OpenSSL crypto 及三项核心 VC CRT 均位于新包目录，不依赖 Qt SDK 或已安装的 VC CRT 路径。
- 前后快照核对本应用 INI、头像、分组、传输记录及本应用登录启动项未变；没有重置设置或终止其他用户进程。结果和截图：`artifacts/zh2-help-smoke.json`、`artifacts/zh2-help-window.png`。
- 此结果仅证明帮助窗口和所列运行依赖可用，**不是完整主窗口、网络、中文排版或干净系统验收**。

zh2 EXE SHA-256：

```text
46245fe24df894cbed6d892ff0b73694d3f495c9e210245b854a9014500e69d2
```

zh2 ZIP SHA-256：

```text
ceded294d6c468430f674e908a5aec161db254166822e74639bdeb22e4073a2d
```

## 2026-10-27：macOS arm64 中文测试包（zh2）

从提交 `b9cf38f0963fc2de6c97191bc2748c1c117c75bf` 构建；相较 Windows zh2
使用的 `9245b33`，只有验证文档差异，应用源码一致。本轮没有修改应用源码或协议。
用户已运行 Windows 包，并在本轮确认 **Mac 主窗口能看到 Windows 设备**。

环境：macOS 26.7 arm64、Xcode 27.0（27A266a）、Apple clang 21.0.0、Qt 6.8.3、
OpenSSL 3.6.1、CMake 4.4.4、Ninja 1.13.2，Release。本机 OpenSSL 的最低系统为
macOS 26.0，所以此包明确以 **Apple Silicon / macOS 26.0+** 为目标；实际运行
验证系统为 26.7，不是 Intel 或较旧 macOS 的测试结果。

Qt、CMake、Ninja 安装在忽略目录 `artifacts/macos-build/`。Qt 的 8 个官方归档均与
`download.qt.io` 的 SHA-256 一致；单连接下载较慢，改用校验 Content-Range 的并行
分段下载，合并后再次校验完整哈希，再交给 aqt 解压。归档清单和日志保存在
`qt-archive-manifest.json`、`qt-download-parallel.log`、`qt-install-verified.log`。

### 构建、回归和打包

实际命令由本地 `artifacts/macos-build/build-mac.sh` 执行，核心配置为：

```bash
cmake -S . -B build-macos-arm64 -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="<Qt 6.8.3 SDK>" \
  -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=26.0 \
  -DOPENSSL_ROOT_DIR="<OpenSSL 3.6.1 SDK>" \
  -DLMC_VERSION=0.1.0-alpha -DLMC_LITE_BUILD=ON \
  -DLMC_BUILD_LMM=OFF -DLMC_BUILD_TEST_AGENT=OFF -DBUILD_TESTING=ON \
  -DLMC_RELEASE_DIR="$PWD/artifacts/macos-build/stage/lmc/release"
cmake --build build-macos-arm64 --parallel 2
QT_QPA_PLATFORM=offscreen ctest --test-dir build-macos-arm64 \
  --output-on-failure --no-tests=error --timeout 90
cmake --build build-macos-arm64 --target package-local
```

- Release 构建通过，CTest **6/6 通过，27.07 秒**：crypto、clipboardimage、
  filetransfer、filereceive、translations、contactgroups。
- 中文翻译为 **384 finished / 0 unfinished**；应用及 Qt 标准中文 QM 均随包部署。
- 复用仓库 `stage-mac-app` 在忽略目录中的镜像暂存结构运行，随后复制到独立 `dist`
  目录；`macdeployqt`、`lipo -verify_arch arm64`、ad-hoc 签名及
  `codesign --verify --deep --strict` 均通过。
- 检查包内 **34 个 Mach-O 文件**，均包含 arm64，非系统链接依赖均能在包内找到，
  没有依赖本机 Qt SDK 或 Homebrew 的绝对库路径。主程序仅为 arm64；未加入测试代理。
- 日志：`artifacts/macos-build/release-ctest-stage.log`、
  `build-macos-arm64/Testing/Temporary/LastTest.log`、
  `artifacts/macos-build/bundle-audit.json`。

### 实际失败及处理

1. aqt 默认创建用户目录中的临时文件被沙箱阻止，改用项目忽略目录内的缓存和临时目录。
2. 同一个 Qt `qmake` 在沙箱内报缺少 `neon` 并退出 134，沙箱外正常返回 6.8.3；
   因此构建、测试与启动检查在沙箱外执行，没有更改系统安全策略。
3. 首次编译完成但链接失败：`ld: framework 'AGL' not found`。旧 Qt SDK 的
   `FindWrapOpenGL.cmake` 仍强制链接新 Xcode SDK 已移除的 AGL。仅对本地 Qt SDK
   应用与 [Qt 上游实现](https://github.com/qt/qtbase/blob/dev/cmake/FindWrapOpenGL.cmake)
   一致的 AGL 链接删除；保留 OpenGL，未改仓库 CMake 或应用源码。补丁为
   `artifacts/macos-build/qt-sdk-agl.patch`，失败日志为 `first-build-agl-failure.log`。
   修改 SDK 后重新配置、链接、测试和打包全部通过。
4. 初次编译包含既有 OpenSSL RSA 和 Qt 颜色 API 弃用警告，未作为本次打包扩展重构。

### 启动与双机验证边界

清除 Qt/DYLD 开发环境并限制 PATH 为系统目录后，对最终 App 启动 `/?`。
动态加载日志确认 Qt、OpenSSL crypto、Cocoa 平台插件及图片插件从最终 App 内加载。
原生界面读取工具发生超时，未取得截图，也未完成帮助窗口的人工关闭核验；帮助测试
进程在 180 秒超时后由测试脚本终止，**这项帮助窗口检查不记为通过**。

界面工具随后启动了正常主程序；用户明确反馈“能看到 Windows 设备”，因此记录
主窗口运行与 **Mac 发现 Windows** 为用户确认通过。随后对相同 App 执行只读
`/inst`，返回预期的 1（检测到正在运行的实例），stderr 为空；结果保存于
`artifacts/macos-build/instance-smoke.json`。主程序保留运行，方便继续双机测试。

未使用 `/noconfig`、`/nohistory` 或 `/unsync`；没有修改防火墙、安全设置或系统登录项。
Windows 发现 Mac、双向文字/图片/文件完整性、吞吐、睡眠恢复、24 小时常驻、Intel、
较旧 macOS 和本轮远端 CI 仍待测。测试包只有 ad-hoc 签名，没有 Apple 公证。

最终 App 位于 `dist/LanChat-Lite-0.1.0-alpha-zh2-macos-arm64/LAN-Messenger.app`；
ZIP 与 `.sha256` 同级。包内验证文档是压缩前快照，最终归档校验结果随后追加。

### Mac zh2 ZIP 最终核验

执行 `python3 artifacts/macos-build/pack-mac.py` 成功，结果记录于
`artifacts/macos-build/package-result.json`：

- [Mac arm64 ZIP](../dist/LanChat-Lite-0.1.0-alpha-zh2-macos-arm64.zip)：
  **47,215,061 bytes，约 47.2 MB / 45.0 MiB，175 个文件和符号链接**。
- ZIP CRC 通过；完整解压后，所有文件 SHA-256 与符号链接目标均与发行目录一致。
- 解压后主程序执行权限保留，`codesign --verify --deep --strict` 通过。
- [外部 SHA-256](../dist/LanChat-Lite-0.1.0-alpha-zh2-macos-arm64.sha256) 已生成。

ZIP SHA-256：

```text
05d16113e2476dc02b6a28d3b2e10e09a85021852424f2d2f5afd8b79c3e0d7e
```

## 2026-10-27：UI 与 Windows/macOS 品牌统一（ui1）

基于 `2bdfef3` 的本地工作区修改。环境：macOS 26.7 (25G229)、arm64、Apple clang 21.0.0、Qt 6.8.3、Release，`LMC_LITE_BUILD=ON`；生产测试代理及 LMM 均关闭。沿用上一轮已配置的 Qt/OpenSSL SDK。

### 改动范围

- 联系人主窗口采用更清晰的资料区、带文字的工具按钮、64/36 px 详略列表、选中/键盘焦点样式和无设备提示。
- 聊天窗口增加输入区留白与发送按钮层级，支持 Tab 聚焦、空格发送，工具栏显示文件/文件夹文字与同风格图标；Classic 聊天记录移除旧蓝色横线。
- 原创 SVG 生成 PNG、七尺寸 ICO 和最高 1024 px ICNS。Windows 文件、版本资源、打包脚本统一为 `LanChatLite.exe`；Mac bundle/可执行文件为 `LanChat Lite.app` / `LanChat Lite`；应用显示名称均为 LanChat Lite。配置身份与原作者署名保持不变。
- Windows/macOS CI 路径、NSIS 元数据和使用文档已同步；本节记录本地验证结果，提交推送后的远端 CI 状态以 Actions 为准。

### 执行与结果

以下命令使用本地已配置的 CMake/Ninja 可执行文件；Qt 在默认沙箱中存在前轮记录的 CPU 检测问题，本轮构建、测试与 GUI 核验在沙箱外执行。

```bash
cmake -S . -B build-macos-arm64 -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DLMC_VERSION=0.1.0-alpha -DLMC_LITE_BUILD=ON \
  -DLMC_BUILD_TEST_AGENT=OFF -DLMC_BUILD_LMM=OFF -DBUILD_TESTING=ON
cmake --build build-macos-arm64 --parallel 2
QT_QPA_PLATFORM=offscreen ctest --test-dir build-macos-arm64 \
  --output-on-failure --no-tests=error --timeout 90
cmake --build build-macos-arm64 --target package-local
```

- 最终 Release 构建通过；CTest **6/6 通过，10.14 秒**：crypto、clipboardimage、filetransfer、filereceive、translations、contactgroups。
- 简体中文 QM 为 **387 finished / 0 unfinished**。
- 临时 UI harness 直接链接生产 `lmc_core` 与相同资源，使用独立 QSettings、测试数据路径和合成联系人，不初始化网络。检查中文/英文即时切换、长联系人名、空列表、主窗口 320×400、聊天 440×360、空格发送、输入清空与多尺寸应用图标，全部通过。它只验证控件与发送信号，不代表网络收发。
- 使用 `QT_QPA_PLATFORM=offscreen` 的 Fusion 浅色/深色调色板、`QT_SCALE_FACTOR=2` 与原生 `cocoa` 分别渲染并检查截图。截图、harness、构建和测试日志位于忽略目录 `artifacts/ui-check/`。这些是 Mac 上的渲染结果，不是 Windows 实机截图。
- Mac 打包脚本部署 Qt 并 ad-hoc 签名成功；`codesign --verify --deep --strict` 通过。34 个 Mach-O 文件均含 arm64，非系统依赖在包内存在，无 Homebrew/Qt SDK 绝对链接路径；plist 名称和包内 ICNS 与源资源一致。
- 清理 Qt/DYLD 开发环境、将 PATH 限为系统目录后，最终包执行只读 `/inst` 返回 0、stderr 为空。未执行 `/noconfig`、未启动真实网络会话，未修改登录项或系统安全设置。
- `git diff --check`、Mac 脚本语法检查、QRC 全部资源路径检查及七尺寸 ICO 结构/PNG 数据检查通过。

### 过程中的失败与修正

- Context7 CLI 在沙箱外返回 `fetch failed`，改查 Qt 官方样式表、应用图标与高 DPI 文档，链接见 `BRANDING.zh-CN.md`。
- 图标生成工具首次编译因 Qt 头文件缺少 `__yield` 声明失败，编译参数加入 `-include arm_acle.h` 后成功，不修改 SDK 或生产源文件。
- 初次截图发现资料区被布局拉高、状态按钮有多余分割区，已限制资料区高度并使用整按钮状态菜单，再次渲染通过。
- 首版临时依赖审计把 `otool -L` 输出的文件标题误判为绝对依赖，改为仅解析依赖行后复核通过。
- 原生 Cocoa harness 出现一次 `IMKCFRunLoopWakeUpReliable` 系统日志，所有交互断言通过并正常退出；offscreen 有既有 `Sans Serif` 字体别名警告，无 Qt 样式解析错误。

### 测试包与边界

本机独立测试目录为 `dist/LanChat-Lite-0.1.0-alpha-ui1-macos-arm64/`，应用名 `LanChat Lite.app`。旧 zh2 包保留。Windows/macOS 都需要重新构建才能取得新版 UI 与图标，旧包不会自动变化。

Windows 本轮构建、真实 Explorer/任务栏图标、Windows DPI/主题切换、Intel Mac、真实双机聊天/传输、睡眠恢复和长期常驻均未验证。此次未改动网络或文件传输所有权，因此未额外执行 ASan/UBSan。Mac 包为本地 ad-hoc 签名，无 Apple 公证。

ui1 ZIP 最终核验：`dist/LanChat-Lite-0.1.0-alpha-ui1-macos-arm64.zip` 为 **46,430,312 bytes**，170 个文件/符号链接；CRC、解压后逐文件 SHA-256/符号链接一致性、执行权限及签名均通过。包内验证文档为压缩前快照。外部 `.sha256` 已生成，核验详情见 `artifacts/ui-check/package-result.json`。

```text
02addc848d9898713ee65c2bfbccd1daccfbbdaacfae148fe228e6177ce0f2b5
```

## 2026-10-27：macOS 简体中文完整性检查与语言声明修复

基于 `9e64325` 的本地工作区修改。环境：macOS 26.7 (25G229)、arm64、
Apple clang 21.0.0、Qt 6.8.3、Release；沿用已配置的本地 SDK，生产测试代理与
LMM 关闭。构建和 Qt 测试在沙箱外执行，原因同前述本机 Qt CPU 检测限制。

### 检查结果与修复

- 使用 Qt 6.8.3 `lupdate` 重新提取 `lmc/src`、`lmcapp/src`：当前源码与中文词库
  的 `(context, source, comment)` 集合均为 **387 条**，无缺失、过时、空白或
  unfinished 条目。保留的英文是键名、快捷键或省略号；未修改词库。
- 应用与 Qt 中文 QM 的构建、加载和 Mac 安装路径正确。Qt 中文包包含
  `MAC_APPLICATION_MENU` 的服务、隐藏、退出及 `QCocoaTheme` 的不保存译文。
- 发现生成的 Mac `Info.plist` 只有 `CFBundleDevelopmentRegion=English`，
  没有 `CFBundleLocalizations`。Foundation `NSBundle` 实测旧包声明只有 English，
  偏好 `zh-Hans` 时仍匹配 English。新增回归测试在修复前因此失败。
- 新增 Mac plist 模板，从实际 `lmc_*.ts` 文件生成全部 21 个语言标识，并保留
  英文开发语言及既有名称、标识符、图标和版本。修复后 Foundation 实测
  `zh-Hans → zh-CN`、`en → en-US`、`de → de-DE`，不支持的 `zh-Hant → en-US`。
  机制依据为 [Qt 6.8 macOS 国际化说明](https://doc.qt.io/qt-6.8/macos-issues.html#internationalization)，
  本轮通过 Context7 CLI 查询官方文档成功。

### 执行与结果

以下 `cmake` / `ctest` 使用项目忽略目录中已安装的可执行文件：

```bash
lupdate lmc/src lmcapp/src -no-obsolete -ts artifacts/mac-zh-check/current.ts
cmake -S . -B build-macos-arm64 -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DLMC_VERSION=0.1.0-alpha
cmake --build build-macos-arm64 --parallel 2
ctest --test-dir build-macos-arm64 --output-on-failure --no-tests=error --timeout 90
QT_QPA_PLATFORM=cocoa build-macos-arm64/tests/tst_translations
cmake --install build-macos-arm64 --prefix "$PWD/artifacts/mac-zh-check/lmc/release"
```

- Release 构建通过；最终 CTest **6/6 通过，7.07 秒**。新增测试核对生成包的语言
  列表与已部署词库一致，核对 Mac 菜单译文及切回英文。
- 原生 `cocoa` 平台单独运行翻译测试：**8 passed / 0 failed**（含初始化与清理）。
  设置窗口标题、语言选择持久化、Qt 取消按钮、词库占位符与上述 Mac 翻译查询通过。
- 在独立忽略目录复制并运行现有 `stage-mac-app`，部署 Qt 并签名成功；最终 App
  的 plist 语法、21 个语言声明、应用名称与标识符均核对通过，两个中文 QM 与测试
  使用的构建产物逐字节相同，`codesign --verify --deep --strict` 通过。
- 日志、Foundation 探针与核验 JSON 位于 `artifacts/mac-zh-check/`。独立测试 App
  为 `artifacts/mac-zh-check/lmc/setup/mac/LanChat Lite.app`；未覆盖旧 zh2/ui1 包。

### 验证边界

AppKit 原生文件对话框等系统控件由 macOS 按应用声明与系统语言偏好选取语言，
不保证响应应用首选项的即时切换；本轮验证了系统语言匹配和 Cocoa 翻译测试，
没有逐个打开原生文件对话框或人工遍历菜单，也没有执行跨系统收发测试。
Windows、Intel Mac 与远端 CI 未重跑；未改传输所有权，因此未额外运行 ASan/UBSan。
未启动真实聊天、修改用户语言偏好、登录项、防火墙或系统安全设置。

## 2026-10-27：修复设置关闭后的悬空指针（settingsfix1）

用户报告 Mac 打开设置、点击确定后窗口变白，关闭按钮也无法正常工作。
基线为 `cab9904`，保留此前 UI、品牌和 macOS 语言声明修改。

`lmcCore::showSettings()` 缓存设置窗口，在 `exec()` 返回 Accepted 后调用
`settingsChanged()`，随后还会再次使用同一窗口。设置窗口却启用了
`Qt::WA_DeleteOnClose`，导致确定、取消或关闭后对象已被销毁，缓存指针悬空；
确定后的设置刷新因此会访问已释放对象，后续再次打开同样不安全。

生产修复仅移除设置窗口的自动销毁标记，让已有父窗口负责生命周期，匹配现有复用
逻辑。未修改界面布局、网络协议或用户配置。Qt 的关闭/销毁语义通过 Context7 CLI
查询 [QDialog 官方文档](https://doc.qt.io/qt-6.8/qdialog.html#done) 核对。

### 复现与回归

在现有 `translations` 测试加入参数化用例
`settingsDialogCanBeReusedAfterClosing`：使用真实设置窗口的默认属性和 `exec()`，
点击确定、取消或调用窗口关闭，断言对象仍有效、窗口已隐藏、模态状态已解除；
确定后执行设置刷新，并再次打开和关闭同一窗口。配置仍隔离在临时目录中。
同时移除旧语言选择测试中手动禁用自动销毁的覆盖，避免掩盖生产行为。

```bash
QT_QPA_PLATFORM=offscreen build-macos-arm64/tests/tst_translations settingsDialogCanBeReusedAfterClosing
QT_QPA_PLATFORM=cocoa build-macos-arm64/tests/tst_translations settingsDialogCanBeReusedAfterClosing
cmake --build build-macos-arm64 --parallel 2
ctest --test-dir build-macos-arm64 --output-on-failure --no-tests=error --timeout 90
QT_QPA_PLATFORM=cocoa build-macos-arm64/tests/tst_translations
```

- 修复前，offscreen 和原生 Cocoa 后端均稳定 **3 个数据行失败**：关闭后
  `QPointer` 为空，错误为 `Closing preferences deleted the dialog still cached by lmcCore`。
  该测试捕获关闭路径的错误生命周期，不声称自动重现截图的每个渲染细节。
- 修复后 Release 构建通过，全部 CTest **6/6 通过，10.19 秒**。
- 原生 Cocoa 全套翻译/设置测试 **11 passed / 0 failed，3.698 秒**，包含三种关闭
  路径、重复打开、设置刷新及语言选择持久化。出现 macOS 输入法 mach port 提示，
  未导致断言失败。
- Qt 6.8.3、OpenSSL 3.6.1、macOS 26.7 arm64；沿用先前本地工具链及 AGL SDK
  修正。生产 `LMC_BUILD_TEST_AGENT=OFF`，未更改传输所有权，未额外运行 sanitizer。
- 新包包含当前名称、图标和 21 种语言声明；Qt 部署、34 个 Mach-O 的 arm64/依赖检查
  及 ad-hoc 签名验证通过。独立输出为
  `dist/LanChat-Lite-0.1.0-alpha-settingsfix1-macos-arm64/LanChat Lite.app`，
  最低 macOS 26.0；不覆盖旧 zh2/ui1 包或终止用户正在运行的应用。
- 日志和打包核验位于 `artifacts/settings-close-fix/`：`red.log`、`red-cocoa.log`、
  `build-green.log`、`ctest.log`、`green-cocoa.log`、`stage.log`、`bundle-audit.json`。

仍需用户退出旧进程后用新包复测实际聊天主窗口。Windows/Intel Mac、远端 CI、
双机传输与长期常驻未在本轮重测，不将隔离的 Cocoa 设置测试称为完整实机验收。

修复包最终核验：ZIP **46,462,529 bytes（约 46.5 MB）**，175 个文件和符号链接。
CRC、完整解压后的逐文件 SHA-256/符号链接比对、执行权限与解压后签名检查均通过。
ZIP 与外部 `.sha256` 位于 `dist/`；随包验证记录为压缩前快照。

```text
f133faf4d4aa216d1c1ab2e3f0e9294a6844136cb00a5f0b5d32189aee4781f6
```

### 2026-10-27：已安装应用的实际界面测试

通过原生界面工具打开“应用程序”中的修复版。运行进程路径和可执行文件 SHA-256
与已验证的 settingsfix1 包一致；结果记录于 `artifacts/settings-close-fix/installed-ui-test.json`。

- 从应用菜单打开首选项，直接点击“确定”：设置关闭，主窗口的头像、状态、签名栏和
  联系人区域恢复，未出现白屏。
- 再次打开首选项：成功。截图和辅助功能树显示完整设置表单；未主动改变任何设置值。
- 点击“取消”：设置关闭，主窗口正常恢复。此 Mac 设置表单没有独立的红色关闭按钮。
- 另外点击主窗口红色关闭按钮：随后界面工具无法读取窗口并超时，进程仍在运行。
  源码中该按钮调用 `hide()`，行为设计为隐藏到托盘；已请求用户恢复窗口确认，
  本次尚不将托盘恢复记为通过。没有强制退出应用或更改系统设置。

以上是安装后实际按钮操作的结果，不仅是 QtTest；跨机收发仍未在本轮执行。

后续用户明确反馈：白屏发生在首选项点击“确定”之后，而非托盘恢复。
此前辅助功能树中有控件不能单独证明实际绘制正常，故本问题仍待确认，不能据此宣布修复完成。
本轮重新核验 `/Applications/LanChat Lite.app`（PID 36306），分别通过辅助功能按钮
和截图坐标鼠标点击“确定”各一次；两次返回的实际截图均显示头像、签名和联系人 Long，
暂未复现白屏。未修改设置项、未重启进程，也未据未复现结果添加推测性修复。
正在确认用户触发前是否修改过设置，以补齐复现条件。

### 2026-10-27：设置确认后的 macOS 原生弹层白屏排查

用户确认“不修改设置，直接点确定”并保留现场后，原生截图和辅助功能树均证实
主窗口仅剩标题栏、内容全白。现场为 `/Applications/LanChat Lite.app`，PID 40481；
此前 PID 36306 已退出。`sample 40481 3` 显示主线程在正常事件循环中等待，
不是保存设置阻塞；仍可用应用菜单正常退出。工具冷启动后的重复点击未稳定复现，
因此不能把此前工具点击正常当作用户问题已消失。

候选修复只在 macOS 将设置的 `Qt::WindowModal` 改为 `Qt::ApplicationModal`，
保留父子所有权、`exec()` 与保存逻辑，避开原生 sheet。代价是设置打开期间阻止
应用其他窗口交互；Windows 行为不变。此为对原生弹层兼容问题的规避，尚未证明
Qt/AppKit 内部的具体根因。

验证环境仍为 macOS 26.7 arm64、Qt 6.8.3、Release：

```bash
artifacts/macos-build/venv/bin/cmake --build build-macos-arm64 --parallel 2
artifacts/macos-build/venv/bin/ctest --test-dir build-macos-arm64 --output-on-failure --timeout 90
QT_QPA_PLATFORM=cocoa build-macos-arm64/tests/tst_translations
```

- 自动回归新增 macOS 模态类型约束及关闭后父窗口可见、可用检查；修改前 3 项模态约束失败，
  修改后完整 CTest **6/6（9.18 秒）**、Cocoa 翻译/设置测试 **11/11**。
  这些断言锁定规避方式与生命周期，不能独立复现或证明原生屏幕白屏已修复。
- 重新部署 Qt 依赖并通过 ad-hoc 签名校验，将验证版替换至 `/Applications/LanChat Lite.app`；
  上一版保存在 `artifacts/settings-sheet-fix/previous-LanChat Lite.app`，未清空配置或记录。
- 安装版设置显示独立标题栏；工具实测“确定”及红色关闭按钮均返回有完整内容的主窗口。
  用户随后按原操作复测并确认“正常，白屏消失”。当前安装版可执行文件 SHA-256 为
  `d511064c8d40de71ef29c3fb13925b876fef8189435e9c796ecf882801dce108`。
  此结果支持原生 sheet 路径的规避有效；Windows 与跨机收发未重测。
- 采样、构建、红/绿测试、部署日志保存在 `artifacts/settings-sheet-fix/`。

## 2026-10-27：截图直接显示在聊天中

基于 `bfd7e9f` 工作区修改。macOS 26.7 arm64、Qt 6.8.3、Release；沿用已有本地
Qt/OpenSSL 工具链和 AGL SDK 修正，生产 `LMC_BUILD_TEST_AGENT=OFF`、`LMC_BUILD_LMM=OFF`。

### 行为与范围

- 剪贴板截图确认发送后携带可选 `contenttype=image/png` 元数据；新版自动接收，
  收到本地完成事件后显示缩略图。普通文件和无标记的旧版截图保持文件附件行为。
- 发送端显示本地图片及发送状态；只有现有 DONE2 摘要核对通过后才标记完成。
  发送成功的原图移动到接收目录下的 `Sent Images`，供点击打开及重载使用；若移动失败，
  本次会话继续使用临时文件，退出后可能失效。接收端使用本地实际落盘路径，忽略远端路径。
- 缩略图不超过 320×240，共享缓存不超过 8 MiB；PNG 输入文件不超过 65 MiB，
  解码前检查 16 MiPixels / 64 MiB 限制。缺失、损坏或超限时显示不可用。
  原图属于本地聊天文件，用户可清理；历史记录/导出 HTML 保存可点击的原图链接。
- 修正聊天文件状态只修改副本的问题，使完成/中断状态和实际保存路径可随聊天重载保留。
  文件的 START2/DONE2 实现与传输对象所有权未变，未额外运行 ASan/UBSan。

### 命令与结果

```bash
artifacts/macos-build/venv/bin/cmake --build build-macos-arm64 --parallel 2
QT_QPA_PLATFORM=offscreen build-macos-arm64/tests/tst_chatimage
artifacts/macos-build/venv/bin/ctest --test-dir build-macos-arm64 --output-on-failure --timeout 90
QT_QPA_PLATFORM=cocoa build-macos-arm64/tests/tst_chatimage
artifacts/macos-build/venv/bin/cmake --install build-macos-arm64 --prefix "$PWD/artifacts/chatimage/package/lmc/release"
```

- 修改前新增测试 **2 失败**：没有自动接受图片、发送端没有内联图片对象。
- 首次构建在沙箱中运行 Qt `uic` 报 `Incompatible processor ... neon`；同一命令
  在沙箱外通过 CPU 特性检查。测试扩展曾调用私有 `addFileTransfer` 编译失败，
  已改为在测试中提供文件请求回显，未开放生产私有 API。
- 最终 Release 构建通过，只有已有 Qt/OpenSSL 弃用警告；全部 CTest **7/7 通过，8.86 秒**。
- 新 `chatimage` 套件在 offscreen 和 Cocoa 均为 **10 passed / 0 failed**（含初始化与清理），
  覆盖自动接受、完成前不显示、失败/超限/损坏/删除、发送图片保留、重载、普通 PNG 附件不变、
  原图 URL 分发、真实 TCP PNG 收发、实际像素绘制和缩窄窗口。
- TCP 用例使用真实 `FileSender` / `FileReceiver` 和 loopback socket，验证收图完成事件触发
  渲染、双端完成及像素一致；文件请求信令由测试提供，不等同于双机设备发现和完整 GUI 互传。
- Cocoa 功能测试 **1.213 秒**；已检查生成的聊天区域截图，图片为可见缩略图，带完成状态。
  损坏 PNG 测试中的 libpng 报错是预期拒绝；offscreen 的字体/尺寸提示未导致失败。
- 本轮日志和渲染截图保存在忽略目录 `artifacts/chatimage/`。测试使用隔离设置和临时文件；
  未改变用户配置、已安装应用、系统权限或防火墙。

Windows、Intel Mac、远端 CI、Windows↔Mac 真实局域网、长期连续传图与 24h 内存仍待测。
两端必须更新后才能获得截图自动接收/内联显示；本轮通过不代表已安装的旧程序具有此功能。

### 本地 macOS 试用包

生成 `dist/LanChat-Lite-0.1.0-alpha-inline-images1-macos-arm64.zip`（46,491,234 bytes，
约 46.5 MB），最低系统版本沿用本地构建的 macOS 26.0。此包未替换 `/Applications` 中的旧版。
Qt 依赖部署、34 个 Mach-O 的 arm64/依赖检查、中文词库、ad-hoc 签名均通过。
首次使用 Python `shutil.copytree` 复制已签名 App 时，语言文件的签名扩展属性丢失，
导致后续解压签名检查失败；改用 `ditto --rsrc --extattr` 复制后重新打包。
最终 ZIP CRC、解压后 181 个文件/符号链接比对、执行权限与严格签名检查均通过。
清除 Qt/DYLD 开发环境后的只读 `/inst` 启动检查通过，未启动网络会话。
随包文档为打包前快照；核验详情见 `artifacts/chatimage/package-result.json`、
`bundle-audit.json`、`package-smoke.json`。

SHA-256：`6b7c915e571adf79e1855bd3b49a962ca4f163d7e4cf7f60b0e8bdee914d79b2`。

### 2026-10-28：替换安装版并启动

按用户明确要求，使用应用自身 `/term` 命令正常退出旧进程，确认退出后将已核验的
inline-images1 版本替换到 `/Applications/LanChat Lite.app`。旧版完整备份保存在
`artifacts/chatimage-install-20261028/previous-LanChat Lite.app`，未清空配置或聊天记录。
安装版严格签名检查通过，可执行文件 SHA-256 与新包一致：
`c6b5d97788739eda756f7c460559a4671f345d8f3a6b474788c1201c1362b9d3`。

已打开新版并通过原生界面树及截图确认主窗口正常显示：原昵称、头像、在线状态与
联系人区域可见。此时界面提示暂未发现设备；没有发送测试消息或宣称双机传图已通过。
安装和备份核验记录位于 `artifacts/chatimage-install-20261028/install.json`。

## 2026-10-28：截图右键复制图片与打开文件位置

用户确认聊天内截图可见后，要求右键复制与打开文件位置。本轮基于 `50838a3`，
只为聊天内图片增加独立右键菜单：复制完整原图到系统剪贴板，或调用系统文件管理器
打开该图所在目录（与文件传输窗口的打开目录行为一致）。文字区域保留原有菜单。
菜单按实际点击的图片取路径，支持滚动、连续多图和中文/空格/特殊字符路径；
原图已删除时禁用两项操作，解码失败时提示且保留原剪贴板内容。

原图读取复用缩略图的 PNG 校验和 16 MiPixels / 64 MiB 解码上限，不把缩略图当原图复制。
复制时才解码完整图片，8 MiB 缩略图缓存不变。网络协议和传输所有权没有变化。
Qt 接口通过 Context7 CLI 核对：QTextEdit `anchorAt`、QUrl `fromLocalFile`、QClipboard `setImage`。

### 构建与测试

环境：macOS 26.7 arm64、Qt 6.8.3、Release，生产测试代理和 LMM 继续关闭。
命令使用已配置的本地工具链，Qt 编译与原生 GUI 测试在沙箱外运行。

```bash
artifacts/macos-build/venv/bin/cmake --build build-macos-arm64 --parallel 2
artifacts/macos-build/venv/bin/ctest --test-dir build-macos-arm64 --output-on-failure --timeout 90
QT_QPA_PLATFORM=cocoa build-macos-arm64/tests/tst_chatimage
artifacts/macos-build/venv/bin/cmake --install build-macos-arm64 --prefix "$PWD/artifacts/chatimage-menu/package/lmc/release"
```

- 修改前新增的发送/接收图片菜单用例 **2 失败**：没有“复制图片”菜单项。
- Release 构建通过；最终全部 CTest **7/7 通过，7.94 秒**。
- `chatimage` 在 macOS 原生 Cocoa 下 **14 passed / 0 failed，1.947 秒**（含初始化与清理）。
  实际右键事件与菜单点击覆盖发送/接收、多图目标选择、800×400 原图尺寸与关键像素/透明度、
  从系统剪贴板粘贴到真实消息编辑器、目录 URL、空白区域菜单、缺失/损坏原图和普通文字复制。
  目录打开通过截获 QDesktopServices 的本地目录 URL 验证，没有自动打开测试目录。
- 文字复制测试最初在窗口显示前选中文字，被正常 resize 自动滚动清除选区；
  已调整为显示后选中，再验证菜单复制，未为此改动生产文字复制行为。
- 初次 Cocoa 套件虽最终通过，但在菜单回调内显示错误警告时耗时约 132.8 秒。
  改为菜单关闭后再执行图片操作和显示警告，最终同套测试约 1.95 秒通过。
  该结果支持避免嵌套原生菜单/弹窗的处理顺序，不声称定位 Qt/AppKit 内部根因。
- 损坏 PNG 用例中的 libpng 报错、offscreen 菜单警告及 macOS 输入法 mach port 提示
  未造成最终失败。原生测试结束时恢复原剪贴板 MIME 内容，未打印或保存其内容。
- 日志保存在 `artifacts/chatimage-menu/`。本轮未新增依赖、未改传输所有权，未另跑 sanitizer。
  Windows/Intel Mac、跨机收发及 QQ/微信等外部应用的实际粘贴仍待测。

沿用此前用户要求直接替换并打开新版的授权，部署 Qt 依赖并通过 ad-hoc 严格签名检查后，
正常退出旧进程，更新本机安装版。旧版保存在 `artifacts/chatimage-menu/previous-LanChat Lite.app`，
未清空用户配置或聊天记录。安装核验见 `artifacts/chatimage-menu/installed.json`。

更新后已打开应用，原生界面树和截图确认主窗口正常显示，昵称、头像和状态保留。
安装版可执行文件 SHA-256：`befcefca81a18c36568ad61f06f4127844d617967be55f824ffe62b942b12087`。

## 2026-10-28：文件/文件夹多选与拖入聊天框

基于 `50838a3` 及工作区已有的截图右键改动，增加文件与文件夹多选发送；聊天工具栏、
联系人菜单共享选择逻辑。文件继续使用原生多选对话框（测试环境可强制 Qt 对话框）；
文件夹使用 Qt 对话框，在列表和详细视图启用 Ctrl/Cmd、Shift 多选。

聊天输入区、消息区及窗口空白处接受本地文件/目录混合拖入，逐项沿用原来的发送请求。
修正输入框将文件 URL 插入为文字的问题；只接管含本地路径的 URL 拖拽，普通文字与
网页链接仍走原编辑流程。拖拽使用复制动作，保留文字草稿；PNG 不标记为截图消息。
过滤不存在的路径与非本地 URL，并按连接状态和对端文件/目录能力判断是否接受。
未修改 START2/DONE2、网络/磁盘处理或传输所有权，未额外运行 sanitizer。

Qt 接口通过 find-docs / Context7 CLI 核对，使用 Qt 6.8 官方文档中的
`QFileDialog::getOpenFileNames`、`selectedFiles` 和拖拽事件说明。
环境为 macOS 26.7 arm64、Qt 6.8.3、Release；`LMC_BUILD_TEST_AGENT=OFF`、
`LMC_BUILD_LMM=OFF`。沿用已有本地工具链，在沙箱外执行构建和测试：

```bash
artifacts/macos-build/venv/bin/cmake --build build-macos-arm64 --parallel 2
artifacts/macos-build/venv/bin/ctest --test-dir build-macos-arm64 --output-on-failure --no-tests=error --timeout 90
QT_QPA_PLATFORM=cocoa build-macos-arm64/tests/tst_attachmentinput
git diff --check
```

- 修改前新增回归 **8 passed / 13 failed**，复现文件/目录单选、输入框吞掉拖入条目、
  首 URL 无效导致整次拒绝，以及拖拽能力判断不完整。
- 最终 Release 构建通过，仅有本次重编译文件中已有的 `setNamedColor` 弃用警告。
  完整 CTest **8/8 通过，9.96 秒**；新增 `attachmentinput` 在 offscreen 下通过。
- 原生 Cocoa 下 **35 passed / 0 failed，1.820 秒**（含初始化/清理）：鼠标配合
  Ctrl/Cmd 在两种视图实际多选，覆盖聊天与联系人两个入口、文件/目录、取消选择、
  上次目录与草稿保留；通过 Qt 拖拽事件覆盖三个聊天区域、中文/空格路径、混合条目、
  无效/非本地 URL、对端能力、断线、复制动作、源文件保留和文字/网页链接编辑。
- 文件选择测试强制使用 Qt 对话框；拖拽通过 Qt 事件投递验证请求类型与路径，
  不等于已经验证 Finder/Explorer 原生拖拽、原生文件选择面板或完整双机传输。
  既有真实 loopback START2/DONE2 回归随完整 CTest 通过，不把旧 mock 当作协议证据。
- 日志保存在忽略目录 `artifacts/attachmentinput-*.log`；测试使用隔离设置和临时数据，
  未修改已安装应用、用户配置、防火墙或系统设置。`git diff --check` 通过。

Windows/Intel Mac 构建、Windows Explorer/Mac Finder 实际拖入、Shift 连选及实体
Windows↔Mac 批量收发和哈希核对仍待测；本次代码和本地构建尚未替换已安装的旧版应用。

## 2026-10-29：网络切换时 SOCKS5 UDP 崩溃

用户反馈切换网络时出现 macOS“意外退出”。本机 10 月 28 日一份、10 月 29 日两份
安装版崩溃报告均为 `EXC_BAD_ACCESS / SIGSEGV`，无效地址 `0x20`；前三层一致：
`QList<QRingChunk>::isEmpty` → `QRingBuffer::clear` →
`QSocks5SocketEnginePrivate::_q_emitPendingReadNotification`。

新增 `lannetwork` 回归，在测试进程内指定本地 SOCKS5 代理，握手完成后断开 UDP
关联的控制连接。未修改系统代理、网络、防火墙或真实用户配置。修改前运行：

```bash
build-macos-arm64/tests/tst_lannetwork udpSurvivesProxyDisconnect -nocrashhandler
```

退出码 **139**；生成的崩溃报告具有同一无效地址、前三层符号和符号偏移，与用户报告
一致。仅将 UDP 收发 socket 设为 `NoProxy` 后，同一复现 **3 passed / 0 failed**
（含初始化和清理，210 ms），且本地测试代理未收到关联请求。

原因是局域网 socket 沿用 Qt 默认代理选择；SOCKS5 UDP 控制连接断开后，Qt 6.8.3
上述通知路径访问仅为 TCP connect 模式分配的 `connectData->readBuffer`。
已核对 [Qt 6.8.3 源码](https://github.com/qt/qtbase/blob/v6.8.3/src/network/socket/qsocks5socketengine.cpp)
和 Context7 CLI 返回的 Qt 6.8 `setProxy` 文档。修复仅在 UDP 收发、TCP 监听、
发起聊天和文件连接的五个创建点显式设置 `QNetworkProxy::NoProxy`，不修改 Qt、
系统代理或传输协议。未更改传输所有权，未额外运行 sanitizer。

环境：macOS 26.7 arm64、Qt 6.8.3、Apple clang、Release，基于 `8199240` 工作区；
`LMC_BUILD_TEST_AGENT=OFF`、`LMC_BUILD_LMM=OFF`。使用已有本地工具链：

```bash
artifacts/macos-build/venv/bin/cmake --build build-macos-arm64 --parallel 2
artifacts/macos-build/venv/bin/ctest --test-dir build-macos-arm64 --output-on-failure --no-tests=error --timeout 90
git diff --check
```

- 首次在沙箱内构建时 Qt `uic -h` 报 `Incompatible processor ... neon`；改在沙箱外
  执行后构建成功，测试也在沙箱外运行。
- 新增 TCP 回归修改前 **3 项失败**，分别捕获监听、聊天、文件 socket 使用默认代理。
- 最终 Release 构建成功，完整 CTest **9/9 通过，11.30 秒**。
- `lannetwork` 覆盖代理断开复现、五轮 UDP 停止/启动及真实 loopback 收包、五轮 TCP
  停止/启动及连接关闭，以及代理存在时聊天/文件直连握手。现有 START2/DONE2
  真实 loopback 回归随完整套件通过，不以旧文件 mock 作为协议验证。
- 复现与构建/测试日志位于忽略目录 `artifacts/network-switch/`。

未自动切换用户正在使用的网络；真实 Wi-Fi/有线切换、VPN/TUN 路由变化、睡眠恢复、
Windows/Intel Mac 和实体 Windows↔Mac 收发仍待测。本修复验证消除了已重现的代理
断开崩溃，不将本地测试写成完整的实机网络切换验收。

### 本机修复包与启动核验

使用 `cmake --install` 暂存 Release，复用 `stage-mac-app` 部署 Qt 依赖；ad-hoc
签名及 `codesign --verify --deep --strict` 通过。确认旧应用未运行后，完整备份到
`artifacts/network-switch/previous-LanChat Lite.app`，再通过同卷重命名替换安装版，
安装后再次校验签名和可执行文件哈希。备份与安装核验见
`artifacts/network-switch/installed.json`；操作不涉及配置或聊天记录目录。

安装版可执行文件 SHA-256：
`c501faf876c29bcf93cc374b2ea6f14ae6a5ab20537ebf530a2a2b549f0640c7`。
启动后通过原生界面树确认主窗口显示、在线状态与联系人区域可见；没有发送测试消息，
真实网络切换仍等待用户复测。

## 2026-10-30：Cocoa 无障碍缓存释放崩溃

用户再次报告“意外退出”。本次 02:01:41 的报告来自上一轮已安装的修复版：应用 UUID
与安装版一致，可执行文件 SHA-256 仍为
`c501faf876c29bcf93cc374b2ea6f14ae6a5ab20537ebf530a2a2b549f0640c7`。
启动时间为前一天 16:45:20。本次属于另一条崩溃路径，主线程前八层为：

```text
objc_release +16
__RELEASE_OBJECTS_IN_THE_ARRAY__ +116
-[__NSArrayM dealloc] +148
-[QMacAccessibilityElement dealloc] +60
__RELEASE_OBJECTS_IN_THE_ARRAY__ +116
-[__NSArrayM dealloc] +148
AutoreleasePoolPage::releaseUntil +204
objc_autoreleasePoolPop +244
```

### 原生复现与原因

新增 `tst_macosaccessibility.mm`，使用 Cocoa 插件在测试进程内读取 QTreeWidget 的原生
无障碍行和单元格，再清空模型、销毁窗口并排空 autorelease pool。仅使用合成联系人，
不读取真实联系人、消息，也不修改系统辅助功能设置或启用 VoiceOver。

```bash
QT_QPA_PLATFORM=cocoa build-macos-arm64/tests/tst_macosaccessibility -nocrashhandler
QT_QPA_PLATFORM=cocoa build-macos-arm64/tests/tst_macosaccessibility contactRowsSurviveModelReset:single-column -nocrashhandler
```

- Qt 6.8.3 下，两次独立运行均退出 **139**；最小化为单列也立即崩溃，原生报告前八层
  与本次用户报告一致，包括各符号偏移。
- 在测试进程增加 `NSZombieEnabled=YES` 后退出 **133**，明确打印
  `-[QMacAccessibilityElement release]: message sent to deallocated instance`。
- 官方 [QTBUG-134784](https://qt-project.atlassian.net/browse/QTBUG-134784)
  及 [6.9 分支修复](https://github.com/qt/qtbase/commit/ae0bd2c52d5e7f112dcace027b4d279b532e1b86)
  对应同一缓存引用计数缺陷；已核对 Qt 6.9.3 源码包含该修复。
  问题追踪器列出的修复版本包括 6.8.4 商业 LTS 和公开的 6.9.1。
- 保持测试源码不变，仅换 Qt 6.9.3，独立复现 **4 passed / 0 failed，866 ms**
  （含初始化和清理），单列/双列各 10 轮模型清空及原生对象释放通过。

修复方式是 macOS 参考构建升级为 Qt 6.9.3，并在 CMake 拒绝 6.9.1 之前的 macOS
构建；Windows、Linux 的 Qt 版本保持原有配置。未给应用增加拦截崩溃的异常处理，
未禁用无障碍功能，未修改 Qt 源码或上一轮局域网直连修复。CI 增加原生 Cocoa 测试，
不会以 offscreen 代替该回归。

### 工具链准备中的限制

默认 aqt 镜像下载速度低，停止后换镜像。安装器请求新镜像时返回错误页面，SHA-256
校验失败，未禁用校验。随后使用 curl 下载八个 Qt 归档，逐个对照 Qt 官方 `.sha256`
核验，再解包并用 aqt 的 Updater 做路径重定位。完整清单见忽略目录中的
`artifacts/accessibility-crash/qt-archives.json`。

首次手动解包使用了错误的目录层级，Updater 因缺少预期的 `mkspecs/qconfig.pri` 失败。
自动审批拒绝清理共享 Qt 根目录，担心影响已有依赖；已放弃该清理，改用独立的
`artifacts/accessibility-crash/Qt/6.9.3/macos`，后续构建不使用误解包目录。
旧 Qt 工具链仍保留。日志位于 `artifacts/accessibility-crash/`，均不纳入提交。

### 完整构建、打包及安装核验

环境：macOS 26.7 arm64、Apple clang 21、Qt 6.9.3、Release，基于 `679f76c`；
生产测试代理、LMM 保持关闭。使用独立构建目录 `build-macos-arm64-qt693`，Qt 前缀指向
上述隔离目录；构建、测试及签名在沙箱外执行。主要验证命令：

```bash
artifacts/macos-build/venv/bin/cmake --build build-macos-arm64-qt693 --parallel 2
artifacts/macos-build/venv/bin/ctest --test-dir build-macos-arm64-qt693 --output-on-failure --no-tests=error --timeout 90
QT_QPA_PLATFORM=cocoa build-macos-arm64-qt693/tests/tst_chatimage
QT_QPA_PLATFORM=cocoa build-macos-arm64-qt693/tests/tst_attachmentinput
git diff --check
```

- Release 构建成功；日志中的 QColor、OpenSSL 弃用警告来自原有代码，未为本次问题
  修改相邻 API。
- 完整 CTest **10/10 通过，31.87 秒**，其中 `macosaccessibility` 强制 Cocoa 并通过；
  原来的代理断开、START2/DONE2、图片及其他回归保持通过。
- 原生 Cocoa 图片菜单测试 **14 passed / 0 failed，2073 ms**；附件选择/拖拽测试
  **35 passed / 0 failed，1134 ms**（均含初始化与清理）。
- 用旧 Qt 6.8.3 重新配置时，CMake 按预期明确报错并拒绝构建，避免未来再打入旧版本。
- `stage-mac-app` 部署后，包内 QtCore、QtGui、QtWidgets、QtNetwork 的版本均为
  **6.9.3**。严格签名通过；包内 Cocoa arm64 插件 UUID 为
  `8F15D38E-B6EE-3B49-BB29-DFC8E6B55B1E`。
- 再以 `DYLD_FRAMEWORK_PATH` 和 `QT_QPA_PLATFORM_PLUGIN_PATH` 指向实际应用包，运行
  同一原生回归，**4 passed / 0 failed，693 ms**。dyld/plugin 日志明确确认 QtGui 和
  `libqcocoa.dylib` 从包内加载，未仅验证开发机上的 Qt。

使用旧应用自身 `/term` 正常退出，备份到
`artifacts/accessibility-crash/previous-LanChat Lite.app` 后替换本机安装版。
新包与安装版的严格签名、可执行文件哈希及 Qt 版本核验通过，详情见
`artifacts/accessibility-crash/installed.json`。新可执行文件 SHA-256：
`207acd7f5d4ca0e80435b2b91a2a694c42df097e7558ce6825660a489d0a372c`。
启动后通过原生界面树确认主窗口、在线状态、联系人区域和操作按钮正常显示；没有
发送测试消息或读写聊天记录目录。

Windows/Intel Mac 本轮构建及远端 CI 尚未执行；实际跨机收发、真实网络切换、睡眠恢复
和长时间常驻仍待测，不将本轮通过写成 24 小时稳定性保证。

## 2026-10-31：启动后 Dock 图标变模糊

用户提供启动前后的 Dock 截图：未运行时清晰，启动后图标像素化。当前应用包 ICNS
具备最高 1024 px 的图像，ICO 也确实包含 16、24、32、48、64、128、256 px，
不是原始素材缺少高清尺寸。

新增 `macosicon` 原生 Cocoa 回归，调用与应用启动相同的
`QApplication::setWindowIcon(QIcon(IDR_APPICON))`，直接检查
`NSApp.applicationIconImage` 的实际表示尺寸。环境为 macOS 26.7 arm64 / Qt 6.9.3：

```bash
QT_QPA_PLATFORM=cocoa build-macos-arm64-qt693/tests/tst_macosicon
```

- 修改前 Qt 能列出 ICO 的完整七档尺寸，但原生 Dock 图像最大仅 **16 px**，用例失败。
- 尝试保留资源 `.ico` 后缀，原生结果仍为 16 px，已撤回这一无效实验。
- 仅将 macOS 的 `IDR_APPICON` 指向已有的原始 256×256 PNG，原生 Dock 图像恢复为
  **256 px**，用例 **3 passed / 0 failed，16 ms**（含初始化和清理）。

未修改设计、放大小图、生成新素材、改动 Dock 设置或清理系统图标缓存；包内 ICNS
和其他平台的 ICO 保持原样。测试编入 macOS 的 Cocoa 套件，避免只检查资源文件尺寸
而漏掉运行时转换后的图像。此前尚未提交的 Qt 6.9.3 与无障碍回归改动保留。

Release 完整构建及全部 CTest **11/11 通过，16.93 秒**，包括网络、原生无障碍及
新增 Dock 图标回归。命令沿用 `build-macos-arm64-qt693`，构建与测试在沙箱外执行。
`stage-mac-app` 部署 Qt 6.9.3 并通过严格签名检查；打包后的 `lmc.rcc` 与构建产物
SHA-256 一致，确认包含新 PNG 资源映射。

正常退出旧应用，备份到 `artifacts/dock-icon/previous-LanChat Lite.app` 后更新本机。
安装版签名及 Qt 版本核验通过，可执行文件 SHA-256：
`0c8ac883f459c7c381858175bbf6e03690637d6850741bc40387997a20076f39`。
原生界面树和窗口截图确认新应用正常启动；未访问或修改配置、聊天记录目录。
Dock 专用自动截图接口超时，因此未宣称已完成实际 Dock 截图对照；当前视觉问题的
自动证据是原生图像表示由 16 px 恢复为 256 px。验证与安装记录位于忽略目录
`artifacts/dock-icon/`。Windows/Intel Mac 和远端 CI 本轮未执行。

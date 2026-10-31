# LanChat Lite 界面与图标

Windows/macOS 共用 Qt Widgets 界面、`lite.qss` 和双气泡图标。背景和文字使用系统调色板，青绿色用于操作图标与发送按钮。保留系统窗口标题栏、默认字体、菜单和原有设置。联系人详略视图分别使用 64/36 逻辑像素行高；聊天最小 440×360，主窗口最小 320×400。

| 用途 | 资源或名称 |
|---|---|
| 产品显示名称 | LanChat Lite |
| Windows 文件与版本信息 | `LanChatLite.exe` / `lmcwin32.rc` |
| macOS 应用包与包内可执行文件 | `LanChat Lite.app` / `LanChat Lite` |
| 配置身份（保持兼容） | `wenlong66` / `LanChatLite` |
| 主图标、关于窗口与状态图标 | `lmc/src/resources/lite-icons/` |
| Qt 资源映射 | `lmc/src/resource.qrc` |

SVG 为本项目原创，PNG/ICO/ICNS 是生成资源，按 GPL-3.0-or-later 许可；旧上游素材与署名保留。普通操作图标使用 64 px PNG，无需增加生产运行时 SVG 依赖。ICO 包含 16、24、32、48、64、128、256 px；ICNS 包含 1×/2×资源，最大 1024 px。

macOS 运行时的 `IDR_APPICON` 使用原始 256×256 `application.png`，应用包继续使用
`application.icns`。这是为了避免多帧 ICO 经 Cocoa 转换后只给 Dock 留下 16×16 图像，
导致启动后变模糊。Windows/其他平台继续使用多尺寸 ICO。`macosicon` 原生回归直接
检查设置应用图标后的 `NSApplication.applicationIconImage`，要求至少保留 256 px。

## 重新生成

开发工具 `scripts/render-icons.cpp` 依赖 Qt 6 Core/Gui/Svg，在仓库根目录运行。Mac 使用 Qt SDK 的路径设置 `LCHAT_QT`，先创建 `artifacts/branding`，再编译：

```bash
c++ -std=c++17 scripts/render-icons.cpp -o artifacts/branding/render-icons \
  -F "$LCHAT_QT/lib" \
  -I "$LCHAT_QT/lib/QtCore.framework/Headers" \
  -I "$LCHAT_QT/lib/QtGui.framework/Headers" \
  -I "$LCHAT_QT/lib/QtSvg.framework/Headers" \
  -framework QtCore -framework QtGui -framework QtSvg \
  -Wl,-rpath,"$LCHAT_QT/lib"
artifacts/branding/render-icons
iconutil -c icns artifacts/branding/LanChatLite.iconset \
  -o lmc/src/resources/lite-icons/application.icns
```

本轮 Apple clang 21 / Qt 6.8.3 arm64 组合需要在编译参数中增加 `-include arm_acle.h`，否则 Qt 的 `__yield` 声明报错。生成工具不进入默认生产构建。

修改资源后重新构建并执行 `package-local`，Mac 再运行 `stage-mac-app`；Windows 重新构建资源文件并执行 `windeployqt`。旧测试包与已经运行的进程不会随源码自动更新。

参考：[Qt 应用图标](https://doc.qt.io/qt-6/appicon.html)、[Qt 高 DPI](https://doc.qt.io/qt-6/highdpi.html)、[Qt 样式表](https://doc.qt.io/qt-6/stylesheet-reference.html)。

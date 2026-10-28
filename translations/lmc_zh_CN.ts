<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN" sourcelanguage="en_US">
<context>
    <name>AboutDialog</name>
    <message>
        <location filename="../lmc/src/gui/main/aboutdialog.ui" line="17"/>
        <source>Dialog</source>
        <translation>对话框</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/aboutdialog.ui" line="40"/>
        <source>&lt;Logo&gt;</source>
        <translation>&lt;标志&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/aboutdialog.ui" line="47"/>
        <source>&lt;Application Title&gt;</source>
        <translation>&lt;应用程序名称&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/aboutdialog.ui" line="54"/>
        <source>&lt;_Qt Version&gt;</source>
        <translation>&lt;_Qt 版本&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/aboutdialog.ui" line="70"/>
        <source>About</source>
        <translation>关于</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/aboutdialog.ui" line="91"/>
        <source>&lt;Description&gt;</source>
        <translation>&lt;简介&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/aboutdialog.ui" line="117"/>
        <source>Thanks</source>
        <translation>致谢</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/aboutdialog.ui" line="155"/>
        <source>License</source>
        <translation>许可证</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/aboutdialog.ui" line="232"/>
        <source>Close</source>
        <translation>关闭</translation>
    </message>
</context>
<context>
    <name>AppRunner</name>
    <message>
        <location filename="../lmc/src/app/apprunner.cpp" line="36"/>
        <source>Usage:	lmc [switches]
	All switches are optional.

/loopback - Allows loopback communication with local machine.
/nohistory - Deletes existing history.
/nofilehistory - Deletes existing file transfer history.
/noconfig - Resets preferences to default values.
/sync - Synchronizes application settings and their external dependencies.
/unsync - Removes all external application dependencies.
/term - Closes an instance which is already running.
/inst - Returns a value indicating whether an instance is already running.
/quit - Closes once the command line parameters are processed.
/trace - Enables debug logging to logs/lmc_*.log (disabled by default).
/? - Display this help.

Example:
Start in loopback mode with default settings: lmc /loopback /noconfig

Some command line switches are mutually exclusive. If multiple switches
are specified, they will take precedence in the order given above.
Copyright (C) 2010-2012 Qualia Digital Solutions.
</source>
        <translation>用法：	lmc [选项]
	所有选项均为可选。

/loopback - 允许与本机进行回环通信。
/nohistory - 删除现有历史记录。
/nofilehistory - 删除现有文件传输历史记录。
/noconfig - 将首选项重置为默认值。
/sync - 同步应用程序设置及其外部依赖项。
/unsync - 移除所有应用程序外部依赖项。
/term - 关闭正在运行的实例。
/inst - 返回一个值，指示是否已有实例正在运行。
/quit - 处理完命令行参数后退出。
/trace - 启用调试日志，保存到 logs/lmc_*.log（默认禁用）。
/? - 显示此帮助。

示例：
以默认设置启动回环模式：lmc /loopback /noconfig

部分命令行选项互斥。如果指定了多个选项，
将按上述顺序确定优先级。
Copyright (C) 2010-2012 Qualia Digital Solutions.
</translation>
    </message>
    <message>
        <location filename="../lmc/src/app/apprunner.cpp" line="112"/>
        <source>Qt is compiled with QT_NO_SSL. Some functions will not work correctly. Quit application?</source>
        <translation>Qt 编译时启用了 QT_NO_SSL，部分功能将无法正常工作。是否退出应用程序？</translation>
    </message>
    <message>
        <location filename="../lmc/src/app/apprunner.cpp" line="116"/>
        <source>Messenger does not find ssl at startup. Probably missing openssl dll. Some functions will not work correctly. Quit application?</source>
        <translation>应用程序启动时未找到 SSL，可能缺少 OpenSSL DLL。部分功能将无法正常工作。是否退出应用程序？</translation>
    </message>
</context>
<context>
    <name>BroadcastWindow</name>
    <message>
        <location filename="../lmc/src/gui/chat/broadcastwindow.ui" line="14"/>
        <source>Form</source>
        <translation>窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/broadcastwindow.ui" line="143"/>
        <source>Send</source>
        <translation>发送</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/broadcastwindow.ui" line="153"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/broadcastwindow.ui" line="239"/>
        <source>Select All</source>
        <translation>全选</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/broadcastwindow.ui" line="246"/>
        <source>Select None</source>
        <translation>全不选</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/broadcastwindow.ui" line="178"/>
        <location filename="../lmc/src/gui/chat/broadcastwindow.ui" line="213"/>
        <source>Send To:</source>
        <translation>发送至：</translation>
    </message>
</context>
<context>
    <name>ChatRoomWindow</name>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.ui" line="14"/>
        <source>Form</source>
        <translation>窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.ui" line="217"/>
        <source>Send</source>
        <translation>发送</translation>
    </message>
</context>
<context>
    <name>ChatWindow</name>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.ui" line="14"/>
        <source>Form</source>
        <translation>窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.ui" line="207"/>
        <source>Send</source>
        <translation>发送</translation>
    </message>
</context>
<context>
    <name>ClipboardImage</name>
    <message>
        <location filename="../lmc/src/gui/widgets/clipboardimage.h" line="29"/>
        <source>The image exceeds the clipboard attachment limit.</source>
        <translation>图片超出了剪贴板附件的限制。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/widgets/clipboardimage.h" line="34"/>
        <source>The temporary image folder is unavailable.</source>
        <translation>临时图片文件夹不可用。</translation>
    </message>
</context>
<context>
    <name>FileView</name>
    <message>
        <location filename="../lmc/src/gui/files/filemodelview.cpp" line="39"/>
        <source>0 bytes</source>
        <translation>0 字节</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/filemodelview.cpp" line="40"/>
        <source>0 bytes/sec</source>
        <translation>0 字节/秒</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/filemodelview.cpp" line="41"/>
        <source>Calculating time</source>
        <translation>正在计算剩余时间</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/filemodelview.cpp" line="79"/>
        <source>To: </source>
        <translation>发送至： </translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/filemodelview.cpp" line="79"/>
        <source>From: </source>
        <translation>来自： </translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/filemodelview.cpp" line="95"/>
        <location filename="../lmc/src/gui/files/filemodelview.cpp" line="99"/>
        <source>%1 left - %2 of %3 (%4)</source>
        <translation>剩余 %1 - %2 / %3（%4）</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/filemodelview.cpp" line="103"/>
        <source>Completed</source>
        <translation>已完成</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/filemodelview.cpp" line="106"/>
        <source>Canceled</source>
        <translation>已取消</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/filemodelview.cpp" line="109"/>
        <source>Interrupted</source>
        <translation>已中断</translation>
    </message>
</context>
<context>
    <name>HelpWindow</name>
    <message>
        <location filename="../lmc/src/gui/main/helpwindow.ui" line="14"/>
        <source>Form</source>
        <translation>窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/helpwindow.ui" line="55"/>
        <source>Close</source>
        <translation>关闭</translation>
    </message>
</context>
<context>
    <name>Helper</name>
    <message>
        <location filename="../lmc/src/core/shared.cpp" line="65"/>
        <source>%1 bytes</source>
        <translation>%1 字节</translation>
    </message>
</context>
<context>
    <name>HistoryWindow</name>
    <message>
        <location filename="../lmc/src/gui/history/historywindow.ui" line="14"/>
        <source>Form</source>
        <translation>窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/history/historywindow.ui" line="76"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/history/historywindow.ui" line="81"/>
        <source>Date</source>
        <translation>日期</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/history/historywindow.ui" line="133"/>
        <source>Clear History</source>
        <translation>清空历史记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/history/historywindow.ui" line="156"/>
        <source>Close</source>
        <translation>关闭</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.ui" line="14"/>
        <source>Form</source>
        <translation>窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.ui" line="124"/>
        <source>Username</source>
        <translation>用户名</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.ui" line="168"/>
        <source>Status</source>
        <translation>状态</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.ui" line="186"/>
        <source>Type a note</source>
        <translation>输入个性签名</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.ui" line="211"/>
        <source>Change avatar</source>
        <translation>更换头像</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.ui" line="324"/>
        <source>Contacts</source>
        <translation>联系人</translation>
    </message>
</context>
<context>
    <name>SettingsDialog</name>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="17"/>
        <source>Dialog</source>
        <translation>对话框</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1174"/>
        <source>Reset All</source>
        <translation>全部重置</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1355"/>
        <source>Broadcast address:</source>
        <oldsource>Broadcast Address:</oldsource>
        <translation>广播地址：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1392"/>
        <source>&lt;- Add</source>
        <translation>&lt;- 添加</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1419"/>
        <source>Delete</source>
        <translation>删除</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1448"/>
        <source>Advanced</source>
        <translation>高级</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1456"/>
        <source>Multicast address:</source>
        <translation>多播地址：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1832"/>
        <source>Chat Theme</source>
        <translation>聊天主题</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1863"/>
        <source>Refresh</source>
        <translation>刷新</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1908"/>
        <source>Contacts List</source>
        <translation>联系人列表</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1916"/>
        <source>Contacts list view:</source>
        <translation>联系人列表视图：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1941"/>
        <source>Show status as tooltip</source>
        <translation>以提示文字显示状态</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="2016"/>
        <source>Send messages using:</source>
        <translation>发送消息快捷键：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="2041"/>
        <source>&lt;Enter&gt;</source>
        <oldsource>Enter</oldsource>
        <translation>&lt;Enter&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="2048"/>
        <source>&lt;Cmd + Enter&gt;</source>
        <oldsource>Ctrl + Enter</oldsource>
        <translation>&lt;Cmd + Enter&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="2109"/>
        <source>Reset Preferences</source>
        <translation>重置首选项</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="2129"/>
        <source>OK</source>
        <translation>确定</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="2139"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="59"/>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="150"/>
        <source>General</source>
        <translation>常规</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="94"/>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1825"/>
        <source>Appearance</source>
        <translation>外观</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="157"/>
        <source>System</source>
        <translation>系统</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="163"/>
        <source>Start %1 on system startup</source>
        <translation>系统启动时运行 %1</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="170"/>
        <source>Show main window when %1 starts</source>
        <translation>%1 启动时显示主窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="180"/>
        <source>System Tray</source>
        <translation>系统托盘</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="186"/>
        <source>Show system tray icon</source>
        <translation>显示系统托盘图标</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="193"/>
        <source>Minimize main window to system tray</source>
        <translation>将主窗口最小化到系统托盘</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="200"/>
        <source>Single click on tray icon to open</source>
        <translation>单击托盘图标打开窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="207"/>
        <source>Show balloon notifications in tray</source>
        <translation>在托盘区显示气泡通知</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="224"/>
        <source>Language</source>
        <translation>语言</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="232"/>
        <source>Select language:</source>
        <translation>选择语言：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="64"/>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="314"/>
        <source>Account</source>
        <translation>账户</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="321"/>
        <source>Personal</source>
        <translation>个人资料</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="329"/>
        <source>User name as seen by contacts:</source>
        <translation>向联系人显示的用户名：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="358"/>
        <source>First Name:</source>
        <translation>名：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="388"/>
        <source>Last Name:</source>
        <translation>姓：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="404"/>
        <source>About:</source>
        <translation>个人简介：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="432"/>
        <source>Contacts</source>
        <translation>联系人</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="440"/>
        <source>Refresh contacts list every</source>
        <translation>联系人列表刷新间隔</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="470"/>
        <source>seconds</source>
        <translation>秒</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="69"/>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="529"/>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="2005"/>
        <source>Messages</source>
        <translation>消息</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="536"/>
        <source>Message Window</source>
        <translation>消息窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="599"/>
        <source>Show emoticons in instant messages</source>
        <translation>在即时消息中显示表情</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="606"/>
        <source>Add time stamp to instant messages</source>
        <translation>在即时消息中显示时间戳</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="631"/>
        <source>Show date in time stamp</source>
        <translation>在时间戳中显示日期</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="542"/>
        <source>Set incoming messages foreground</source>
        <translation>收到消息时将窗口置于前台</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="549"/>
        <source>Minimize incoming messages to taskbar</source>
        <translation>收到消息时将窗口最小化到任务栏</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="698"/>
        <source>Message Font</source>
        <translation>消息字体</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="74"/>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="836"/>
        <source>History</source>
        <translation>历史记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="99"/>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1998"/>
        <source>Hotkeys</source>
        <translation>快捷键</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="214"/>
        <source>Minimize main window using tray icon</source>
        <translation>通过托盘图标最小化主窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="574"/>
        <source>Apply to public messages</source>
        <oldsource>Apply  to public messages</oldsource>
        <translation>同样应用于公共消息</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="640"/>
        <source>Allow hyperlinks in instant messages</source>
        <translation>允许即时消息中的超链接</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="665"/>
        <source>Convert network paths to links</source>
        <translation>将网络路径转换为链接</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="674"/>
        <source>Remove whitespace from start and end of messages</source>
        <translation>去除消息首尾的空白字符</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="681"/>
        <source>Clear messages when window is closed</source>
        <translation>关闭窗口时清空消息</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="688"/>
        <source>Show Send button in chat windows</source>
        <translation>在聊天窗口中显示发送按钮</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="706"/>
        <source>Change Font...</source>
        <translation>更改字体...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="730"/>
        <source>Change Color...</source>
        <translation>更改颜色...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="754"/>
        <source>Incoming message text size:</source>
        <translation>接收消息的文字大小：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="843"/>
        <source>Message History</source>
        <translation>消息历史记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="849"/>
        <source>Keep a history of conversations</source>
        <translation>保存聊天历史记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="858"/>
        <source>Clear Message History</source>
        <translation>清空消息历史记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="883"/>
        <source>History File Location</source>
        <translation>历史记录文件位置</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="889"/>
        <source>System default location</source>
        <translation>系统默认位置</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="896"/>
        <source>Custom location</source>
        <translation>自定义位置</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="934"/>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1154"/>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1742"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="946"/>
        <source>File Transfer History</source>
        <translation>文件传输历史记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="952"/>
        <source>Keep a history of file transfers</source>
        <translation>保存文件传输历史记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="961"/>
        <source>Clear Transfer History</source>
        <translation>清空传输历史记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="79"/>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1033"/>
        <source>Alerts</source>
        <translation>提醒</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1040"/>
        <source>Status Alerts</source>
        <translation>状态提醒</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1046"/>
        <source>Display status alerts</source>
        <translation>显示状态提醒</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1053"/>
        <source>Suspend alerts when my status is Busy</source>
        <translation>我的状态为“忙碌”时暂停提醒</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1060"/>
        <source>Suspend alerts when my status is Do Not Disturb</source>
        <translation>我的状态为“请勿打扰”时暂停提醒</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1070"/>
        <source>Sounds</source>
        <translation>声音</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1076"/>
        <source>Provide feedback with sounds</source>
        <translation>启用提示音</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1187"/>
        <source>Suspend sounds when my status is Busy</source>
        <translation>我的状态为“忙碌”时暂停提示音</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1194"/>
        <source>Suspend sound when my status is Do Not Disturb</source>
        <translation>我的状态为“请勿打扰”时暂停提示音</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1103"/>
        <source>Play sounds for these events:</source>
        <translation>为以下事件播放提示音：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="84"/>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1251"/>
        <source>Network</source>
        <translation>网络</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1258"/>
        <source>Connection</source>
        <translation>连接</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1266"/>
        <source>Connection timeout (seconds):</source>
        <translation>连接超时（秒）：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1300"/>
        <source>Maximum number of retries:</source>
        <translation>最大重试次数：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1335"/>
        <source>Broadcast</source>
        <oldsource>Broadcast Settings</oldsource>
        <translation>广播</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1341"/>
        <source>Broadcast list:</source>
        <translation>广播地址列表：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1499"/>
        <source>UDP Port*:</source>
        <translation>UDP 端口*：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1542"/>
        <source>TCP Port*:</source>
        <translation>TCP 端口*：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1583"/>
        <source>* Takes effect after you restart %1.</source>
        <translation>* 重启 %1 后生效。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="89"/>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1637"/>
        <source>File Transfer</source>
        <translation>文件传输</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1644"/>
        <source>Incoming File Request</source>
        <translation>文件接收请求</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1682"/>
        <source>Bring window to foreground</source>
        <translation>将窗口置于前台</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1709"/>
        <source>Minimize window to taskbar</source>
        <translation>将窗口最小化到任务栏</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1650"/>
        <source>Accept and start receiving files automatically</source>
        <translation>自动接受并开始接收文件</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1657"/>
        <source>Show File Transfer window when sending or receiving files</source>
        <translation>发送或接收文件时显示文件传输窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1721"/>
        <source>Store Received Files in this Folder</source>
        <translation>将接收的文件保存到此文件夹</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.ui" line="1753"/>
        <source>View Files</source>
        <translation>查看文件</translation>
    </message>
</context>
<context>
    <name>TransferWindow</name>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.ui" line="14"/>
        <source>Form</source>
        <translation>窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.ui" line="88"/>
        <source>Clear List</source>
        <translation>清空列表</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.ui" line="108"/>
        <source>Close</source>
        <translation>关闭</translation>
    </message>
</context>
<context>
    <name>UpdateWindow</name>
    <message>
        <location filename="../lmc/src/gui/main/updatewindow.ui" line="14"/>
        <source>Form</source>
        <translation>窗口</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/updatewindow.ui" line="82"/>
        <source>Close</source>
        <translation>关闭</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/updatewindow.ui" line="92"/>
        <source>Recheck Version</source>
        <translation>重新检查版本</translation>
    </message>
</context>
<context>
    <name>UserInfoWindow</name>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="14"/>
        <source>Dialog</source>
        <translation>对话框</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="34"/>
        <source>&lt;Avatar&gt;</source>
        <translation>&lt;头像&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="43"/>
        <source>&lt;User Name&gt;</source>
        <translation>&lt;用户名&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="50"/>
        <source>&lt;Status&gt;</source>
        <translation>&lt;状态&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="84"/>
        <source>Personal Information</source>
        <translation>个人信息</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="94"/>
        <source>First Name:</source>
        <translation>名：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="101"/>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="122"/>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="160"/>
        <source>N/A</source>
        <translation>无</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="115"/>
        <source>Last Name:</source>
        <translation>姓：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="138"/>
        <source>About:</source>
        <translation>个人简介：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="170"/>
        <source>System Information</source>
        <translation>系统信息</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="176"/>
        <source>IP Address:</source>
        <translation>IP 地址：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="186"/>
        <source>&lt;IP Address&gt;</source>
        <translation>&lt;IP 地址&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="193"/>
        <source>Logon Name:</source>
        <translation>登录名：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="203"/>
        <source>&lt;Logon Name&gt;</source>
        <translation>&lt;登录名&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="210"/>
        <source>Computer Name:</source>
        <translation>计算机名：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="220"/>
        <source>&lt;Computer Name&gt;</source>
        <translation>&lt;计算机名&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="227"/>
        <source>Operating System:</source>
        <translation>操作系统：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="237"/>
        <source>&lt;Operating System&gt;</source>
        <translation>&lt;操作系统&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="244"/>
        <source>Messenger Version:</source>
        <translation>软件版本：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="254"/>
        <source>&lt;Messenger Version&gt;</source>
        <translation>&lt;软件版本&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.ui" line="280"/>
        <source>Close</source>
        <translation>关闭</translation>
    </message>
</context>
<context>
    <name>UserSelectDialog</name>
    <message>
        <location filename="../lmc/src/gui/contacts/userselectdialog.ui" line="17"/>
        <source>Dialog</source>
        <translation>对话框</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userselectdialog.ui" line="36"/>
        <source>Contacts</source>
        <translation>联系人</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userselectdialog.ui" line="59"/>
        <source>OK</source>
        <translation>确定</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/contacts/userselectdialog.ui" line="69"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
</context>
<context>
    <name>lmcAboutDialog</name>
    <message>
        <location filename="../lmc/src/gui/main/aboutdialog.cpp" line="70"/>
        <source>About %1</source>
        <translation>关于 %1</translation>
    </message>
</context>
<context>
    <name>lmcBroadcastWindow</name>
    <message>
        <location filename="../lmc/src/gui/chat/broadcastwindow.cpp" line="300"/>
        <source>Send Broadcast Message</source>
        <translation>发送广播消息</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/broadcastwindow.cpp" line="303"/>
        <source>Change Font Size</source>
        <translation>更改字号</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/broadcastwindow.cpp" line="304"/>
        <source>Insert Smiley</source>
        <translation>插入表情</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/broadcastwindow.cpp" line="340"/>
        <source>No recipient selected</source>
        <translation>未选择接收人</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/broadcastwindow.cpp" line="341"/>
        <source>Please select at least one recipient to send a broadcast.</source>
        <translation>请至少选择一位接收人以发送广播消息。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/broadcastwindow.cpp" line="363"/>
        <source>You are no longer connected. Broadcast message cannot be sent.</source>
        <translation>连接已断开，无法发送广播消息。</translation>
    </message>
</context>
<context>
    <name>lmcChatRoomWindow</name>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="169"/>
        <source>Group Conversation</source>
        <translation>群组聊天</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="303"/>
        <source>%1 says...</source>
        <translation>%1 说...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="470"/>
        <source>Select Font</source>
        <translation>选择字体</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="476"/>
        <source>Select Color</source>
        <translation>选择颜色</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="485"/>
        <source>Save Conversation</source>
        <translation>保存聊天记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="632"/>
        <source>Participants</source>
        <translation>参与者</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="634"/>
        <source>&amp;Conversation</source>
        <translation>聊天(&amp;C)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="635"/>
        <source>Send &amp;File</source>
        <translation>发送文件(&amp;F)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="636"/>
        <source>Get &amp;Information</source>
        <translation>查看资料(&amp;I)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="637"/>
        <source>Insert Smiley</source>
        <translation>插入表情</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="638"/>
        <source>&amp;Save As...</source>
        <translation>另存为(&amp;S)...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="639"/>
        <source>Save this conversation</source>
        <translation>保存当前聊天记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="640"/>
        <source>Send</source>
        <translation>发送</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="641"/>
        <source>Send message</source>
        <translation>发送消息</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="642"/>
        <source>Change Font...</source>
        <translation>更改字体...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="643"/>
        <source>Change message font</source>
        <translation>更改消息字体</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="644"/>
        <source>Change Color...</source>
        <translation>更改颜色...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="645"/>
        <source>Change message text color</source>
        <translation>更改消息文字颜色</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="648"/>
        <source>&amp;Add Contacts...</source>
        <oldsource>&amp;Add A Contact...</oldsource>
        <translation>添加联系人(&amp;A)...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="649"/>
        <source>Add people to this conversation</source>
        <oldsource>Add contacts to this conversation</oldsource>
        <translation>将联系人加入当前聊天</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="719"/>
        <source>You are no longer connected.</source>
        <translation>连接已断开。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="744"/>
        <source>Conversation</source>
        <translation>聊天</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatroomwindow.cpp" line="746"/>
        <source>Public Chat</source>
        <translation>公共聊天</translation>
    </message>
</context>
<context>
    <name>lmcChatWindow</name>
    <message>
        <source>%1 sends an image...</source>
        <translation>%1 发来图片...</translation>
    </message>
    <message>
        <source>File</source>
        <translation>文件</translation>
    </message>
    <message>
        <source>Folder</source>
        <translation>文件夹</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="661"/>
        <source>Insert Smiley</source>
        <translation>插入表情</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="662"/>
        <source>Send A &amp;File...</source>
        <translation>发送文件(&amp;F)...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="663"/>
        <source>Send A Fol&amp;der...</source>
        <translation>发送文件夹(&amp;D)...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="664"/>
        <source>&amp;Save As...</source>
        <translation>另存为(&amp;S)...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="665"/>
        <source>&amp;History</source>
        <translation>历史记录(&amp;H)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="666"/>
        <source>File &amp;Transfers</source>
        <translation>文件传输(&amp;T)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="668"/>
        <source>Send a file to %1</source>
        <translation>向 %1 发送文件</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="670"/>
        <source>Send a folder to %1</source>
        <translation>向 %1 发送文件夹</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="673"/>
        <source>Save this conversation</source>
        <translation>保存当前聊天记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="674"/>
        <source>View History</source>
        <translation>查看历史记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="675"/>
        <source>View File Transfers</source>
        <translation>查看文件传输</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="679"/>
        <source>Change Font...</source>
        <translation>更改字体...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="680"/>
        <source>Change message font</source>
        <translation>更改消息字体</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="681"/>
        <source>Change Color...</source>
        <translation>更改颜色...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="682"/>
        <source>Change message text color</source>
        <translation>更改消息文字颜色</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="412"/>
        <source>Select Font</source>
        <translation>选择字体</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="418"/>
        <source>Select Color</source>
        <translation>选择颜色</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="452"/>
        <source>Save Conversation</source>
        <translation>保存聊天记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="179"/>
        <source>Group Conversation</source>
        <translation>群组聊天</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="201"/>
        <source>%1 says...</source>
        <translation>%1 说...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="209"/>
        <source>Broadcast from %1</source>
        <translation>来自 %1 的广播</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="255"/>
        <source>%1 sends a file...</source>
        <translation>%1 发来文件...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="257"/>
        <source>%1 sends a folder...</source>
        <translation>%1 发来文件夹...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="479"/>
        <source>Image too large</source>
        <translation>图片过大</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="480"/>
        <source>Clipboard images must be at most 16 megapixels and 64 MiB when decoded. Save larger images to a file and use Send File instead.</source>
        <translation>剪贴板图片不得超过 1600 万像素，且解码后不得超过 64 MiB。请将超出限制的图片保存为文件，再使用“发送文件”。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="491"/>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="512"/>
        <source>Cannot send image</source>
        <translation>无法发送图片</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="492"/>
        <source>Open a one-to-one conversation with an online device that supports file transfers.</source>
        <translation>请与支持文件传输的在线设备打开一对一聊天。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="496"/>
        <source>Send image?</source>
        <translation>发送图片？</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="497"/>
        <source>Send this %1 x %2 image to %3?</source>
        <translation>要将这张 %1 x %2 的图片发送给 %3 吗？</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="501"/>
        <source>The image will appear directly in the conversation on updated devices.</source>
        <translation>使用新版的设备会直接在聊天中显示这张图片。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="505"/>
        <source>Send image</source>
        <translation>发送图片</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="512"/>
        <source>The other device is now offline.</source>
        <translation>对方设备现已离线。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="518"/>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="530"/>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="539"/>
        <source>Cannot prepare image</source>
        <translation>无法准备图片</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="519"/>
        <source>The temporary image folder is unavailable.</source>
        <translation>临时图片文件夹不可用。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="531"/>
        <source>The temporary image folder is unavailable or full. Finish current image transfers, or save the image and use Send File.</source>
        <translation>临时图片文件夹不可用或已满。请等待当前图片传输完成，或将图片保存为文件后使用“发送文件”。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="676"/>
        <source>Send</source>
        <translation>发送</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="677"/>
        <source>Send message</source>
        <translation>发送消息</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="678"/>
        <source>Type a message, paste an image, or drop files/folders...</source>
        <translation>输入消息、粘贴图片，或拖入文件/文件夹...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="800"/>
        <source>You are no longer connected.</source>
        <translation>连接已断开。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="803"/>
        <source>%1 is offline.</source>
        <translation>%1 已离线。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="807"/>
        <source>%1 is away.</source>
        <translation>%1 暂时离开。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="811"/>
        <source>%1 is busy. You may be interrupting.</source>
        <translation>%1 正忙，发送消息可能会打扰对方。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/chatwindow.cpp" line="836"/>
        <source>Conversation</source>
        <translation>聊天</translation>
    </message>
</context>
<context>
    <name>lmcCore</name>
    <message>
        <location filename="../lmc/src/app/lmc.cpp" line="146"/>
        <source>Preferences could not be imported from &apos;%1&apos;.

File may not exist, or may not be compatible with this version of %2.</source>
        <translation>无法从“%1”导入首选项。

文件可能不存在，或与当前版本的 %2 不兼容。</translation>
    </message>
    <message>
        <location filename="../lmc/src/app/lmc.cpp" line="153"/>
        <location filename="../lmc/src/app/lmc.cpp" line="194"/>
        <source>LAYOUT_DIRECTION</source>
        <translation>LTR</translation>
    </message>
    <message>
        <location filename="../lmc/src/app/lmc.cpp" line="963"/>
        <source>A port address conflict has been detected. %1 will close now.</source>
        <translation>检测到端口地址冲突，%1 即将关闭。</translation>
    </message>
    <message>
        <location filename="../lmc/src/app/lmc.cpp" line="965"/>
        <source>%1 cannot start because another application is using the port configured for use with %2.</source>
        <translation>无法启动 %1，因为为 %2 配置的端口正被其他应用程序占用。</translation>
    </message>
    <message>
        <location filename="../lmc/src/app/lmc.cpp" line="140"/>
        <source>Your preferences file is corrupt or invalid.

%1 is unable to recover your settings.</source>
        <translation>首选项文件已损坏或无效。

%1 无法恢复您的设置。</translation>
    </message>
</context>
<context>
    <name>lmcHelpWindow</name>
    <message>
        <location filename="../lmc/src/gui/main/helpwindow.cpp" line="101"/>
        <source>Help</source>
        <translation>帮助</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/helpwindow.cpp" line="107"/>
        <source>&lt;html&gt;&lt;head&gt;&lt;meta http-equiv=&apos;Content-Type&apos; content=&apos;text/html; charset=utf-8&apos; /&gt;&lt;/head&gt;&lt;body&gt;&lt;h2&gt;Welcome to LAN Messenger Help&lt;/h2&gt;&lt;h3&gt;Help&lt;/h3&gt;&lt;p&gt;You can find documentation on LAN Messenger&apos;s &lt;a href=&apos;%1&apos;&gt;wiki&lt;/a&gt; website.&lt;/p&gt;&lt;p&gt;Before asking any question, please refer yourself to the &lt;a href=&apos;%2&apos;&gt;FAQ&lt;/a&gt;.&lt;/p&gt;&lt;p&gt;You might then get (and give) help on the &lt;a href=&apos;%3&apos;&gt;Support Forums&lt;/a&gt;.&lt;/p&gt;&lt;h3&gt;Contribute to the project&lt;/h3&gt;&lt;p&gt;You can help the LAN Messenger project giving some of your time to help the community, to translate the application or documentation, and to test the program. And of course, you can &lt;b&gt;promote&lt;/b&gt; LAN Messenger.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head&gt;&lt;meta http-equiv=&apos;Content-Type&apos; content=&apos;text/html; charset=utf-8&apos; /&gt;&lt;/head&gt;&lt;body&gt;&lt;h2&gt;欢迎使用 LAN Messenger 帮助&lt;/h2&gt;&lt;h3&gt;帮助&lt;/h3&gt;&lt;p&gt;您可以在 LAN Messenger 的 &lt;a href=&apos;%1&apos;&gt;维基&lt;/a&gt;网站查阅文档。&lt;/p&gt;&lt;p&gt;提问前，请先参阅&lt;a href=&apos;%2&apos;&gt;常见问题&lt;/a&gt;。&lt;/p&gt;&lt;p&gt;您也可以前往&lt;a href=&apos;%3&apos;&gt;支持论坛&lt;/a&gt;寻求帮助，或帮助其他用户。&lt;/p&gt;&lt;h3&gt;参与项目&lt;/h3&gt;&lt;p&gt;您可以抽出一些时间帮助社区、翻译应用程序或文档，以及测试程序，为 LAN Messenger 项目贡献力量。当然，您也可以&lt;b&gt;推广&lt;/b&gt; LAN Messenger。&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
</context>
<context>
    <name>lmcHistoryWindow</name>
    <message>
        <location filename="../lmc/src/gui/history/historywindow.cpp" line="133"/>
        <source>Message History</source>
        <translation>消息历史记录</translation>
    </message>
</context>
<context>
    <name>lmcMainWindow</name>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="211"/>
        <source>%1 is online.</source>
        <translation>%1 已上线。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="246"/>
        <source>%1 is offline.</source>
        <translation>%1 已离线。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="267"/>
        <source>You are online.</source>
        <translation>您已上线。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="1131"/>
        <source>%1 - Not Connected</source>
        <translation>%1 - 未连接</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="269"/>
        <source>You are no longer connected.</source>
        <translation>连接已断开。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="443"/>
        <source>Select avatar picture</source>
        <translation>选择头像图片</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="565"/>
        <source>Add New Group</source>
        <translation>新建分组</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="565"/>
        <source>Enter a name for the group</source>
        <translation>请输入分组名称</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="571"/>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="604"/>
        <source>A group named &apos;%1&apos; already exists. Please enter a different name.</source>
        <translation>名为“%1”的分组已存在，请使用其他名称。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="597"/>
        <source>Rename Group</source>
        <translation>重命名分组</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="598"/>
        <source>Enter a new name for the group</source>
        <translation>请输入新的分组名称</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="899"/>
        <source>&amp;Messenger</source>
        <translation>即时通信(&amp;M)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="900"/>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="934"/>
        <source>&amp;New Chat Room</source>
        <translation>新建聊天室(&amp;N)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="902"/>
        <source>&amp;Refresh Contacts List</source>
        <translation>刷新联系人列表(&amp;R)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="901"/>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="935"/>
        <source>&amp;Public Chat</source>
        <translation>公共聊天(&amp;P)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="444"/>
        <source>Images (*.bmp *.gif *.jpg *.jpeg *.png *.tif *.tiff)</source>
        <translation>图片 (*.bmp *.gif *.jpg *.jpeg *.png *.tif *.tiff)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="903"/>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="921"/>
        <source>E&amp;xit</source>
        <translation>退出(&amp;X)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="904"/>
        <source>&amp;Tools</source>
        <translation>工具(&amp;T)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="905"/>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="917"/>
        <source>&amp;History</source>
        <translation>历史记录(&amp;H)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="906"/>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="918"/>
        <source>File &amp;Transfers</source>
        <translation>文件传输(&amp;T)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="907"/>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="919"/>
        <source>&amp;Preferences</source>
        <translation>首选项(&amp;P)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="908"/>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="909"/>
        <source>&amp;Help</source>
        <translation>帮助(&amp;H)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="910"/>
        <source>%1 &amp;online</source>
        <translation>%1 网站(&amp;O)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="912"/>
        <source>Check for &amp;Updates...</source>
        <translation>检查更新(&amp;U)...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="913"/>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="920"/>
        <source>&amp;About</source>
        <translation>关于(&amp;A)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="914"/>
        <source>&amp;Show %1</source>
        <translation>显示 %1(&amp;S)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="916"/>
        <source>&amp;Change Status</source>
        <translation>更改状态(&amp;C)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="922"/>
        <source>Add &amp;New Group</source>
        <translation>新建分组(&amp;N)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="923"/>
        <source>&amp;Rename This Group</source>
        <translation>重命名此分组(&amp;R)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="924"/>
        <source>&amp;Delete This Group</source>
        <translation>删除此分组(&amp;D)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="925"/>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="931"/>
        <source>&amp;Conversation</source>
        <translation>聊天(&amp;C)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="926"/>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="933"/>
        <source>Send &amp;Broadcast Message</source>
        <translation>发送广播消息(&amp;B)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="927"/>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="932"/>
        <source>Send &amp;File</source>
        <translation>发送文件(&amp;F)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="928"/>
        <source>Send Fol&amp;der</source>
        <translation>发送文件夹(&amp;D)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="929"/>
        <source>Get &amp;Information</source>
        <translation>查看资料(&amp;I)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="930"/>
        <source>&amp;Browse for more pictures...</source>
        <translation>浏览更多图片(&amp;B)...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/mainwindow.cpp" line="958"/>
        <source>%1 will continue to run in the background. Activate this icon to restore the application window.</source>
        <translation>%1 将继续在后台运行。点击此图标可恢复应用程序窗口。</translation>
    </message>
</context>
<context>
    <name>lmcMessageLog</name>
    <message>
        <source>Copy Image</source>
        <translation>复制图片</translation>
    </message>
    <message>
        <source>Open File Location</source>
        <translation>打开文件位置</translation>
    </message>
    <message>
        <source>Cannot copy image</source>
        <translation>无法复制图片</translation>
    </message>
    <message>
        <source>The original image is unavailable or exceeds the image size limit.</source>
        <translation>原图不可用，或超过图片大小限制。</translation>
    </message>
    <message>
        <source>Cannot open file location</source>
        <translation>无法打开文件位置</translation>
    </message>
    <message>
        <source>The image file or its folder is unavailable.</source>
        <translation>图片文件或所在文件夹不可用。</translation>
    </message>
    <message>
        <source>Image to %1</source>
        <translation>发送给 %1 的图片</translation>
    </message>
    <message>
        <source>Image from %1</source>
        <translation>%1 发来的图片</translation>
    </message>
    <message>
        <source>Image</source>
        <translation>图片</translation>
    </message>
    <message>
        <source>Receiving image...</source>
        <translation>正在接收图片...</translation>
    </message>
    <message>
        <source>Image unavailable</source>
        <translation>图片不可用</translation>
    </message>
    <message>
        <source>Click to open image</source>
        <translation>点击查看原图</translation>
    </message>
    <message>
        <source>Image (click to open)</source>
        <translation>图片（点击查看）</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="545"/>
        <source>Broadcast message from %1:</source>
        <translation>来自 %1 的广播消息：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="156"/>
        <source>This message was not delivered to %1:</source>
        <translation>以下消息未送达 %1：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="169"/>
        <source>Your message was not sent.</source>
        <translation>您的消息未发送。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="652"/>
        <source>Sending &apos;%1&apos; to %2.</source>
        <translation>正在向 %2 发送“%1”。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="659"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="676"/>
        <source>%1 is sending you a file:</source>
        <translation>%1 正在向您发送文件：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="678"/>
        <source>%1 is sending you a folder:</source>
        <translation>%1 正在向您发送文件夹：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="685"/>
        <source>%1 sends you a file:</source>
        <translation>%1 向您发送文件：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="703"/>
        <source>Accept</source>
        <translation>接受</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="704"/>
        <source>Decline</source>
        <translation>拒绝</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="698"/>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="750"/>
        <source>Accepted</source>
        <translation>已接受</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="753"/>
        <source>Declined</source>
        <translation>已拒绝</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="687"/>
        <source>%1 sends you a folder:</source>
        <translation>%1 向您发送文件夹：</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="756"/>
        <source>Canceled</source>
        <translation>已取消</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="760"/>
        <source>Interrupted</source>
        <translation>已中断</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="763"/>
        <source>Completed</source>
        <translation>已完成</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="777"/>
        <source>%1 is typing...</source>
        <translation>%1 正在输入...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="780"/>
        <source>%1 has entered text</source>
        <translation>%1 已输入文字</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="794"/>
        <source>%1 has joined this conversation</source>
        <translation>%1 已加入当前聊天</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="797"/>
        <source>%1 has left this conversation</source>
        <translation>%1 已离开当前聊天</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="916"/>
        <source>&amp;Copy</source>
        <translation>复制(&amp;C)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/chat/messagelog.cpp" line="917"/>
        <source>Select &amp;All</source>
        <translation>全选(&amp;A)</translation>
    </message>
</context>
<context>
    <name>lmcSettingsDialog</name>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="225"/>
        <source>Save History</source>
        <translation>保存历史记录</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="226"/>
        <source>Messenger DB (*.db)</source>
        <translation>聊天数据库 (*.db)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="232"/>
        <source>Select folder</source>
        <translation>选择文件夹</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="263"/>
        <source>Select Font</source>
        <translation>选择字体</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="269"/>
        <source>Select Color</source>
        <translation>选择颜色</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="275"/>
        <source>Are you sure you want to reset your %1 preferences?</source>
        <translation>确定要重置 %1 的首选项吗？</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="276"/>
        <source>Reset Preferences</source>
        <translation>重置首选项</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="300"/>
        <source>Hello, this is an incoming message.</source>
        <translation>你好，这是一条收到的消息。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="304"/>
        <source>Hello, this is a consecutive incoming message.</source>
        <translation>你好，这是随后收到的一条消息。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="308"/>
        <source>This is a broadcast message!</source>
        <translation>这是一条广播消息！</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="312"/>
        <source>Myself</source>
        <translation>我</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="315"/>
        <source>Hi, this is an outgoing message.</source>
        <translation>你好，这是一条发出的消息。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="319"/>
        <source>Hi, this is a consecutive outgoing message.</source>
        <translation>你好，这是随后发出的一条消息。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="341"/>
        <source>This is another incoming message.</source>
        <translation>这是另一条收到的消息。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="398"/>
        <source>&lt;File Not Found&gt;</source>
        <translation>&lt;未找到文件&gt;</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="412"/>
        <source>Select sound</source>
        <translation>选择提示音</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="413"/>
        <source>Wave Files (*.wav)</source>
        <translation>WAV 音频文件 (*.wav)</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="445"/>
        <source>Preferences</source>
        <translation>首选项</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="453"/>
        <source>System Tray (Not Available)</source>
        <translation>系统托盘（不可用）</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="457"/>
        <source>Status Alerts (Not Available)</source>
        <translation>状态提醒（不可用）</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="461"/>
        <source>Sounds (Not Available)</source>
        <translation>声音（不可用）</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="484"/>
        <source>auto</source>
        <translation>自动</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/settingsdialog.cpp" line="486"/>
        <source>Empty = automatic: try 50000, use any free port if busy. Set a value to require that exact port.</source>
        <translation>留空表示自动选择：优先尝试 50000，若被占用则使用任意空闲端口。填写数值则仅使用指定端口。</translation>
    </message>
</context>
<context>
    <name>lmcStrings</name>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="61"/>
        <source>Local-network text, pictures and files for Windows and macOS.
Derived from LAN Messenger. No account or central server required.</source>
        <translation>在 Windows 和 macOS 之间通过局域网发送文字、图片和文件。
基于 LAN Messenger 开发，无需账户或中心服务器。</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="68"/>
        <source>Automatic</source>
        <translation>自动</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="74"/>
        <source>Small text</source>
        <translation>小号文字</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="75"/>
        <source>Medium text</source>
        <translation>中号文字</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="76"/>
        <source>Large text</source>
        <translation>大号文字</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="83"/>
        <source>Available</source>
        <translation>在线</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="84"/>
        <source>Busy</source>
        <translation>忙碌</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="85"/>
        <source>Do Not Disturb</source>
        <translation>请勿打扰</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="86"/>
        <source>Be Right Back</source>
        <translation>马上回来</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="87"/>
        <source>Away</source>
        <translation>离开</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="88"/>
        <source>Appear Offline</source>
        <translation>隐身</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="95"/>
        <source>Incoming message</source>
        <translation>收到消息</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="96"/>
        <source>User is online</source>
        <translation>用户上线</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="97"/>
        <source>User is offline</source>
        <translation>用户离线</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="98"/>
        <source>Incoming file transfer</source>
        <translation>收到文件传输请求</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="99"/>
        <source>File transfer completed</source>
        <translation>文件传输完成</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="100"/>
        <source>Incoming public message</source>
        <translation>收到公共消息</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="107"/>
        <source>5 minutes</source>
        <translation>5 分钟</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="108"/>
        <source>10 minutes</source>
        <translation>10 分钟</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="109"/>
        <source>15 minutes</source>
        <translation>15 分钟</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="110"/>
        <source>20 minutes</source>
        <translation>20 分钟</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="111"/>
        <source>30 minutes</source>
        <translation>30 分钟</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="112"/>
        <source>45 minutes</source>
        <translation>45 分钟</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="113"/>
        <source>60 minutes</source>
        <translation>60 分钟</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="114"/>
        <source>Never</source>
        <translation>从不</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="121"/>
        <source>Detailed</source>
        <translation>详细</translation>
    </message>
    <message>
        <location filename="../lmc/src/core/lmcstrings.cpp" line="122"/>
        <source>Compact</source>
        <translation>紧凑</translation>
    </message>
</context>
<context>
    <name>lmcTransferWindow</name>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="179"/>
        <source>&apos;%1&apos; has been sent to %2.</source>
        <translation>已将“%1”发送给 %2。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="181"/>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="196"/>
        <source>File Transfer Completed</source>
        <translation>文件传输完成</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="194"/>
        <source>&apos;%1&apos; has been received from %2.</source>
        <translation>已接收来自 %2 的“%1”。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="319"/>
        <source>File Transfers</source>
        <translation>文件传输</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="321"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="322"/>
        <source>Show In Folder</source>
        <translation>在文件夹中显示</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="323"/>
        <source>Remove From List</source>
        <translation>从列表中移除</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="374"/>
        <source>Calculating time</source>
        <translation>正在计算剩余时间</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="379"/>
        <source>%1d </source>
        <translation>%1天 </translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="383"/>
        <source>%1h </source>
        <translation>%1小时 </translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="387"/>
        <source>%1m </source>
        <translation>%1分钟 </translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="391"/>
        <source>%1s </source>
        <translation>%1秒 </translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/files/transferwindow.cpp" line="413"/>
        <source>/sec</source>
        <translation>/秒</translation>
    </message>
</context>
<context>
    <name>lmcUpdateWindow</name>
    <message>
        <location filename="../lmc/src/gui/main/updatewindow.cpp" line="140"/>
        <source>%1 updates</source>
        <translation>%1 更新</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/updatewindow.cpp" line="159"/>
        <source>Launching an update request...</source>
        <translation>正在请求检查更新...</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/updatewindow.cpp" line="162"/>
        <source>An error occured while checking for updates.</source>
        <translation>检查更新时发生错误。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/updatewindow.cpp" line="165"/>
        <source>The new version %1 is available on %2 web site.</source>
        <translation>新版本 %1 已在 %2 网站发布。</translation>
    </message>
    <message>
        <location filename="../lmc/src/gui/main/updatewindow.cpp" line="168"/>
        <source>You have the latest version of %1.</source>
        <translation>您使用的已是最新版本的 %1。</translation>
    </message>
</context>
<context>
    <name>lmcUserInfoWindow</name>
    <message>
        <location filename="../lmc/src/gui/contacts/userinfowindow.cpp" line="78"/>
        <source>User Information</source>
        <translation>用户资料</translation>
    </message>
</context>
<context>
    <name>lmcUserSelectDialog</name>
    <message>
        <location filename="../lmc/src/gui/contacts/userselectdialog.cpp" line="133"/>
        <source>Select Contacts</source>
        <translation>选择联系人</translation>
    </message>
</context>
<context>
    <name>lmcUserTreeWidget</name>
    <message>
        <source>No devices yet
Open LanChat Lite on another computer on the same network.</source>
        <translation>暂未发现设备
请在同一局域网的另一台电脑上打开 LanChat Lite。</translation>
    </message>
</context>
<context>
    <name>ChatImage</name>
    <message>
        <source>Image unavailable</source>
        <translation>图片不可用</translation>
    </message>
</context>
</TS>

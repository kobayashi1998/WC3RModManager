#魔兽争霸3重制版Mod管理器 | WC3R Mod Manager 

该项目基于[WC3 Mod Manager](https://gitlab.com/EzraZebra/WC3ModManager),旨在为魔兽争霸3重制版于提供一个简单且可靠的Mod安装卸载工具。

最新版本为v1.1.1，下载地址[百度网盘|提取码：gbbx](https://pan.baidu.com/s/1ifYSsqwCngW3wmbKtql_9g)。

BUG反馈、建议，请到[QQ群-WC3ModManager](https://jq.qq.com/?_wv=1027&k=3z5zGFVR)。

#常见问题
##Q：如何安装Mod
A：	1.在网盘中选择Mod，解压到软件目录的Mods文件夹下
	2.在软件主界面上，点击刷新，选择一个Mod，点击安装即可

##Q：点击安装Mod后游戏内没有变化
A：	1.检查“允许本地文件”选项，确保其处于开启状态
	2.检查游戏根目录路径是否设置正确，注意区分1.27和重制版
	3.检查软件目录下Mods文件夹

# 更新日志
v1.1.1 2020年9月7日
[Chg]删除了原项目切换冰封王座/混乱之治的功能
[Chg]修改了关于页面的布局和内容，放上了仓库地址
[Fix]针对1.32.6PTR发生的游戏目录结构发生的变化进行了适配

# 核心功能
* 安装/卸载Mod - 在游戏目录下创建/删除Mod文件夹的符号链接
* 启动/关闭“允许本地文件” - 允许/不允许游戏使用本地文件/Mod

# 开发环境
项目使用 [Qt Creator 4.12.2](https://www.qt.io/download-qt-installer)开发，使用MinGW 7.3.0 64-bit进行编译构建。
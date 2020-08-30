![WC3 Mod Manager](https://raw.githubusercontent.com/EzraZebra/WC3ModManager/master/img/logo.png)

The easiest and most reliable way to make a mod in Warcraft III is using the "Allow Local Files" registry setting, which allows custom files in the game's install folder to be read by the game.

This project aims to make it as easy as possible to manage and play mods.

The latest binary can be downloaded on [The Hive Workshop](https://www.hiveworkshop.com/threads/wc3-mod-manager.308948/).

## Core Features
* Mount/Unmount mod - create symbolic links in the WC3 folder to the selected mod's files
* Toggle "Allow Local Files" - enable/disable any custom files in the WC3 folder
* Toggle "Preferred Game Version" - switch between vanilla and expansion

# Contributing
WC3 Mod Manager is currently being developed in [Qt Creator 4.7.1](https://www.qt.io/download-qt-installer) and built with Qt 5.12.0/MinGW 7.3.0 64bit.

Other IDEs should be able to open and build the project, but you'll need Qt5's libraries. MSVC should be able to compile as well, but I haven't tested.

Please feel free to fork, make pull requests, etc.

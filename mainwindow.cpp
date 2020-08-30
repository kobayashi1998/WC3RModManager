#include "ui_mainwindow.h"
#include "ui_about.h"
#include "thread.h"
#include "utils.h"

MainWindow:: MainWindow(bool newLaunchingShortcut, QMainWindow *newSplash, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    shortcutMode = newLaunchingShortcut;
    splash = newSplash;

    if(shortcutMode) connect(this, &MainWindow::shortcutQuit, qApp, &QApplication::quit, Qt::QueuedConnection);
    else
    {
        status("正在配置UI...");
        ui->setupUi(this);

        //Icons
        warningIcon.addPixmap(QPixmap(":/icons/warning.png"), QIcon::Disabled);

        refresh();

        //Menubar
        connect(ui->actionOpenGameFolder, SIGNAL(triggered()), this, SLOT(openGameFolder()));
        connect(ui->actionOpenModsFolder, SIGNAL(triggered()), this, SLOT(openModsFolder()));
        connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(openSettings()));
        connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(openAbout()));

        //Tools
        connect(ui->gameBtn, SIGNAL(clicked()), this, SLOT(launchGame()));
        connect(ui->editorBtn, SIGNAL(clicked()), this, SLOT(launchEditor()));
        connect(ui->allowFilesCbx, SIGNAL(stateChanged(int)), this, SLOT(setAllowFiles()));
        connect(ui->addModBtn, SIGNAL(clicked()), this, SLOT(addMod()));
        connect(ui->refreshBtn, SIGNAL(clicked()), this, SLOT(refresh()));

        //Modlist
        connect(ui->modList, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(renameModStart(QTableWidgetItem*)));
        QAction *actionMount = new QAction("Mount", ui->modList);
        ui->modList->addAction(actionMount);
        connect(actionMount, SIGNAL(triggered()), this, SLOT(mountMod()));
        ui->modList->addAction(ui->actionOpen);
        connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openModFolder()));
        ui->modList->addAction(ui->actionRename);
        connect(ui->actionRename, SIGNAL(triggered()), this, SLOT(renameModAction()));
        ui->modList->addAction(ui->actionDelete);
        connect(ui->actionDelete, SIGNAL(triggered()), this, SLOT(deleteMod()));

        splash->close();
        delete splash;
        splash = nullptr;
        status("准备就绪.");
    }
}

void MainWindow::openGameFolder()
{
    status("正在打开游戏文件夹...");

    if(QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(config->getSetting("GamePath")))))
        status("游戏文件夹已打开");
    else status("游戏文件夹打开失败.");
}

void MainWindow::openModsFolder()
{
    status("正在打开Mod文件夹...");

    if(QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(config->modPath))))
        status("Mod文件夹已打开.");
    else status("Mod文件夹打开失败.");
}

void MainWindow::openSettings()
{
    if(!settings) settings = new Settings(this, config);
    if(settings->exec() == 1) refresh("设置已保存.");
}

void MainWindow::openAbout()
{
    if(!about)
    {
        about = new QDialog(this);
        Ui::About uiAbout;
        uiAbout.setupUi(about);
        about->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    }
    about->show();
}

void MainWindow::refresh(QString statusMsg, QString selectedMod, bool scanMods)
{
    if(!shortcutMode)
    {
        getAllowFiles();

        if(scanMods)
        {
            int selectedRow = -1;
            if(config->getSetting("Mounted") != "") selectedMod = QString::fromStdString(config->getSetting("Mounted"));
            else if(selectedMod == "") selectedRow = ui->modList->currentRow();

            ui->modList->clearContents();
            ui->modList->setRowCount(0);

            QDirIterator itMods(QString::fromStdString(config->modPath), QDir::NoDotAndDotDot|QDir::Dirs);
            while(itMods.hasNext())
            {
                itMods.next();
                QString qsModSize = "0 MB", qsFileCount = "0 files",
                        modName = itMods.fileName();
                int row = ui->modList->rowCount();
                if(selectedMod == modName) selectedRow = row;

                ui->modList->insertRow(row);

                if(modName.toStdString() == config->getSetting("Mounted"))
                {
                    qsModSize = QString::fromStdString(config->getSetting("MountedSize"));
                    qsFileCount = QString::fromStdString(config->getSetting("MountedCount"));
                }
                else ui->modList->hideRow(row);

                ui->modList->setItem(row, 0, new QTableWidgetItem(modName));
                ui->modList->setItem(row, 1, new QTableWidgetItem(qsModSize));
                ui->modList->setItem(row, 2, new QTableWidgetItem(qsFileCount));

                if(modName.toStdString() != config->getSetting("Mounted"))
                {
                    Controller *c = new Controller(this, "扫描", modName);
                    connect(c->worker, &Worker::scanModUpdate, this, &MainWindow::scanModUpdate);
                    emit c->scanMod(row);
                }
                else
                {
                    if(config->getSetting("MountedError") != "")
                    {
                        QStringList errorList = QString::fromStdString(config->getSetting("MountedError")).split(";", QString::SkipEmptyParts);
                        if(errorList.size() > 0)
                        {
                            QString errorString = "警告:";
                            for(int i=0; i < errorList.size(); i++)
                                if(i == 0 || errorList.at(i) != errorList.at(i-1))
                                    errorString += "\n("+QString::number(i+1)+") "+errorList.at(i);

                            ui->modList->item(row, 0)->setIcon(warningIcon);
                            ui->modList->item(row, 0)->setToolTip(errorString);
                        }
                    }

                    ui->modList->resizeColumnToContents(0);

                    if(selectedRow >= 0 && selectedRow < ui->modList->rowCount())
                    {
                        ui->modList->scrollToItem(ui->modList->item(selectedRow, 0));
                        ui->modList->setFocus();
                    }
                }
            }

            if(selectedRow >= 0 && selectedRow < ui->modList->rowCount())
                ui->modList->selectRow(selectedRow);
            else if(config->getSetting("Mounted") != "")
            {
                if(QDir().mkdir(QString::fromStdString(config->modPath)+"/"+selectedMod))
                {
                    refresh(statusMsg, selectedMod);
                    return;
                }
                else statusMsg = "创建文件夹 "+selectedMod+" 失败.";
            }
        }

        getMount();
    }

    if(statusMsg == "")
    {
        if(!shortcutMode) status("已刷新.");
    }
    else status(statusMsg);
}

void MainWindow::scanModUpdate(QString modSize, QString fileCount, int row)
{
    if(modSize != "0 MB") ui->modList->item(row, 1)->setText(modSize);
    if(fileCount != "0 files") ui->modList->item(row, 2)->setText(fileCount);
    if(ui->modList->isRowHidden(row) && (fileCount != "0 files" || modSize != "0 MB" || config->getSetting("hideEmptyMods") != "1"))
    {
        ui->modList->showRow(row);
        ui->modList->resizeColumnToContents(0);
        ui->modList->scrollToItem(ui->modList->item(ui->modList->currentRow(), 0));
        ui->modList->setFocus();
    }
}

void MainWindow::launchGame()
{
    refresh("正在启动游戏...", "", false);
    if(QDesktopServices::openUrl(QUrl::fromLocalFile(
            QString::fromStdString(config->getSetting("GamePath"))+"/_retail_/x86_64/Warcraft III.exe")))
    {
        status("游戏已启动.");
        if(shortcutMode) emit shortcutQuit();
    }
    else status("游戏启动失败.", true);
}

void MainWindow::launchEditor()
{
    refresh("正在启动编辑器...", "", false);
    if(QDesktopServices::openUrl(QUrl::fromLocalFile(
            QString::fromStdString(config->getSetting("GamePath"))+"/_retail_/x86_64/World Editor.exe")))
    {
        status("编辑器已启动.");
        if(shortcutMode) emit shortcutQuit();
    }
    else status("编辑器启动失败.", true);
}

void MainWindow::setLaunchIcons()
{
    if(ui->allowFilesCbx->isChecked() && config->getSetting("Mounted") != "")
    {
        ui->gameBtn->setIcon(war3mod);
        ui->editorBtn->setIcon(worldeditmod);
    }
    else
    {
        ui->gameBtn->setIcon(war3);
        ui->editorBtn->setIcon(worldedit);
    }
}

void MainWindow::setAllowFiles(QString alwFls)
{
    DWORD allowFiles = alwFls == "0" ? 0 : alwFls == "1" ? 1 : ui->allowFilesCbx->isChecked() ? 1 : 0;

    if(utils::regSet(L"Allow Local Files", allowFiles))
    {
        if(allowFiles == 1) status("允许本地文件已开启.");
        else status("允许本地文件已关闭.");
    }
    else status("允许本地文件设置失败.", true);

    if(!shortcutMode) getAllowFiles();
}

void MainWindow::getAllowFiles()
{
    std::string allowFiles = utils::regGet(L"Allow Local Files", REG_DWORD);

    if(allowFiles=="0") ui->allowFilesCbx->setChecked(false);
    else if(allowFiles=="1") ui->allowFilesCbx->setChecked(true);
    else status("允许本地文件设置失败.");
    setLaunchIcons();
}

void MainWindow::mountMod(QString modSize, QString fileCount)
{
    bool mounting = false;
    if(!shortcutMode)
    {
        ui->toggleMountBtn->setEnabled(false);
        ui->modList->setEnabled(false);
        ui->gameBtn->setEnabled(false);
        ui->editorBtn->setEnabled(false);
    }

    if(config->getSetting("Mounted") == "")
    {
        if(shortcutMode || modSelected())
        {
            std::string gamePath = config->getSetting("GamePath")+"/_retail_";
            int iSelectedMod = 0;
            QString selectedMod;
            if(shortcutMode) selectedMod = tmpModName;
            else
            {
                iSelectedMod = ui->modList->currentRow();
                selectedMod = ui->modList->item(iSelectedMod, 0)->text();
            }

            status("正在安装 "+selectedMod+"...");

            if(QFileInfo(QString::fromStdString(gamePath)).isDir())
            {
                if(!shortcutMode)
                {
                    modSize = ui->modList->item(iSelectedMod, 1)->text();
                    fileCount = ui->modList->item(iSelectedMod, 2)->text();

                }
                config->setSetting("Mounted", selectedMod.toStdString());
                config->setSetting("MountedSize", modSize.toStdString());
                config->setSetting("MountedCount", fileCount.toStdString());
                config->saveConfig();

                Controller *c = new Controller(this, "安装", selectedMod);
                connect(c->worker, &Worker::resultReady, this, &MainWindow::mountModReady);
                emit c->mountMod();
                mounting = true;
            }
            else status("无效的魔兽争霸III重制版文件夹.", true);
        }
        else status("请选择一个Mod来安装.");
    } else
    {
        status("已安装: "+QString::fromStdString(config->getSetting("Mounted")));
        if(shortcutMode) unmountMod();
    }

    if(!mounting && !shortcutMode) getMount(true);
}

void MainWindow::mountModReady(QString modName, int success, int failed, int missing, bool abort)
{
    bool scanMods = false,
         errors = true;
    if(success > 0)
    {
        if(abort || failed+missing > 0)
        {
            scanMods = true;
            config->setSetting("MountedError", result2errorMsg("安装", success, failed, missing, abort));
            config->saveConfig();
        }
        else errors = false;
    }
    else
    {
        config->deleteSetting("Mounted");
        config->deleteSetting("MountedSize");
        config->deleteSetting("MountedCount");
        config->deleteSetting("MountedError");
        config->saveConfig();
    }

    QString statusMsg = result2statusMsg(modName, "安装", success, failed, missing, abort);
    refresh(statusMsg, modName, scanMods);

    if(shortcutMode)
    {
        if(errors && QMessageBox::warning(splash, "启动游戏?",
                        QString(abort ? "安装中止: " : "安装时发生错误:")
                            +statusMsg
                            +"\n你想继续启动游戏吗？",
                        QMessageBox::Yes|QMessageBox::No)
           == QMessageBox::No)
        {
            emit shortcutQuit();
            return;
        }

        if(shortcutEditor) launchEditor();
        else launchGame();
    }
}

void MainWindow::unmountMod()
{
    if(!shortcutMode)
    {
        ui->toggleMountBtn->setEnabled(false);
        ui->gameBtn->setEnabled(false);
        ui->editorBtn->setEnabled(false);
    }

    if(config->getSetting("Mounted") != "")
    {
        status("正在卸载 "+QString::fromStdString(config->getSetting("Mounted"))+" ...");

        Controller *c = new Controller(this, "卸载", QString::fromStdString(config->getSetting("Mounted")));
        connect(c->worker, &Worker::resultReady, this, &MainWindow::unmountModReady);
        emit c->unmountMod();
    }
    else
    {
        status("没有已安装的Mod.");

        if(shortcutMode) shortcutMountMod();
        else getMount(true);
    }
}

void MainWindow::unmountModReady(QString modName, int success, int failed, int missing, bool abort, bool force)
{
    bool unmounted = false;
    if(!force && (abort || failed+missing > 0))
    {
        std::string errorMsg = result2errorMsg("卸载", success, failed, missing, abort, force);
        if(config->getSetting("MountedError") != "") errorMsg = config->getSetting("MountedError")+";"+errorMsg;
        config->setSetting("MountedError", errorMsg);
    }
    else
    {
        unmounted = true;
        config->deleteSetting("Mounted");
        config->deleteSetting("MountedSize");
        config->deleteSetting("MountedCount");
        config->deleteSetting("MountedError");
    }

    config->saveConfig();

    QString statusMsg = result2statusMsg(modName, "卸载", success, failed, missing, abort, force);
    refresh(statusMsg, modName, unmounted);

    if(shortcutMode)
    {
        if(unmounted) shortcutMountMod();
        else if(QMessageBox::warning(splash, "重试?",
                            "卸载 "+QString(abort ? "失败: " : "中止: ")
                                +statusMsg
                                +"\n要重试一次吗?",
                            QMessageBox::Yes|QMessageBox::No)
                == QMessageBox::Yes) unmountMod();
        else emit shortcutQuit();
    }
}

void MainWindow::getMount(bool setFocus)
{
    disconnect(ui->toggleMountBtn, SIGNAL(clicked()), this, SLOT(unmountMod()));
    disconnect(ui->toggleMountBtn, SIGNAL(clicked()), this, SLOT(mountMod()));

    if(config->getSetting("Mounted") == "")
    {
        ui->toggleMountBtn->setText("安装");//&mount
        ui->modList->setStyleSheet("");
        ui->modList->setEnabled(true);
        if(setFocus) ui->modList->setFocus();
        connect(ui->toggleMountBtn, SIGNAL(clicked()), this, SLOT(mountMod()));
    }
    else
    {
        ui->toggleMountBtn->setText("卸载");//Un&mount
        ui->modList->setStyleSheet( QString("QTableWidget::item:selected { border: 2px dashed #bcbe00;")
                                   +"border-top-color: #f7f500; border-bottom-color: #f7f500;"
                                   +"color: #eee; background-color: #555; }");
        ui->modList->setEnabled(false);

        connect(ui->toggleMountBtn, SIGNAL(clicked()), this, SLOT(unmountMod()));
    }

    ui->toggleMountBtn->setEnabled(true);
    setLaunchIcons();
    ui->gameBtn->setEnabled(true);
    ui->editorBtn->setEnabled(true);
}

void MainWindow::addMod()
{
    status("正在添加Mod...");
    QString folder = QFileDialog::getExistingDirectory(this, "添加Mod", "",
                                                        QFileDialog::ShowDirsOnly | QFileDialog::HideNameFilterDetails);

    if(folder != "")
    {
        QMessageBox copyMove(this);
        copyMove.setWindowTitle("移动 或者 复制?");
        copyMove.setText("你想移动或者复制这个文件夹?\n"+folder);
        copyMove.addButton("移动", QMessageBox::ActionRole); //btn index: 0 (== PROCFILE_MOVE)
        copyMove.addButton("复制", QMessageBox::ActionRole); //btn index: 1 (== PROCFILE_COPY)
        copyMove.addButton(QMessageBox::Cancel);
        copyMove.setIcon(QMessageBox::Warning);

        int result = copyMove.exec();

        if(result == PROCFILE_MOVE || result == PROCFILE_COPY)
        {
            QString modName = QDir(folder).dirName(),
                    newFolder = QString::fromStdString(config->modPath)+"/"+modName;
            if(!QDir(newFolder).exists())
            {
                status("正在添加Mod "+modName+"...");

                Controller *c = new Controller(this, "Adding", modName);
                connect(c->worker, &Worker::resultReady, this, &MainWindow::addModReady);
                emit c->moveFolder(folder, newFolder, result);
            }
            else status("同名Mod已存在.", true);
        }
    }
}

void MainWindow::addModReady(QString modName, int success, int failed, int missing, bool abort)
{
    refresh(result2statusMsg(modName, "添加", success, failed, missing, abort), modName);
}

void MainWindow::openModFolder()
{
    QString path = QString::fromStdString(config->modPath);
    QString modName = "Mods";

    if(modSelected())
    {
        QString modName = ui->modList->item(ui->modList->currentRow(), 0)->text();
        path += "/"+modName;
    }

    status("正在打开"+modName+"文件夹...");

    if(QDesktopServices::openUrl(QUrl::fromLocalFile(path)))
        status(modName+"文件夹已打开.");
    else status(modName+"文件夹打开失败.");
}

void MainWindow::renameModAction()
{
    if(modSelected()) renameModStart(ui->modList->item(ui->modList->currentRow(), 0));
}

void MainWindow::renameModStart(QTableWidgetItem *item)
{
    if(item->column() != 0) item = ui->modList->item(item->row(), 0);
    tmpModName = item->text();
    renameModItem = item;
    connect(ui->modList, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(renameModSave(QTableWidgetItem*)));
    ui->modList->editItem(item);
}

void MainWindow::renameModSave(QTableWidgetItem *item)
{
    disconnect(ui->modList, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(renameModSave(QTableWidgetItem*)));

    QString statusMsg = "";
    QString selectedMod = tmpModName;
    if(item == renameModItem)
    {
        QString qsModPath = QString::fromStdString(config->modPath);
        if(QFileInfo(qsModPath+"/"+tmpModName).isDir())
        {
            QString newModName = item->text();
            if(!QDir().rename(qsModPath+"/"+tmpModName, qsModPath+"/"+newModName))
                statusMsg = "Mod重命名失败.";
            else
            {
                selectedMod = newModName;
                statusMsg = tmpModName+"被重命名为"+newModName+".";
            }
        }
        else statusMsg = "未找到Mod.";
    }
    else statusMsg = "正在保存错误项目.";

    tmpModName = "";
    renameModItem = nullptr;
    refresh(statusMsg, selectedMod);
}

void MainWindow::deleteMod()
{
    if(modSelected())
    {
        QString qsModName = ui->modList->item(ui->modList->currentRow(), 0)->text();

        if(QMessageBox::warning(this, "永久删除"+qsModName+"?",
                "你确定要永久的删除"+qsModName+" ("
               +ui->modList->item(ui->modList->currentRow(), 1)->text()+" /"
               +ui->modList->item(ui->modList->currentRow(), 2)->text()+")?",
                QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
        {
            status("正在删除"+qsModName+"...");

            Controller *c = new Controller(this, "Deleting", qsModName);
            connect(c->worker, &Worker::resultReady, this, &MainWindow::deleteModReady);
            emit c->deleteMod();
        }
    }
}

void MainWindow::deleteModReady(QString modName, int success, int failed, int missing, bool abort)
{
    refresh(result2statusMsg(modName, "删除", success, failed, missing, abort), modName);
}

bool MainWindow::modSelected()
{
    if(ui->modList->currentRow() < 0 || ui->modList->currentRow() >= ui->modList->rowCount())
    {
        status("没有Mod被选择.");
        return false;
    }
    return true;
}

void MainWindow::status(QString msg, bool warning)
{
    if(splash) splash->statusBar()->showMessage(msg);
    else ui->statusBarLbl->setText(msg);
    if(warning) QMessageBox::warning(activeWindow(), "错误", msg);
}

QString MainWindow::result2statusMsg(QString modName, QString action,
                                         int success, int failed, int missing, bool abort, bool force)
{
    std::map<QString, QString> dict = action_dict(action);
    QString statusMsg,
           pAction = dict.find("pAction")->second,
           cAction = dict.find("cAction")->second;
    int totErrors = failed+missing;

    if(abort) statusMsg = action+" "+modName+": 中止.";
    else if((force && totErrors > 0) || (success > 0 && !(action == "卸载" && totErrors > 0)))
        statusMsg = modName+" "+pAction+".";
    else if(totErrors > 0) statusMsg = cAction+" "+modName+"失败.";
    else statusMsg = "没有文件被"+cAction+".";

    if(abort || totErrors > 0)
    {
        statusMsg += " ["+QString::number(success)+" 文件 "+pAction;
        if(failed > 0) statusMsg += ", "+QString::number(failed)+" 文件失败";
        if(missing > 0) statusMsg += ", "+QString::number(missing)+" 文件丢失";
        statusMsg += "]";
    }

    return statusMsg;
}

std::string MainWindow::result2errorMsg(std::string action,int success, int failed, int missing, bool abort, bool force)
{
    std::map<QString, QString> dict = action_dict(QString::fromStdString(action));
    std::string errorMsg,
           pAction = dict.find("pAction")->second.toStdString(),
           cAction = dict.find("cAction")->second.toStdString(),
           pAction_c = dict.find("pAction_c")->second.toStdString();
    int totErrors = failed+missing;

    if(abort) errorMsg = action+" 中止";
    else if(force) "强制"+cAction;
    else if(success > 0 && !(action == "卸载" && totErrors > 0))
        errorMsg = pAction_c;
    else if(totErrors > 0) errorMsg = action+"失败";
    else errorMsg = "没有文件"+cAction;

    errorMsg += ": "+utils::i2s(success)+" 文件"+pAction;
    if(failed > 0) errorMsg += ", "+utils::i2s(failed)+" 文件失败";
    if(missing > 0) errorMsg += ", "+utils::i2s(missing)+" 文件丢失";

    return errorMsg;
}

std::map<QString, QString> MainWindow::action_dict(QString action)
{
    if(action == "安装")
        return {
            { "pAction", "安装" },
            { "pAction_c", "安装" },
            { "cAction", "安装" }
        };
    else if(action == "卸载")
        return {
            { "pAction", "卸载" },
            { "pAction_c", "卸载" },
            { "cAction", "卸载" }
        };
    else if(action == "添加")
        return {
            { "pAction", "添加" },
            { "pAction_c", "添加" },
            { "cAction", "添加" }
        };
    else if(action == "删除")
        return {
            { "pAction", "删除" },
            { "pAction_c", "删除" },
            { "cAction", "删除" }
        };
    else
        return {
            { "pAction", "处理" },
            { "pAction_c", "处理" },
            { "cAction", "处理" }
        };
}

MainWindow::~MainWindow()
{
    delete ui;
    delete config;
    if(splash) delete splash;
    if(settings) delete settings;
    if(about) delete about;
    if(renameModItem) delete renameModItem;
}

void MainWindow::shortcutLaunch(QString mod, bool editor)
{

    if(mod != "" && mod != nullptr)
    {
        setAllowFiles("1");
        if(mod.toStdString() != config->getSetting("Mounted"))
        {
            tmpModName = mod;
            shortcutEditor = editor;

            if(config->getSetting("Mounted") != "") unmountMod();
            else shortcutMountMod();

            return;
        }
    }
    else setAllowFiles("0");

    if(editor) launchEditor();
    else launchGame();
}

void MainWindow::shortcutMountMod()
{
    Controller *c = new Controller(this, "扫描", tmpModName);
    connect(c->worker, &Worker::scanModDone, this, &MainWindow::mountMod);
    emit c->scanMod();
}

QMainWindow* MainWindow::activeWindow()
{
    if(splash) return splash;
    else return this;
}

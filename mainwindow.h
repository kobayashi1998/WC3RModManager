#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define WINVER _WIN32_WINNT_WIN7 //Must be at least Vista for CreateSymbolicLinkA()
#ifdef _WIN32_WINNT
    #undef _WIN32_WINNT
#endif
#define _WIN32_WINNT _WIN32_WINNT_WIN7

#include "settings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow *ui;
    QMainWindow *splash=nullptr;
    Settings *settings=nullptr;
    QDialog *about=nullptr;
    QTableWidgetItem *renameModItem=nullptr;
    QString tmpModName;
    bool shortcutMode;
    bool shortcutEditor;

    void setLaunchIcons();
    void getAllowFiles();
    void getGameVersion();
    void getMount(bool=false);
    bool modSelected();
    void status(QString, bool=false);
    void shortcutMountMod();

    QString result2statusMsg(QString, QString, int, int, int, bool=false, bool=false);
    std::string result2errorMsg(std::string, int, int, int, bool=false, bool=false);
    std::map<QString, QString> action_dict(QString);

    QIcon war3 = QIcon(":/icons/war3.png");
    QIcon war3mod = QIcon(":/icons/war3_mod.png");
    QIcon worldedit = QIcon(":/icons/worldedit.png");
    QIcon worldeditmod = QIcon(":/icons/worldedit_mod.png");
    QIcon warningIcon = QIcon();

public:
    explicit MainWindow(bool, QMainWindow*, QWidget *parent = nullptr);
    ~MainWindow();
    Config *config = new Config();
    void shortcutLaunch(QString, bool);
    QMainWindow* activeWindow();

private slots:
    void openGameFolder();
    void openModsFolder();
    void openSettings();
    void openAbout();
    void refresh(QString="", QString="", bool=true);
    void scanModUpdate(QString, QString, int);
    void launchGame();
    void launchEditor();
    void setAllowFiles(QString="");
    void mountMod(QString="", QString="");
    void mountModReady(QString, int, int, int, bool=false);
    void unmountMod();
    void unmountModReady(QString, int, int, int, bool=false, bool=false);
    void addMod();
    void addModReady(QString, int, int, int, bool=false);
    void openModFolder();
    void renameModAction();
    void renameModStart(QTableWidgetItem*);
    void renameModSave(QTableWidgetItem*);
    void deleteMod();
    void deleteModReady(QString, int, int, int, bool=false);

signals:
    void shortcutQuit();
};

#endif // MAINWINDOW_H

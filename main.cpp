#include "mainwindow.h"
#include "ui_splashscreen.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_DisableWindowContextHelpButton);

    QMainWindow *splash = new QMainWindow();
    Ui::SplashScreen uiSplashScreen;
    uiSplashScreen.setupUi(splash);
    splash->setWindowFlags(Qt::SplashScreen|Qt::WindowStaysOnTopHint);
    splash->statusBar()->showMessage("启动中...");
    splash->show();

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringList() << "l" << "launch", "Mod to launch. Enter \"\" to launch without mod.", "mod", ""));
    parser.addOption(QCommandLineOption(QStringList() << "e" << "editor", "Launch World Editor."));
    parser.process(a);

    MainWindow w(parser.isSet("launch"), splash);
    w.show();

    return a.exec();
}

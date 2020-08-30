#ifndef THREAD_H
#define THREAD_H

#include "mainwindow.h"
#include <fstream>

#define PROCFILE_MOVE 0
#define PROCFILE_COPY 1
#define PROCFILE_LINK 2
#define PROCFILE_SUCCESS 0
#define PROCFILE_FAILED 1
#define PROCFILE_MISSING 2
#define PROCFILE_BACKUP_EXT ".wmmbackup"

namespace Ui {
class FileStatus;
}

class FileStatus : public QDialog
{
    Q_OBJECT

    Ui::FileStatus *ui;

public:
    explicit FileStatus(QWidget *parent = nullptr);
    ~FileStatus();
    void setText(QString);
    void setInfoText(QString);
    void addErrorText(QString);
    void result(bool=false);

private slots:
    void abort();
    void forceUnmountClicked();

signals:
    void forceUnmount(bool=true);
};

class Worker : public QObject
{
    Q_OBJECT

    Config *config;
    QString mod;
    std::ofstream out_files;
    std::ofstream backup_files;

    int deleteFile(QString);
    std::array<int, 3> mountModIterator(QString, QString);
    std::pair<int, std::string> moveFile(QString, QString, int=PROCFILE_MOVE);
    void removePath(QString, QString="");

public:
    Worker(Config*, QString="");
    bool abort = false;

public slots:
    void scanModWorker(int);
    void mountModWorker();
    void unmountModWorker(bool=false);
    void deleteModWorker();
    void moveFolderWorker(QString, QString, int=PROCFILE_MOVE);

signals:
    void scanModUpdate(QString, QString, int);
    void scanModDone(QString, QString);
    void resultReady(QString, int, int, int, bool, bool=false);
    void status(QString, bool=false);
    void appendAction(QString);
};

class Controller : public QObject
{
    Q_OBJECT

    QThread workerThread;
    QString action;
    QString mod;
    FileStatus *fileStatus=nullptr;

public:
    Controller(MainWindow *, QString, QString, bool=true);
    ~Controller();
    Worker *worker;

private slots:
    void result(QString, int, int, int, bool, bool=false);
    void status(QString, bool=false);
    void appendAction(QString);
    void abort();

signals:
    void scanMod(int=0);
    void mountMod();
    void unmountMod(bool=false);
    void deleteMod();
    void moveFolder(QString, QString, int=PROCFILE_MOVE);
};

#endif // THREAD_H

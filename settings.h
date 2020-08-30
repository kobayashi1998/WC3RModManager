#ifndef SETTINGS_H
#define SETTINGS_H

#include "config.h"
#include <QDialog>
#include <QtWidgets>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

    Ui::Settings *ui;
    Config *config;
    void loadSettings();

public:
    explicit Settings(QWidget *parent = nullptr, Config *newConfig = new Config());
    ~Settings();

private slots:
    void browseGame();
    void save(QAbstractButton *);

public slots:
    int exec();
};

#endif // SETTINGS_H

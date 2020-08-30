#include "settings.h"
#include "ui_settings.h"
#include "utils.h"

Settings::Settings(QWidget *parent, Config *newConfig) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);

    config = newConfig;

    connect(ui->dirBtn, SIGNAL(clicked()), this, SLOT(browseGame()));
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton *)), this, SLOT(save(QAbstractButton *)));
}

void Settings::loadSettings()
{
    ui->dirEdit->setText(QDir::toNativeSeparators(QString::fromStdString(config->getSetting("GamePath"))));
    ui->hideEmptyCbx->setChecked(config->getSetting("hideEmptyMods") == "1");
}

void Settings::save(QAbstractButton *btn)
{
    if(ui->buttonBox->standardButton(btn) == QDialogButtonBox::Apply)
    {
        QString gamePath = ui->dirEdit->text();
        QFileInfo fiGamePath(gamePath);
        if(!fiGamePath.exists() || !fiGamePath.isDir())
            QMessageBox::warning(this, "错误", "魔兽争霸III重制版: 无效的文件夹.");
        else
        {
            config->setSetting("GamePath", gamePath.toStdString());
            config->setSetting("hideEmptyMods", ui->hideEmptyCbx->isChecked() ? "1" : "0");
            config->saveConfig();
            accept();
        }
    }
}

void Settings::browseGame()
{
    QString folder = ui->dirEdit->text();

    folder = QFileDialog::getExistingDirectory(this, "Warcraft III Reforged Folder", folder,
                                                 QFileDialog::ShowDirsOnly | QFileDialog::HideNameFilterDetails);

    if(folder != "") ui->dirEdit->setText(QDir::toNativeSeparators(folder));
}

int Settings::exec()
{
    loadSettings();
    return QDialog::exec();
}

Settings::~Settings()
{
    delete ui;
}

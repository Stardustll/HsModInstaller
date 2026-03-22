#include "hsmodinstaller.h"
#include "./ui_hsmodinstaller.h"
#include <QSettings>

HsModInstaller::HsModInstaller(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HsModInstaller)
{
    ui->setupUi(this);
    QString path = getPath();
    if(!path.isEmpty()){
        ui->lineEdit->setText(path);
    }
}

HsModInstaller::~HsModInstaller()
{
    delete ui;
}

void HsModInstaller::on_toolButton_clicked(bool checked)
{

}

QString getPath(){
    QString regPath = "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Hearthstone";

    QSettings settings(regPath, QSettings::NativeFormat);

    return settings.value("InstallLocation").toString();
}
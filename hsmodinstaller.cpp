#include "hsmodinstaller.h"
#include "./ui_hsmodinstaller.h"
#include <QSettings>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>

HsModInstaller::HsModInstaller(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HsModInstaller)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
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
     QString dir = QFileDialog::getExistingDirectory(this,
                    "选择炉石传说根目录",
                    "C:\\Program Files (x86)",
                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty()){
         dir = QDir::toNativeSeparators(dir);
        ui->lineEdit->setText(dir);
    }
}

QString getPath(){
    QString regPath = "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Hearthstone";

    QSettings settings(regPath, QSettings::NativeFormat);

    return settings.value("InstallLocation").toString();
}
void HsModInstaller::on_pushButton_3_clicked()
{
    QString url = "http://127.0.0.1:58744";
    QDesktopServices::openUrl(QUrl(url));
}


void HsModInstaller::on_pushButton_clicked()
{
    ui->progressBar->setVisible(true);
}


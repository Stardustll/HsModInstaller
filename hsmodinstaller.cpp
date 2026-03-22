#include "hsmodinstaller.h"
#include "./ui_hsmodinstaller.h"
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QDesktopServices>
#include <QMessageBox>
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


void HsModInstaller::on_pushButton_2_clicked()
{
    QString dir = ui->lineEdit->text() + "/BepInEx";

    auto result = QMessageBox::question(this,"确认","确定要卸载吗？\n注意，此操作会删除整个BepInEx文件夹！",
                    QMessageBox::Yes|QMessageBox::No);

    if(result == QMessageBox::No){
        return;
    }

    if(QFile::exists(dir)){
        try {
            QFile::remove(dir);
            QMessageBox::information(this,"提示","卸载成功！");
        } catch (...) {
            QMessageBox::critical(this,"错误","卸载失败！");
        }
    }else{
        QMessageBox::information(this,"提示","未发现目标文件夹");
    }
}


#include "hsmodinstaller.h"
#include "./ui_hsmodinstaller.h"
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QDir>
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

// 获取炉石安装路径
QString getPath(){
    QString regPath = "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Hearthstone";

    QSettings settings(regPath, QSettings::NativeFormat);

    return settings.value("InstallLocation").toString();
}

// 打开配置网址
void HsModInstaller::on_pushButton_3_clicked()
{
    QString url = "http://127.0.0.1:58744";
    QDesktopServices::openUrl(QUrl(url));
}

// 安装mod
void HsModInstaller::on_pushButton_clicked()
{
    ui->progressBar->reset();
    ui->progressBar->setRange(0,100);
    ui->progressBar->setVisible(true);

    QString targetPath = ui->lineEdit->text();

    QStringList files = {"BepInEx.zip","HsMod.dll","HsModConfigManager.zip"};

    for(int i = 0;i<files.size();i++){
        QString src = ":/file/resources/" + files[i];
        QString dst = targetPath + "/" + files[i];

        if(QFile::exists(dst))QFile::remove(dst);
        if(QFile::copy(src,dst)){
            QFile::setPermissions(dst,QFile::WriteOwner|QFile::ReadOwner);
        }

        // 更新进度条
        ui->progressBar->setValue((i+1)*20);
        QCoreApplication::processEvents();
    }
}

// 卸载mod
void HsModInstaller::on_uninstallBtn_clicked()
{
    QString targetdir = ui->lineEdit->text() + "/BepInEx";
    QDir dir(targetdir);

    auto result = QMessageBox::question(this,"确认","确定要卸载吗？\n注意，此操作会删除整个BepInEx文件夹！",
                    QMessageBox::Yes|QMessageBox::No);

    if(result == QMessageBox::No){
        return;
    }

    if(dir.exists()){
        try {
            ui->progressBar->setRange(0,0);
            ui->progressBar->setVisible(true);
            dir.removeRecursively();
            QMessageBox::information(this,"提示","卸载成功！");
        } catch (...) {
            QMessageBox::critical(this,"错误","卸载失败！");
        }
    }else{
        QMessageBox::information(this,"提示","未发现目标文件夹");
    }
}


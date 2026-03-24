#include "hsmodinstaller.h"
#include "./ui_hsmodinstaller.h"
#include "description.h"
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>
#include <QProcess>

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

// 安装依赖
void HsModInstaller::on_pushButton_clicked()
{
    ui->progressBar->reset();
    ui->progressBar->setRange(0,100);
    ui->progressBar->setVisible(true);

    QString targetPath = ui->lineEdit->text();

    /*
    QStringList files = {"BepInEx.zip","HsMod.dll","HsModConfigManager.zip"};

    for(int i = 0;i<files.size();i++){
        QString src = ":/file/resources/" + files[i];
        QString dst = targetPath + "/" + files[i];

        if(i==1 && QDir().mkpath(targetPath+"\\BepInEx\\plugins")){
            if(QFile::exists(targetPath+"\\BepInEx\\plugins\\" + files[i]))QFile::remove(targetPath+"\\BepInEx\\plugins\\" + files[i]);
            if(QFile::copy(src,targetPath+"\\BepInEx\\plugins\\" + files[i])){
                QFile::setPermissions(dst,QFile::WriteOwner|QFile::ReadOwner);
                continue;
            }
        }

        if(QFile::exists(dst))QFile::remove(dst);
        if(QFile::copy(src,dst)){
            QFile::setPermissions(dst,QFile::WriteOwner|QFile::ReadOwner);
        }

        // 更新进度条
        ui->progressBar->setValue((i+1)*20);
        QCoreApplication::processEvents();
    }



    // 解压文件
    QString cmd = QString("powershell -Command \"Expand-Archive -Path '%1' -DestinationPath '%2' -Force\"");
    QProcess process;
    process.startCommand(cmd.arg(QDir::toNativeSeparators(targetPath+"/"+files[0]), QDir::toNativeSeparators(targetPath)));
    if (!process.waitForFinished(-1)) { // -1 表示无限等待直到完成
        QMessageBox::critical(this, "错误", "解压超时或失败！");
        return;
    }
    QFile::remove(QDir::toNativeSeparators(targetPath+"/"+files[0]));
    ui->progressBar->setValue(80);
    QCoreApplication::processEvents();

    QDir().mkpath(targetPath+"/BepInEx/HsMod/config");
    process.startCommand(cmd.arg(QDir::toNativeSeparators(targetPath+"/"+files[2]), QDir::toNativeSeparators(targetPath+"/BepInEx/HsMod/config")));
    if (!process.waitForFinished(-1)) { // -1 表示无限等待直到完成
        QMessageBox::critical(this, "错误", "解压超时或失败！");
        return;
    }
    QFile::remove(QDir::toNativeSeparators(targetPath+"/"+files[2]));
    ui->progressBar->setValue(100);
    QCoreApplication::processEvents();

    QMessageBox::information(this,"提示","安装成功！");
    */
    QString src = ":/file/resources/BepInEx.zip";
    QString dst = targetPath + "/BepInEx.zip";

    if(QFile::exists(dst))QFile::remove(dst);
    if(QFile::copy(src,dst)){
        QFile::setPermissions(dst,QFile::WriteOwner|QFile::ReadOwner);
    }

    // 更新进度条
    ui->progressBar->setValue(50);
    QCoreApplication::processEvents();

    // 解压文件
    QString cmd = QString("powershell -Command \"Expand-Archive -Path '%1' -DestinationPath '%2' -Force\"");
    QProcess process;
    process.startCommand(cmd.arg(QDir::toNativeSeparators(dst), QDir::toNativeSeparators(targetPath)));
    if (!process.waitForFinished(-1)) { // -1 表示无限等待直到完成
        QMessageBox::critical(this, "错误", "解压超时或失败！");
        return;
    }
    QFile::remove(QDir::toNativeSeparators(dst));
    ui->progressBar->setValue(100);
    QCoreApplication::processEvents();

    QMessageBox::information(this,"提示","安装成功！");
}

// 完全卸载
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
            ui->progressBar->setRange(0,100);
            ui->progressBar->setValue(100);
            QMessageBox::information(this,"提示","卸载成功！");
        } catch (...) {
            QMessageBox::critical(this,"错误","卸载失败！");
        }
    }else{
        QMessageBox::information(this,"提示","未发现目标文件夹");
    }
}


void HsModInstaller::on_illustrateBtn_clicked()
{
    Description d;
    d.show();
    d.exec();
}


void HsModInstaller::on_InsHmBtn_clicked()
{
    ui->progressBar->setValue(0);

    auto result = QMessageBox::question(this,"提示","请先确保安装好BepInEx依赖！",
                                        QMessageBox::Yes|QMessageBox::No);

    if(result == QMessageBox::No){
        return;
    }

    QString targetPath = ui->lineEdit->text();
    QString src = ":/file/resources/HsMod.dll";
    QString dst = targetPath + "\\BepInEx\\plugins\\HsMod.dll";

    if(!QFile::exists(targetPath+"\\BepInEx")){
        QMessageBox::information(this,"提示","未发现BepInEx依赖，请先进行安装！");
        return;
    }

    // 安装配置界面
    QDir().mkpath(targetPath+"/BepInEx/HsMod/config");
    if(QFile::copy(":/file/resources/HsModConfigManager.zip",targetPath+"/BepInEx/HsMod/config/HsModConfigManager.zip")){
        QString cmd = QString("powershell -Command \"Expand-Archive -Path '%1' -DestinationPath '%2' -Force\"");
        QProcess process;
        process.startCommand(cmd.arg(QDir::toNativeSeparators(targetPath+"/BepInEx/HsMod/config/HsModConfigManager.zip"),
                                     QDir::toNativeSeparators(targetPath+"/BepInEx/HsMod/config")));
        if (!process.waitForFinished(-1)) { // -1 表示无限等待直到完成
            QMessageBox::critical(this, "错误", "解压超时或失败！");
            return;
        }

        QFile::remove(QDir::toNativeSeparators(targetPath+"/BepInEx/HsMod/config/HsModConfigManager.zip"));
        ui->progressBar->setValue(80);
        QCoreApplication::processEvents();
    }

    QDir().mkpath(targetPath+"\\BepInEx\\plugins");
    if(QFile::copy(src,dst)){
        QFile::setPermissions(dst,QFile::WriteOwner|QFile::ReadOwner);
        QMessageBox::information(this,"提示","安装成功！");
    }
    ui->progressBar->setValue(100);

}


void HsModInstaller::on_InHbBtn_clicked()
{
    ui->progressBar->setValue(0);

    auto result = QMessageBox::question(this,"提示","请先确保安装好BepInEx依赖！",
                                        QMessageBox::Yes|QMessageBox::No);

    if(result == QMessageBox::No){
        return;
    }

    QString targetPath = ui->lineEdit->text();
    QString src = ":/file/resources/HsBattle.dll";
    QString dst = targetPath + "\\BepInEx\\plugins\\HsBattle.dll";

    if(!QFile::exists(targetPath+"\\BepInEx")){
        QMessageBox::information(this,"提示","未发现BepInEx依赖，请先进行安装！");
        return;
    }

    QDir().mkpath(targetPath+"\\BepInEx\\plugins");
    if(QFile::copy(src,dst)){
        QFile::setPermissions(dst,QFile::WriteOwner|QFile::ReadOwner);
        QMessageBox::information(this,"提示","安装成功！");
    }

    ui->progressBar->setValue(100);
}


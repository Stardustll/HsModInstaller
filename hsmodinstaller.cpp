#include "hsmodinstaller.h"
#include "./ui_hsmodinstaller.h"
#include "appinfo.h"
#include "description.h"
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>
#include <QProcess>
#include <QVersionNumber>

namespace {

QString versionLabelText(const QString &version, const QString &status = QString())
{
    QString labelText = QStringLiteral("当前版本：%1").arg(version.toHtmlEscaped());
    if (!status.isEmpty()) {
        labelText += QStringLiteral("（%1）").arg(status.toHtmlEscaped());
    }

    return QStringLiteral(
               "<html><head/><body><p><span style=\" color:#ffffff;\">%1</span></p></body></html>")
        .arg(labelText);
}

QString normalizeVersionText(QString versionText)
{
    versionText = versionText.trimmed();

    while (!versionText.isEmpty() && !versionText.front().isDigit()) {
        versionText.remove(0, 1);
    }

    int suffixIndex = versionText.indexOf('-');
    const int buildIndex = versionText.indexOf('+');
    if (suffixIndex < 0 || (buildIndex >= 0 && buildIndex < suffixIndex)) {
        suffixIndex = buildIndex;
    }
    if (suffixIndex >= 0) {
        versionText.truncate(suffixIndex);
    }

    return versionText;
}

QVersionNumber parseVersion(const QString &versionText)
{
    return QVersionNumber::fromString(normalizeVersionText(versionText)).normalized();
}

bool isRemoteVersionNewer(const QString &localVersionText, const QString &remoteVersionText)
{
    const QVersionNumber localVersion = parseVersion(localVersionText);
    const QVersionNumber remoteVersion = parseVersion(remoteVersionText);

    if (!localVersion.isNull() && !remoteVersion.isNull()) {
        return QVersionNumber::compare(remoteVersion, localVersion) > 0;
    }

    return !remoteVersionText.trimmed().isEmpty()
        && remoteVersionText.trimmed() != localVersionText.trimmed();
}

QString updatePageUrl(const QString &htmlUrl)
{
    if (!htmlUrl.trimmed().isEmpty()) {
        return htmlUrl;
    }

    return QStringLiteral(HSMODINSTALLER_REPOSITORY_URL "/releases/latest");
}

} // namespace

HsModInstaller::HsModInstaller(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HsModInstaller)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    ui->label_3->setText(versionLabelText(QCoreApplication::applicationVersion()));
    QString path = getPath();
    if(!path.isEmpty()){
        ui->lineEdit->setText(path);
    }

    m_networkManager = new QNetworkAccessManager(this);
    QTimer::singleShot(0, this, &HsModInstaller::checkForUpdates);
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

void HsModInstaller::checkForUpdates()
{
    const QString currentVersion = QCoreApplication::applicationVersion();
    ui->label_3->setText(versionLabelText(currentVersion, QStringLiteral("正在检查更新")));

    QNetworkRequest request(QUrl(QStringLiteral(HSMODINSTALLER_RELEASE_API_URL)));
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      QStringLiteral("HsModInstaller/%1").arg(currentVersion));
    request.setRawHeader("Accept", "application/vnd.github+json");
    request.setRawHeader("X-GitHub-Api-Version", "2022-11-28");
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
#endif

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleUpdateCheckFinished(reply);
    });
}

void HsModInstaller::handleUpdateCheckFinished(QNetworkReply *reply)
{
    const QString currentVersion = QCoreApplication::applicationVersion();
    const QString errorMessage = reply->errorString();
    const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray responseData = reply->readAll();
    const QNetworkReply::NetworkError networkError = reply->error();
    reply->deleteLater();

    if (networkError != QNetworkReply::NoError || httpStatus < 200 || httpStatus >= 300) {
        ui->label_3->setText(versionLabelText(currentVersion, QStringLiteral("更新检查失败")));
        ui->label_3->setToolTip(QStringLiteral("自动检查更新失败：%1").arg(errorMessage));
        return;
    }

    QJsonParseError parseError{};
    const QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        ui->label_3->setText(versionLabelText(currentVersion, QStringLiteral("更新检查失败")));
        ui->label_3->setToolTip(QStringLiteral("更新信息解析失败：%1").arg(parseError.errorString()));
        return;
    }

    const QJsonObject releaseObject = doc.object();
    const QString latestTag = releaseObject.value("tag_name").toString().trimmed();
    const QString releaseName = releaseObject.value("name").toString().trimmed();
    const QString releaseNotes = releaseObject.value("body").toString().trimmed();
    const QString htmlUrl = releaseObject.value("html_url").toString().trimmed();

    if (latestTag.isEmpty()) {
        ui->label_3->setText(versionLabelText(currentVersion, QStringLiteral("更新检查失败")));
        ui->label_3->setToolTip(QStringLiteral("GitHub 返回的数据里没有 tag_name。"));
        return;
    }

    if (!isRemoteVersionNewer(currentVersion, latestTag)) {
        ui->label_3->setText(versionLabelText(currentVersion, QStringLiteral("已是最新")));
        ui->label_3->setToolTip(QStringLiteral("已检查到最新版本：%1").arg(latestTag));
        return;
    }

    ui->label_3->setText(
        versionLabelText(currentVersion, QStringLiteral("可更新到 %1").arg(latestTag)));
    ui->label_3->setToolTip(QStringLiteral("检测到新版本：%1").arg(latestTag));

    QMessageBox updateDialog(this);
    updateDialog.setIcon(QMessageBox::Information);
    updateDialog.setWindowTitle(QStringLiteral("发现新版本"));
    updateDialog.setText(QStringLiteral("检测到新版本：%1").arg(latestTag));

    QString detailText = QStringLiteral("当前版本：%1").arg(currentVersion);
    if (!releaseName.isEmpty() && releaseName != latestTag) {
        detailText += QStringLiteral("\n版本名称：%1").arg(releaseName);
    }
    detailText += QStringLiteral("\n是否打开 GitHub 发布页查看更新？");
    updateDialog.setInformativeText(detailText);

    if (!releaseNotes.isEmpty()) {
        updateDialog.setDetailedText(releaseNotes);
    }

    QPushButton *openButton =
        updateDialog.addButton(QStringLiteral("打开发布页"), QMessageBox::AcceptRole);
    updateDialog.addButton(QStringLiteral("稍后再说"), QMessageBox::RejectRole);
    updateDialog.exec();

    if (updateDialog.clickedButton() == openButton) {
        QDesktopServices::openUrl(QUrl(updatePageUrl(htmlUrl)));
    }
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


#ifndef HSMODINSTALLER_H
#define HSMODINSTALLER_H

#include <QMainWindow>

class QNetworkAccessManager;
class QNetworkReply;

QT_BEGIN_NAMESPACE
namespace Ui {
class HsModInstaller;
}
QT_END_NAMESPACE

class HsModInstaller : public QMainWindow
{
    Q_OBJECT

public:
    HsModInstaller(QWidget *parent = nullptr);
    ~HsModInstaller();

private slots:
    void on_toolButton_clicked(bool checked);

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_uninstallBtn_clicked();


    void on_illustrateBtn_clicked();

    void on_InsHmBtn_clicked();

    void on_InHbBtn_clicked();

    void checkForUpdates();
    void handleUpdateCheckFinished(QNetworkReply *reply);

private:
    Ui::HsModInstaller *ui;
    QNetworkAccessManager *m_networkManager = nullptr;
};

QString getPath();
#endif // HSMODINSTALLER_H

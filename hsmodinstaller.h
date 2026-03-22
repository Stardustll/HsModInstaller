#ifndef HSMODINSTALLER_H
#define HSMODINSTALLER_H

#include <QMainWindow>

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

    void on_pushButton_2_clicked();

private:
    Ui::HsModInstaller *ui;
};

QString getPath();
#endif // HSMODINSTALLER_H

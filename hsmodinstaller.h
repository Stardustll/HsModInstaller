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

private:
    Ui::HsModInstaller *ui;
};
#endif // HSMODINSTALLER_H

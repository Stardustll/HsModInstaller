#include "hsmodinstaller.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HsModInstaller w;
    w.show();
    return a.exec();
}

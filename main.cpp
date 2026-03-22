#include "hsmodinstaller.h"
#include "detecion.h"
#include <shlobj.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!IsUserAnAdmin()){
        Detecion d;
        d.show();
        d.exec();
    }
    HsModInstaller w;
    w.show();
    return a.exec();
}

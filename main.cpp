#include "hsmodinstaller.h"
#include "appinfo.h"
#include "detecion.h"
#include <shlobj.h>
#include <QApplication>
#include <QNetworkProxyFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(QStringLiteral("HsModInstaller"));
    a.setApplicationVersion(QStringLiteral(HSMODINSTALLER_VERSION));
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    if(!IsUserAnAdmin()){
        Detecion d;
        d.show();
        d.exec();
    }
    HsModInstaller w;
    w.show();
    return a.exec();
}

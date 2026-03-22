#include "hsmodinstaller.h"
#include "./ui_hsmodinstaller.h"

HsModInstaller::HsModInstaller(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HsModInstaller)
{
    ui->setupUi(this);
}

HsModInstaller::~HsModInstaller()
{
    delete ui;
}

#include "detecion.h"
#include "ui_detecion.h"

Detecion::Detecion(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Detecion)
{
    ui->setupUi(this);
}

Detecion::~Detecion()
{
    delete ui;
}

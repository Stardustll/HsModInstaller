#ifndef DETECION_H
#define DETECION_H

#include <QDialog>

namespace Ui {
class Detecion;
}

class Detecion : public QDialog
{
    Q_OBJECT

public:
    explicit Detecion(QWidget *parent = nullptr);
    ~Detecion();

private:
    Ui::Detecion *ui;
};

#endif // DETECION_H

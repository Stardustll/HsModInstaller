#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <QDialog>

namespace Ui {
class Description;
}

class Description : public QDialog
{
    Q_OBJECT

public:
    explicit Description(QWidget *parent = nullptr);
    ~Description();

private:
    Ui::Description *ui;
};

#endif // DESCRIPTION_H

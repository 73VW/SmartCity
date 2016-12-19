#ifndef CAMION_H
#define CAMION_H

#include <QWidget>

namespace Ui {
class Camion;
}

class Camion : public QWidget
{
    Q_OBJECT

public:
    explicit Camion(QWidget *parent = 0);
    ~Camion();

    void setPosition(QPointF position);
    void setNbVelo(int nbVelo);

private:
    Ui::Camion *ui;
};

#endif // CAMION_H

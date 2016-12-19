#include "camion.h"
#include "ui_camion.h"

Camion::Camion(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Camion)
{
    ui->setupUi(this);

    QPixmap pixCam(":/img/camion.png");
    ui->lblIcon->setPixmap(pixCam);

    ui->lblNbVelo->setText("X");
}

Camion::~Camion()
{
    delete ui;
}

void Camion::setPosition(QPointF position)
{
    setGeometry(position.x(),position.y(),100,100);
}

void Camion::setNbVelo(int nbVelo)
{
   if(nbVelo>0)
       ui->lblNbVelo->setText(QString::number(nbVelo));
   else
       ui->lblNbVelo->setText("X");
}

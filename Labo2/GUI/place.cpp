#include "place.h"
#include "ui_place.h"

// un lieu à une position geographique et un nom

Place::Place(QString name,QPointF position,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Place)
{
    ui->setupUi(this);

    ui->lblName->setText(name);
    QPixmap pixPlace(":/img/place.png");
    ui->lblIcon->setPixmap(pixPlace);

    ui->lblNbVelo->setText("X");
    QPixmap pixVelo(":/img/bike.png");
    ui->lblVelo->setPixmap(pixVelo);

    setGeometry(position.x(),position.y(),100,200);

    destination = QPointF(position.x()+20,position.y()+80);
}

Place::~Place()
{
    delete ui;
}

void Place::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
    paint.drawEllipse(15,90,50,50);
}

QPointF Place::getDest()
{
    return destination;
}

void Place::setNbVelo(int nbVelo)
{
   if(nbVelo>0)
       ui->lblNbVelo->setText(QString::number(nbVelo));
   else
       ui->lblNbVelo->setText("X");
}

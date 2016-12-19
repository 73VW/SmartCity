#include "people.h"
#include "ui_people.h"

People::People(QString name,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::People)
{
    ui->setupUi(this);

    ui->lblName->setText(name);
    QPixmap pixPeople(":/img/people_action.png");
    ui->lblIcon->setPixmap(pixPeople);
}

People::~People()
{
    delete ui;
}

void People::setPosition(QPointF position)
{
    setGeometry(position.x(),position.y(),100,100);
}
// au changement d'état l'image change
void People::changeState(int state)
{
    QPixmap pixmap;
    switch (state) {
    case ParamList::WAIT:
        pixmap=QPixmap(":/img/people_wait.png");
        break;
    case ParamList::ACTION:
        pixmap=QPixmap(":/img/people_action.png");
        break;
    case ParamList::BIKE:
       pixmap=QPixmap(":/img/people_bike.png");
        break;
    default:
        break;
    }
    ui->lblIcon->setPixmap(pixmap);

}

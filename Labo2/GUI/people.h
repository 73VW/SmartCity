#ifndef PEOPLE_H
#define PEOPLE_H

#include <QWidget>
#include <QDebug>

#include "paramlist.h"

namespace Ui {
class People;
}

class People : public QWidget
{
    Q_OBJECT

public:
    explicit People(QString name,QWidget *parent = 0);
    ~People();

    void setPosition(QPointF position);
    void changeState(int state);

private:
    Ui::People *ui;
};

#endif // PEOPLE_H

#ifndef PLACE_H
#define PLACE_H

#include <QWidget>
#include <QPainter>

namespace Ui {
class Place;
}

class Place : public QWidget
{
    Q_OBJECT

public:
    explicit Place(QString name,QPointF position,QWidget *parent = 0);
    ~Place();

    virtual void paintEvent(QPaintEvent *);

    QPointF getDest();
    void setNbVelo(int nbVelo);

private:
    Ui::Place *ui;

    QPointF destination;
};

#endif // PLACE_H

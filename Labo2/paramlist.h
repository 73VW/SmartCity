#ifndef PARAMLIST_H
#define PARAMLIST_H

#include <QObject>
#include <QString>

class ParamList
{
public:
    static QString nameList[15];
    static QString siteList[15];

    static const int WAIT=1;
    static const int BIKE=2;
    static const int ACTION=3;
};

#endif // PARAMLIST_H

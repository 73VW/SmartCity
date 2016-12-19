#include "mainwindow.h"
#include <QApplication>
#include <iostream>

#include "algothread.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    int nbSite = (int)argv[0];
//    int nbHabitants= (int)argv[1];
//    int nbBorne = (int)argv[2];
//    int nbVelo=(int)argv[3];

    int nbSite = 5;
    int nbHabitants= 10;
    int nbBorne = 5;
    int nbVelo=30;

    if(nbSite >= 2 && nbBorne >= 4 && nbVelo >= nbSite*(nbBorne-2)+3)
    {
        MainWindow* mainWindow = new MainWindow(nbSite,nbHabitants,nbBorne,nbVelo);
        mainWindow->show();

        AlgoThread* algoThread = new AlgoThread(mainWindow,nbSite,nbHabitants,nbBorne,nbVelo);
        algoThread->start();

        return a.exec();
    }
    else return 0;

}

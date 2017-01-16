#include "mainwindow.h"
#include <QApplication>
#include <iostream>

#include "algothread.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    int nbSite = atoi(argv[1]);
    int nbHabitants= atoi(argv[2]);
    int nbBorne = atoi(argv[3]);
    int nbVelo = atoi(argv[4]);

    //nbSite = 5;
    //nbHabitants = 15;
    //nbBorne = 5;
    //nbVelo = 20;

    if(nbSite >= 2 && nbBorne >= 4 && nbVelo >= nbSite*(nbBorne-2)+3)
    {
        MainWindow* mainWindow = new MainWindow(nbSite,nbHabitants,nbBorne,nbVelo);
        mainWindow->show();

        AlgoThread* algoThread = new AlgoThread(mainWindow,nbSite,nbHabitants,nbBorne,nbVelo);
        algoThread->start();

        return a.exec();
    }
    else{
        std::cout << "nbSite " << nbSite << " nbHabitants " << nbHabitants << " nbBorne " << nbBorne << " nbVelo " << nbVelo << std::endl;
        system("pause");
        return 0;
    }
}

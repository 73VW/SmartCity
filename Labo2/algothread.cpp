#include "algothread.h"

AlgoThread::AlgoThread(MainWindow* mainWindow,int _nbSite,int _nbHabitants,int _nbBorne,int _nbVelo)
{
    nbSite = _nbSite;
    nbHabitants = _nbHabitants;
    nbBorne = _nbBorne;
    nbVelo = _nbVelo;

    connect(this,SIGNAL(initSite(int,int)),mainWindow,SLOT(initSite(int,int)));
    connect(this,SIGNAL(initHabitant(int,int)),mainWindow,SLOT(initHabitant(int,int)));

    connect(this,SIGNAL(setHabitantState(int,int)),mainWindow,SLOT(setHabitantState(int,int)));
    connect(this,SIGNAL(setSiteVelo(int,int)),mainWindow,SLOT(setSiteVelo(int,int)));
    connect(this,SIGNAL(startDeplacement(int,int,int,int)),mainWindow,SLOT(startDeplacement(int,int,int,int)));

    connect(this,SIGNAL(setDepotVelo(int)),mainWindow,SLOT(setDepotVelo(int)));
    connect(this,SIGNAL(setCamVelo(int)),mainWindow,SLOT(setCamVelo(int)));
    connect(this,SIGNAL(startCamionDeplacement(int,int,int)),mainWindow,SLOT(startCamionDeplacement(int,int,int)));
}

void* runHabitants(void* arguments)
{
    //MAKE SOME CHANGE HERE
    struct initHab* hab = (struct initHab*)arguments;
    AlgoThread* algoThread = ((AlgoThread*)hab->algoThread);

    emit algoThread->initSite(1,2);
    emit algoThread->initSite(2,3);
    emit algoThread->initSite(3,3);
    emit algoThread->initSite(4,4);
    emit algoThread->initHabitant(1,1);
    emit algoThread->initHabitant(2,1);
    emit algoThread->initHabitant(3,2);
    emit algoThread->initHabitant(4,3);
    emit algoThread->initHabitant(5,4);
    emit algoThread->initHabitant(6,3);
    emit algoThread->initHabitant(7,4);
    emit algoThread->initHabitant(8,2);

    emit algoThread->startDeplacement(1,1,2,10);
    emit algoThread->startDeplacement(2,2,3,8);
    emit algoThread->startDeplacement(3,3,4,11);
    emit algoThread->startDeplacement(4,4,1,5);
    emit algoThread->startDeplacement(5,1,2,3);
    emit algoThread->startDeplacement(6,2,3,7);
    emit algoThread->startDeplacement(7,3,4,9);
    emit algoThread->startDeplacement(8,4,1,12);

}


void* runMaintenance(void* arguments)
{
    //MAKE SOME CHANGE HERE
    struct initDep* dep = (struct initDep*)arguments;
    AlgoThread* algoThread = (AlgoThread*)dep->algoThread;

    emit algoThread->setDepotVelo(1);
    emit algoThread->initCamion();
}

void AlgoThread::run()
{
    //MAKE SOME CHANGE HERE
    struct initHab* hab = new initHab();
    hab->algoThread = this;

    pthread_t thread_hab;
    pthread_create (&thread_hab, NULL, runHabitants, (void*)hab);

    struct initDep* dep = new initDep();
    dep->algoThread = this;

    pthread_t thread_maint;
    pthread_create (&thread_maint, NULL, runMaintenance, (void*)dep);

    pthread_join (thread_hab, NULL);
    pthread_join (thread_maint, NULL);
}

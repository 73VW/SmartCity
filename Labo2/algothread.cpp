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
    //MAKE SOME CHANGES HERE
    struct initHab* hab = (struct initHab*)arguments;
    AlgoThread* algoThread = ((AlgoThread*)hab->algoThread);


    //@TODO destination, temps de trajet aléatoire
    //@TODO temps de pause aléatoire à ajouter
    //@TODO attente de vélo libre avant de déplacement


    int tempsDeTrajet=10;
    emit algoThread->startDeplacement(hab->id,hab->posDep,hab->posArr,tempsDeTrajet);


    //@TODO lieu de départ prochain passage  = lieu d'arrivée
    hab->posDep=hab->posArr;
}


void* runMaintenance(void* arguments)
{
    //MAKE SOME CHANGES HERE
    struct initDep* dep = (struct initDep*)arguments;
    AlgoThread* algoThread = (AlgoThread*)dep->algoThread;

    emit algoThread->setDepotVelo(1);
    emit algoThread->initCamion();
}

void AlgoThread::run()
{

    // instantiation des sites
    emit this->initSite(1,2);
    emit this->initSite(2,3);
    emit this->initSite(3,3);
    emit this->initSite(4,4);


    //MAKE SOME CHANGES HERE
    struct initHab* hab[this->nbHabitants];


    //@TODO Créer autant de threads qu'il y a d'habitants ! --> OK

    //@TODO tirer aléatoirement le premier lieu de départ de chaque habitant
    pthread_t tabThread_hab[this->nbHabitants];
    for(int cptHabitants = 0; cptHabitants<this->nbHabitants; cptHabitants++){
        hab[cptHabitants] = new initHab();
        hab[cptHabitants]->algoThread = this;
        hab[cptHabitants]->id=cptHabitants;
        hab[cptHabitants]->posDep=cptHabitants%this->nbSite;
        //@TODO position d'arrivée à tirer aléatoirement depuis le thread fils
        hab[cptHabitants]->posArr=(cptHabitants*7)%this->nbSite;
        emit this->initHabitant(cptHabitants,hab[cptHabitants]->posDep);
        pthread_create(&tabThread_hab[cptHabitants], NULL, runHabitants, (void*)hab[cptHabitants]);
    }



    struct initDep* dep = new initDep();
    dep->algoThread = this;

    pthread_t thread_maint;
    pthread_create (&thread_maint, NULL, runMaintenance, (void*)dep);



    for(int cptHabitants = 0; cptHabitants<this->nbHabitants; cptHabitants++){
        pthread_join (tabThread_hab[cptHabitants], NULL);
    }
    pthread_join (thread_maint, NULL);
}

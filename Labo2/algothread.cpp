#include "algothread.h"
#include "time.h"

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

    mutex = new QMutex*[nbSite]();
    conditionsArr = new QWaitCondition*[nbSite]();
}

void* runHabitants(void* arguments)
{
    struct initHab* hab = (struct initHab*)arguments;
    AlgoThread* algoThread = ((AlgoThread*)hab->algoThread);
    emit algoThread->setHabitantState(hab->id, 1);

    while(1){

    hab->posArr=rand()%hab->nbSite;
    while(hab->posArr==hab->posDep)
       hab->posArr=rand()%hab->nbSite;

    hab->mutex[hab->posDep]->lock();
    if(hab->nbVeloSite[hab->posDep]>=1){
        hab->nbVeloSite[hab->posDep]--;
        hab->conditionsArr[hab->posDep]->wakeOne();
        emit algoThread->setHabitantState(hab->id, 2);
        emit algoThread->startDeplacement(hab->id,hab->posDep,hab->posArr,hab->tempsTrajet);
        emit algoThread->setSiteVelo(hab->posDep, hab->nbVeloSite[hab->posDep]);
        hab->mutex[hab->posDep]->unlock();
        Sleep(hab->tempsTrajet*1000);


        hab->mutex[hab->posArr]->lock();
        if(hab->nbVeloSite[hab->posArr]>=hab->nbBorne)
            hab->conditionsArr[hab->posArr]->wait(hab->mutex[hab->posArr]);
        emit algoThread->setHabitantState(hab->id, 3);
        hab->nbVeloSite[hab->posArr]++;
        emit algoThread->setSiteVelo(hab->posArr, hab->nbVeloSite[hab->posArr]);
        hab->mutex[hab->posArr]->unlock();
        Sleep(hab->tempsAttente*1000);
        emit algoThread->setHabitantState(hab->id, 1);
        hab->posDep=hab->posArr;
    }
    else{
        hab->mutex[hab->posDep]->unlock();
    }
    hab->tempsTrajet=rand()%10+1;
    hab->tempsAttente=rand()%10+1;


    }
}


void* runMaintenance(void* arguments)
{
    struct initDep* dep = (struct initDep*)arguments;
    AlgoThread* algoThread = (AlgoThread*)dep->algoThread;

    emit algoThread->setDepotVelo(dep->nbVelo);
    emit algoThread->initCamion();

    int tempsTrajet=rand()%10+1;
    int posArr;
    int nbVeloCamion;
    int c;
    while(1){
        int posDep=-1;
        Sleep(10000);

        if(dep->nbVelo>=2)
            nbVeloCamion=2;
        else
            nbVeloCamion=dep->nbVelo;

        dep->nbVelo-=nbVeloCamion;
        emit algoThread->setDepotVelo(dep->nbVelo);
        emit algoThread->setCamVelo(nbVeloCamion);
        for(int i=0; i<dep->nbSite; i++){
            posArr=i;
            emit algoThread->startCamionDeplacement(posDep, posArr, tempsTrajet);
            Sleep(tempsTrajet*1000);
            dep->mutex[i]->lock();
            if(nbVeloCamion<4&&dep->nbVeloSite[i]>dep->nbBorne-2){
                //si le nombre de vélos à emporter est > au nombre de place dans le camion
                if(dep->nbVeloSite[i]-(dep->nbBorne-2)>4-nbVeloCamion){
                    //on emporte le nombre de vélos correspondant au nombre de place
                    c=4-nbVeloCamion;
                }
                //sinon si le nombre de vélos à emporter est <= au nb de place
                else{
                    //on emporte tous les vélos en trop
                    c=dep->nbVeloSite[i]-(dep->nbBorne-2);
                }
                dep->nbVeloSite[i]-=c;
                for(int cpt=0; cpt<c;cpt++)
                    dep->conditionsArr[i]->wakeOne();
                nbVeloCamion+=c;
            }
            else if(nbVeloCamion>0&&dep->nbVeloSite[i]<dep->nbBorne-2){
                //si le nombre de vélos à déposer est > que le nb de vélos dans le camion
                if(dep->nbBorne-2-dep->nbVeloSite[i]>nbVeloCamion){
                    //on dépose le nombre de vélos dans le camion
                    c=nbVeloCamion;
                }
                //sinon si le nombre de vélos à déposer est <= au nb de vélos dans le camion
                else{
                    //on dépose autant de vélos que nécessaire
                    c=dep->nbBorne-2-dep->nbVeloSite[i];
                }
                dep->nbVeloSite[i]+=c;
                nbVeloCamion-=c;
            }
            emit algoThread->setSiteVelo(i, dep->nbVeloSite[i]);
            emit algoThread->setCamVelo(nbVeloCamion);
            dep->mutex[i]->unlock();
            posDep=posArr;
        }
        emit algoThread->startCamionDeplacement(posDep, posDep+1, tempsTrajet);
        Sleep(tempsTrajet*1000);
        dep->nbVelo+=nbVeloCamion;
        emit algoThread->setDepotVelo(dep->nbVelo);
        emit algoThread->setCamVelo(0);

    }
}



void AlgoThread::run()
{

    int nbVeloParSite[nbSite];
    // instantiation des sites
    for(int cpt=0; cpt<nbSite; cpt++){
        nbVeloParSite[cpt]=nbBorne-2;
        emit this->initSite(cpt, nbVeloParSite[cpt]);
        mutex[cpt] = new QMutex();
        conditionsArr[cpt]= new QWaitCondition();
    }
    int nbVeloDepot = nbVelo - nbSite*(nbBorne-2);

    struct initHab* hab[this->nbHabitants];

    srand(time(NULL));
    pthread_t tabThread_hab[this->nbHabitants];
    for(int cptHabitants = 0; cptHabitants<this->nbHabitants; cptHabitants++){
        hab[cptHabitants] = new initHab();
        hab[cptHabitants]->nbVeloSite = nbVeloParSite;
        hab[cptHabitants]->nbSite = this->nbSite;
        hab[cptHabitants]->nbBorne = this->nbBorne;
        hab[cptHabitants]->algoThread = this;
        hab[cptHabitants]->mutex = this->mutex;
        hab[cptHabitants]->id=cptHabitants;
        hab[cptHabitants]->posDep=rand()%nbSite;
        hab[cptHabitants]->tempsTrajet=rand()%5+5;
        hab[cptHabitants]->tempsAttente=rand()%5+1;
        hab[cptHabitants]->conditionsArr=this->conditionsArr;
        emit this->initHabitant(cptHabitants,hab[cptHabitants]->posDep);
        pthread_create(&tabThread_hab[cptHabitants], NULL, runHabitants, (void*)hab[cptHabitants]);
    }



    struct initDep* dep = new initDep();
    dep->algoThread = this;
    dep->nbVelo = nbVeloDepot;
    dep->nbSite = this->nbSite;
    dep->nbVeloSite = nbVeloParSite;
    dep->nbBorne=this->nbBorne;
    dep->mutex = this->mutex;
    dep->conditionsArr=this->conditionsArr;

    pthread_t thread_maint;
    pthread_create (&thread_maint, NULL, runMaintenance, (void*)dep);



    for(int cptHabitants = 0; cptHabitants<this->nbHabitants; cptHabitants++){
        pthread_join (tabThread_hab[cptHabitants], NULL);
    }
    pthread_join (thread_maint, NULL);
}

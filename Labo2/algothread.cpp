/*               Programmation Concurrente
 *                  Labo2
 *                 Laurent Gander et Maël Pedretti
 *               HE-Arc 2017
 */


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
    //récupération des arguments et du thread parent
    struct initHab* hab = (struct initHab*)arguments;
    AlgoThread* algoThread = ((AlgoThread*)hab->algoThread);

    //les habitants attendent
    emit algoThread->setHabitantState(hab->id, 1);

    //boucle infinie
        while(1){

        //tirage aléatoire de la position d'arrivée des habitants
        hab->posArr=rand()%hab->nbSite;
        //tant que la position d'arrivée = position de départ, on retire au sort
        while(hab->posArr==hab->posDep)
           hab->posArr=rand()%hab->nbSite;

        //verrouillage du mutex du lieu de départ
        hab->mutex[hab->posDep]->lock();
        //s'il y a un vélo disponible, l'habitant peut se déplacer
        if(hab->nbVeloSite[hab->posDep]>=1){
            //changement du nombre de vélos sur le site de départ
            hab->nbVeloSite[hab->posDep]--;
            emit algoThread->setSiteVelo(hab->posDep, hab->nbVeloSite[hab->posDep]);
            //réveil d'un des threads qui attenderait sur une place libre (s'il y en a un)
            hab->conditionsArr[hab->posDep]->wakeOne();
            //lancement du déplacement de l'habitant
            emit algoThread->setHabitantState(hab->id, 2);
            emit algoThread->startDeplacement(hab->id,hab->posDep,hab->posArr,hab->tempsTrajet);
            //déverrouillage du mutex du point de départ durant le temps de trajet
            hab->mutex[hab->posDep]->unlock();
            Sleep(hab->tempsTrajet*1000);
            //verrouillage du mutex du lieux d'arrivée
            hab->mutex[hab->posArr]->lock();
            //si toutes les bornes sont occupées, on attend qu'une se libère
            if(hab->nbVeloSite[hab->posArr]>=hab->nbBorne)
                hab->conditionsArr[hab->posArr]->wait(hab->mutex[hab->posArr]);
            //indication que l'habitant est arrivé
            emit algoThread->setHabitantState(hab->id, 3);
            //changement du nombre de vélos sur le site d'arrivée
            hab->nbVeloSite[hab->posArr]++;
            emit algoThread->setSiteVelo(hab->posArr, hab->nbVeloSite[hab->posArr]);
            //libération du mutex
            hab->mutex[hab->posArr]->unlock();
            Sleep(hab->tempsAttente*1000);
            //l'habitant se met en attente jusqu'à ce que la boucle recommence
            emit algoThread->setHabitantState(hab->id, 1);
            //la nouvelle position de départ est l'ancienne position d'arrivée
            hab->posDep=hab->posArr;
            //tirage aléatoire du nouveau temps de trajet et du nouveau temps d'attente
            hab->tempsTrajet=rand()%10+1;
            hab->tempsAttente=rand()%10+1;
        }
        //s'il n'y a pas de vélo libre on libère le mutex et on ne fait rien
        else{
            hab->mutex[hab->posDep]->unlock();
        }
    }
}


void* runMaintenance(void* arguments)
{
    //récupération des arguments et du thread parent
    struct initDep* dep = (struct initDep*)arguments;
    AlgoThread* algoThread = (AlgoThread*)dep->algoThread;

    //mise en place du nombre de vélos au dépôt et lancement du camion
    emit algoThread->setDepotVelo(dep->nbVelo);
    emit algoThread->initCamion();

    //tirage au sort du temps de trajet
    int tempsTrajet=rand()%10+1;
    int posArr;
    int nbVeloCamion;
    int c; //nombre de vélos chargés ou déchargés sur le site
    //boucle infinie
    while(1){
        //position de départ à -1 car dépôt
        int posDep=-1;
        //attente de 10 secondes entre chaque tournée du camion
        Sleep(10000);

        //si le nombre de vélos au dépot est supérieur ou égal à 2, chargement de 2 vélos
        if(dep->nbVelo>=2)
            nbVeloCamion=2;
        //sinon chargement du nombre qu'il y en a (1 ou 0)
        else
            nbVeloCamion=dep->nbVelo;

        emit algoThread->setCamVelo(nbVeloCamion);
        //décompte du dépôt le nombre de vélos qu'il y a dans le camion
        dep->nbVelo-=nbVeloCamion;
        emit algoThread->setDepotVelo(dep->nbVelo);
        //on boucle sur tous les sites
        for(int i=0; i<dep->nbSite; i++){
            posArr=i;
            //déplacement du camion sur le site souhaité
            emit algoThread->startCamionDeplacement(posDep, posArr, tempsTrajet);
            Sleep(tempsTrajet*1000);
            //verrouillage du mutex du site
            dep->mutex[i]->lock();
            //si le camion n'est pas plein et qu'il faut emporter des vélos
            if(nbVeloCamion<4&&dep->nbVeloSite[i]>dep->nbBorne-2){
                //si le nombre de vélos à emporter est > au nombre de place dans le camion
                if(dep->nbVeloSite[i]-(dep->nbBorne-2)>4-nbVeloCamion){
                    //le nombre de vélos correspondant au nombre de place dans le camion est emporté
                    c=4-nbVeloCamion;
                }
                //sinon si le nombre de vélos à emporter est <= au nb de place
                else{
                    //tous les vélos en trop sont emportés
                    c=dep->nbVeloSite[i]-(dep->nbBorne-2);
                }
                //mise à jour de la variable contenant le nombre de vélos sur le site
                dep->nbVeloSite[i]-=c;
                //réveil d'autant d'habitants qui attendent que de vélos emportés
                for(int cpt=0; cpt<c;cpt++)
                    dep->conditionsArr[i]->wakeOne();
                //mise à jour de la variable contenant le nombre de vélos dans le camion
                nbVeloCamion+=c;
            }
            else if(nbVeloCamion>0&&dep->nbVeloSite[i]<dep->nbBorne-2){
                //si le nombre de vélos à déposer est > que le nb de vélos dans le camion
                if(dep->nbBorne-2-dep->nbVeloSite[i]>nbVeloCamion){
                    //le nombre de vélos dans le camion est déposé
                    c=nbVeloCamion;
                }
                //sinon si le nombre de vélos à déposer est <= au nb de vélos dans le camion
                else{
                    //autant de vélos que nécessaire sont déposés
                    c=dep->nbBorne-2-dep->nbVeloSite[i];
                }
                //mise à jour de la variable contenant le nombre de vélos sur le site
                dep->nbVeloSite[i]+=c;
                //mise à jour de la variable contenant le nombre de vélos dans le camion
                nbVeloCamion-=c;
            }
            //mise à jour du nombre de vélos sur le site et dans le camion
            emit algoThread->setSiteVelo(i, dep->nbVeloSite[i]);
            emit algoThread->setCamVelo(nbVeloCamion);
            //déverouillage du mutex
            dep->mutex[i]->unlock();
            //l'ancienne position de départ est la nouvelle position d'arrivée
            posDep=posArr;
        }
        //le camion est déplacé au dépôt quand tous les sites ont été visités
        emit algoThread->startCamionDeplacement(posDep, posDep+1, tempsTrajet);
        Sleep(tempsTrajet*1000);
        //les vélos dans le camion sont déposés au dépôt
        dep->nbVelo+=nbVeloCamion;
        emit algoThread->setDepotVelo(dep->nbVelo);
        emit algoThread->setCamVelo(0);
    }
}



void AlgoThread::run()
{

    int nbVeloParSite[nbSite];
    // instantiation des sites, de leur mutex et de leur condition (on ne peut pas avoir plus de vélos par sites que le nombre de bornes)
    for(int cpt=0; cpt<nbSite; cpt++){
        nbVeloParSite[cpt]=nbBorne-2;
        emit this->initSite(cpt, nbVeloParSite[cpt]);
        mutex[cpt] = new QMutex();
        conditionsArr[cpt]= new QWaitCondition();
    }
    //le nombre de vélos au dépot correspond au nombre de vélos choisi auquel on soustrait combien de vélos
    //on a ajouté sur les sites
    int nbVeloDepot = nbVelo - nbSite*(nbBorne-2);

    //struct pour les habitants
    struct initHab* hab[this->nbHabitants];

    //départ du random pour les trajets
    srand(time(NULL));

    //autant de threads qu'il y a d'habitants
    pthread_t tabThread_hab[this->nbHabitants];

    //création de la structure pour les habitants
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
        //initialisation des habitants avec leurs paramètres de départ
        emit this->initHabitant(cptHabitants,hab[cptHabitants]->posDep);
        //création du thread correspondant à chaque habitant
        pthread_create(&tabThread_hab[cptHabitants], NULL, runHabitants, (void*)hab[cptHabitants]);
    }


    //struct pour le camion
    struct initDep* dep = new initDep();
    dep->algoThread = this;
    dep->nbVelo = nbVeloDepot;
    dep->nbSite = this->nbSite;
    dep->nbVeloSite = nbVeloParSite;
    dep->nbBorne=this->nbBorne;
    dep->mutex = this->mutex;
    dep->conditionsArr=this->conditionsArr;

    //déclaration et création du thread du camion
    pthread_t thread_maint;
    pthread_create (&thread_maint, NULL, runMaintenance, (void*)dep);



    //on attend sur tous les threads avant que quitter la fonction
    for(int cptHabitants = 0; cptHabitants<this->nbHabitants; cptHabitants++){
        pthread_join (tabThread_hab[cptHabitants], NULL);
    }
    pthread_join (thread_maint, NULL);
}

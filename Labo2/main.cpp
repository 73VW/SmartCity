/*               Programmation Concurrente
 *                  Labo2
 *                 Laurent Gander et Maël Pedretti
 *               HE-Arc 2017
 *
 */

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

    //grace a l'utilisation de la fonction atoi(), si le paramètre entré n'est pas un nombre, 0 est retourné
    //il suffit donc de tester si les paramètres sont supérieurs à 0 et s'ils correspondent aux paramètres
    //requis pour le bon fonctionnement du programme
    //si ce n'est pas le cas, un erreur est affichée

    if(nbSite >= 2 && nbBorne >= 4 && nbVelo >= nbSite*(nbBorne-2)+3 && nbHabitants > 0)
    {
        MainWindow* mainWindow = new MainWindow(nbSite,nbHabitants,nbBorne,nbVelo);
        mainWindow->show();

        AlgoThread* algoThread = new AlgoThread(mainWindow,nbSite,nbHabitants,nbBorne,nbVelo);
        algoThread->start();

        return a.exec();
    }
    else{
        std::cout << "un ou plusieurs parametres entres sont invalides :"<<std::endl
                  << "le nombre de site doit etre superieur a 2. Valeur entree : " << nbSite << std::endl
                  << "le nombre d'habitants doit etre superieur a 0. Valeur entree : " << nbHabitants << std::endl
                  << "le nombre de bornes doit etre superieur a 4. Valeur entree : " << nbBorne << std::endl
                  << "le nombre de velos doit etre superieur a nbSite*(nbBorne-2)+3. valeur entree : "<< nbVelo << std::endl;
        system("pause");
        return 0;
    }
}

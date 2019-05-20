#ifndef __EXECUTERCOMMANDE_H
#define __EXECUTERCOMMANDE_H


//Nombre d'arguments de la ligne de commande
int nbrArguments(char ** cmd);

//Retourne le nombre de commandes
int nombreCommandes(struct cmdline *line);

//Execute une commande passée en paramètre
//Prend le nom des redirections on NULL s'il n'y en a pas
// arrierePlan : le programme tourne en arrière plan si égal à 1
void executerCmd(char ** cmd,char *nomEntree, char * nomSortie,char arrierePlan);

//Pour exécuter des commandes avec des tuyaux 
void executerCmdTuyau(struct cmdline *line);
#endif

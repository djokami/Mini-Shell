#ifndef __PIDPREMIERPLAN_H
#define __PIDPREMIERPLAN_H

//Gestion du pid du programme en premier plan (et de son nom)
// pid=0 retourne la valeur du pid au premier plan
// pid>0 change le pid au premier plan
// pid=-1 pas de programme au premier plan
int pidPremierPlan(int pid,char * nomCmd);


//Fonctions pour simplifier l'utilisation de pidPremierPlan
int obtenirPidNomPremierPlan(char *str);
int obtenirPidPremierPlan();
int enleverPidPremierPlan();
int mettrePidPremierPlan(int pid, char * nomCmd);

#endif
#include <stdlib.h>
#include <string.h>
//Gestion du pid du programme en premier plan (et de son nom)
// pid=0 retourne la valeur du pid au premier plan
// pid>0 change le pid au premier plan
// pid=-1 pas de programme au premier plan
int pidPremierPlan(int pid,char * nomCmd) {
	static int p = -1;
	static char nom [100];
	if(pid==0) {
		if(nomCmd!=NULL) {
			strcpy(nomCmd,nom); //on récupère le nom de la commande
		}
		return p;	
	}
	else if(pid>0) {
		p = pid;
		strcpy(nom,nomCmd); //On sauvegarde le nom  de la commande
		return 0;
	}
	else {
		p = -1;
	}
	return -1;
}
//Fonctions pour simplifier l'utilisation de pidPremierPlan
int obtenirPidNomPremierPlan(char *str) {
	return pidPremierPlan(0, str);
}
int obtenirPidPremierPlan() {
	return pidPremierPlan(0, NULL);
}
int enleverPidPremierPlan() {
	return pidPremierPlan(-1, NULL);
}
int mettrePidPremierPlan(int pid, char * nomCmd) {
	return pidPremierPlan(pid, nomCmd);
}
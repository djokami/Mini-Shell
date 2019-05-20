/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

/*
./sdriver.pl -t ./tests/test01.txt -s ./src/shell
*/

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include "couleur.h"
#include "job.h"
#include "pidPremierPlan.h"
#include "executerCommande.h"

//Permet d'exterminer les processus zombis quand un fils meurt
void exterminerZombis() {
	int status;
	int pid;
	while ( (pid=waitpid(-1, &status, WNOHANG))>0) { //Tant qu'il y a des processus enfants morts
		enleverJob(chercherNumJobPid(pid)); //On les enlève des jobs
	}
	return;
}

//Pour tuer le processus au premier plan quans on fait ctr-c
void tuerProcessus() {
	int pid = obtenirPidPremierPlan();
	if(pid!=-1) {
		enleverJob(chercherNumJobPid(pid)); //On supprimer le job
		Kill(pid,SIGINT); //On tue le processus
		enleverPidPremierPlan(); //Il n'est plus au premier plan
	}
	printf("\n");
	return;
}

//Pour arréter le processus au premier plan quand on fait ctr-z
void arreterProcessus() {
	int pid = obtenirPidPremierPlan();
	if(pid!=-1) {
		stopJob(chercherNumJobPid(pid)); //On met à jour la table des jobs
		Kill(pid,SIGTSTP); //On le stop
		enleverPidPremierPlan(); //Le processus n'est plus au premier plan
	}
	printf("\n");
	return;
}


int main(int argc, char * argv[])
{
	//Quand un fils meurt
	Signal(SIGCHLD,exterminerZombis);
	//Quand on fait un ctr-c
	Signal(SIGINT,tuerProcessus);
	//Quand on fait un ctr-z
	Signal(SIGTSTP,arreterProcessus);	

	initialiserJobs(); //Initialise la table des jobs

	while (1) {
		struct cmdline *l;

		//Lecture de la commande
		printf(Bleu_gras"shell> "Blanc);
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			Kill(0,SIGINT); //On tue tous les processus du groupe
			printf("exit\n");
			exit(0);
		}

		if (l->err) {
			/* Erreur de syntaxe, lit une nouvelle commande */
			printf("Erreur: %s\n", l->err);
			continue;
		}

		//Information de debug
		if (argc==2 && !strcmp(argv[1],"-d")) {
			int i, j;
			if (l->in) printf("in: %s\n", l->in);
			if (l->out) printf("out: %s\n", l->out);
			if (l->bg) printf("Commande en arrière plan\n");
			for (i=0; l->seq[i]!=0; i++) {
				char **cmd = l->seq[i];
				printf("seq[%d]: ", i);
				for (j=0; cmd[j]!=0; j++) {
					printf("%s \n", cmd[j]);
				}
			}
		}

		char **cmd = l->seq[0];
		if(nombreCommandes(l)==0) {//Si on rentre rien dans la ligne de commandes
		}
		else if(nombreCommandes(l)>1) { //Si on a un tuyau
			executerCmdTuyau(l);
		} 
		else { //Si commande sans tuyau
			//Quitter le shell
			if(!strcmp("quit",cmd[0]) || !strcmp("q",cmd[0])) {
				Kill(0,SIGINT); //On tue tous les processus du groupe
				exit(0);
			}

			//Affichage des jobs
			else if(!strcmp("jobs",cmd[0]) || !strcmp("j",cmd[0])) {
				afficherJobs();
			}

			//Met en arrière plan une tache
			else if(nbrArguments(cmd)==2 && !strcmp("bg",cmd[0]) ) {
				bg(atoi(cmd[1]));
			}
			else if(nbrArguments(cmd)==1 && !strcmp("bg",cmd[0]) ) {
				bg( getJobs()->tab[0].numJob);//Prend le premier job
			}

			//Met au premier plan
			else if(nbrArguments(cmd)==2 && !strcmp("fg",cmd[0])) {
				fg(atoi(cmd[1]));
			}
			else if(nbrArguments(cmd)==1 && !strcmp("fg",cmd[0]) ) {
				fg(getJobs()->tab[0].numJob); 
			}

			//Arrète un programme
			else if(nbrArguments(cmd)==2 && !strcmp("stop",cmd[0])) {
				stop(atoi(cmd[1]));
			}
			else if(nbrArguments(cmd)==1 && !strcmp("stop",cmd[0]) ) {
				stop(getJobs()->tab[0].numJob);
			}

			//On execute commande shell
			else {
				executerCmd(cmd,l->in,l->out,l->bg);
			}
		}
	}
}

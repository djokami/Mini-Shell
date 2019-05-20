#include <wordexp.h>
#include "csapp.h"
#include "readcmd.h"
#include "executerCommande.h"
#include "couleur.h"
#include "pidPremierPlan.h"
#include "job.h"


//Nombre d'arguments de la ligne de commande
int nbrArguments(char ** cmd) {
	int nb=0;
	while(cmd[nb]!=NULL) {
		nb++;
	}
	return nb;
}

//Retourne le nombre de commandes
int nombreCommandes(struct cmdline *line) {
	int nb=0;
	while(line->seq[nb]!=NULL) {
		nb++;
	}
	return nb;
}

//Execute une commande passée en paramètre
//Prend le nom des redirections on NULL s'il n'y en a pas
// arrierePlan : le programme tourne en arrière plan si égal à 1
void executerCmd(char ** cmd,char *nomEntree, char * nomSortie,char arrierePlan) {
	int status=-1;
	
	//Le nom de la commande pour le mettre dans le jobs
	char nomCommande[100]; nomCommande[0]=0; int i=0;
	while (cmd[i]!=NULL) {
		strcat(nomCommande,cmd[i]);
		strcat(nomCommande," ");
		i++;
	}

	int pid=Fork();
	if(pid == 0) { //Fils
		if(nomEntree!=NULL) {
			int fdEntree = open(nomEntree, O_RDONLY,0);
			if(fdEntree<0) { //Erreur ouverture du fichier
				printf("%sImpossible d'ouvrir en lecture le fichier: %s%s\n",Rouge_gras,nomEntree,Blanc);
				exit(0);
			}
			dup2(fdEntree,0); //Entrée standard devient fichier
			close(fdEntree);
		}
		if(nomSortie!=NULL) {
			int fdSortie = open(nomSortie,O_WRONLY | O_TRUNC | O_CREAT,S_IRWXU);
			if(fdSortie<0) { //Erreur ouverture du fichier
				printf("%sImpossible d'ouvrir en écriture le fichier: %s%s\n",Rouge_gras,nomSortie,Blanc);
				exit(0);
			}
			dup2(fdSortie,1); //Sortie standard devient fichier
			close(fdSortie);
		}

		wordexp_t p;
		wordexp(nomCommande, &p, 0); //Remplace les caractères spéciaux
		execvp(cmd[0],p.we_wordv);

		printf("%sCommande inconnue: %s%s\n",Rouge_gras,nomCommande,Blanc); //Si exec a echoué la commande était inconnue
		exit(1);
	}
	else { //le père
		ajouterJob(pid,nomCommande,1); //Ajout d'un nouveau job
		if (!arrierePlan) { //Si le programme est au premier plan
			mettrePidPremierPlan(pid,nomCommande);
			waitpid(pid,&status,0|WUNTRACED); //Attend que l'exécution du fils soit terminée or arrétée
			if (!WIFSTOPPED(status) && !WIFSIGNALED(status)) { //Si le programme est mort naturellement (sans signal)
				enleverJob(chercherNumJobPid(pid)); //On l'enlève de la table des jobs
			}
			enleverPidPremierPlan();
		}
	}
	return;
}


//Pour exécuter des commandes avec des tuyaux 
void executerCmdTuyau(struct cmdline *line) {
	int nbC = nombreCommandes(line);
	//Allocation des canaux de communication
	int **canaux = malloc(sizeof(int *)*(nbC-1));
	if (canaux==NULL) {
		afficher_rouge_gras("Erreur allocation mémoire\n");
		exit(1);
	}
	for(int i=0;i<nbC-1;i++) {
		canaux[i] = malloc(sizeof(int)*2);
		if (canaux[i]==NULL) {
			afficher_rouge_gras("Erreur allocation mémoire\n");
			exit(1);
		}
	}

	//La première commande
	//Prend comme entrée l'entrée standard
	//Comme sortie l'entrée du canal
	pipe(canaux[0]); //Canal de communication entre les 2 fils
	int pid = Fork();
	if (pid==0) { //Le fils
		close(canaux[0][0]); //Ferme sortie du canal
		dup2(canaux[0][1],1); //Sortie standard -> entrée du canal
		close(canaux[0][1]);
		executerCmd(line->seq[0],line->in,NULL,line->bg);
		exit(0);
	}
	waitpid(pid,0,0);
	close(canaux[0][1]); 

	//Les commandes intermédiares (si plusieurs pipe)
	//Prend comme entrée la sortie de la commande précédante (sortie du canal précédent)
	//Prend comme sortie l'entrée du canal courant
	for(int iC=1; iC<nbC-1; iC++) {
		pipe(canaux[iC]); //Canal de communication entre les 2 fils
		int pidI = Fork();
		if (pidI==0) { //Le fils
			dup2(canaux[iC-1][0],0); //Entrée standard = sortie du canal précédent 
			close(canaux[iC-1][0]); //Ferme sortie du canal
			dup2(canaux[iC][1],1); //Sortie standard -> entrée du canal
			close(canaux[iC][1]);
			executerCmd(line->seq[iC],NULL,NULL,line->bg);
			exit(0);
		}
		waitpid(pidI,0,0);
		close(canaux[iC-1][0]);
		close(canaux[iC][1]);
	}

	//La dernière commande
	//Prend comme entrée l'avant dernière commande
	//Prend comme sortie la sortie standard
	int pid2 = Fork();
	if(pid2==0) { //Le second fils
		close(canaux[nbC-2][1]); //Ferme entrée du canal
		dup2(canaux[nbC-2][0],0); //Entrée standard = sortie du canal 
		close(canaux[nbC-2][0]);
		executerCmd(line->seq[nbC-1],NULL,line->out,line->bg);
		exit(0);
	}
	waitpid(pid2,0,0);
	close(canaux[nbC-2][0]);
}
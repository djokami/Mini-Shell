#include "csapp.h"
#include "job.h"
#include "couleur.h"
#include "pidPremierPlan.h"

jobSeq * jobSequence(int opt) {
    static jobSeq jS;
    if(opt==0) {
        jS.n=0;
    }

    return &jS;
}
void initialiserJobs() {
    jobSequence(0);
}
jobSeq * getJobs() {
    return jobSequence(1);
}

//Cherche le job de numéro numeroJob dans la séquence et renvoie son indice
int chercherJob(int numeroJob) {
    jobSeq jS = *getJobs();
    int i=0;
    while(i<jS.n && jS.tab[i].numJob != numeroJob) {//On cherche le job
        i++;
    }
    if (i==jS.n) { //Erreur job non trouvé
        i=-1;
    }
    return i;
}
//Cherche le job de pid pid dans la séquence et renvoie son numero job
int chercherNumJobPid(int pid) {
    jobSeq jS = *getJobs();
    int i=0;
    while(i<jS.n && jS.tab[i].pid != pid) {//On cherche le job
        i++;
    }
    if (i==jS.n) { //Erreur job non trouvé
        i=-1;
    }
    return jS.tab[i].numJob ;
}

//Affichage des jobs
void afficherJobs () {
    jobSeq jS = *getJobs();
    if (jS.n)
        printf("PID   Numéro job   Status                 Commande\n");
    for(int i=0;i<jS.n;i++) {
        printf("%-6d",jS.tab[i].pid);
        printf("%-13d",jS.tab[i].numJob);
        if(!jS.tab[i].status) {
            printf("%-25s","Arrêté");
        }
        else {
            printf("%-24s","En cours d'exécution");
        }
        printf("%s\n",jS.tab[i].nomCommande);
    }
}

void ajouterJob (int pid,char * nom, int status) {
    jobSeq * jS = getJobs();
    job J;
    J.pid=pid;
    //Lui donne un numéro de job
    if(jS->n==0) {
        J.numJob=1;
    }
    else {
        J.numJob = jS->tab[jS->n-1].numJob+1;
    }
    J.nomCommande = malloc((strlen(nom)+1)*sizeof(char));
    strcpy(J.nomCommande,nom);
    J.status = status;

    jS->tab[jS->n] = J; //Ajoute dans le tableau
    jS->n++;
    if(jS->n==NombreMaxJob) { 
        afficher_rouge_gras("Limite job atteinte\n");    
    }
}

//Enlève le job
void enleverJob (int numJob) {
    jobSeq * jS = getJobs();
    int i=chercherJob(numJob);
    if (i==-1) { //Pas trouver le job
        printf("%sJob %d non trouvé\n%s",Rouge_gras,numJob,Blanc);
    }
    else {//Trouver le job
        free(jS->tab[i].nomCommande);
        for(int iD=i;iD<jS->n-1 ;iD++) { //On décale les valeurs
            jS->tab[iD]=jS->tab[iD+1];
        }
        jS->n = jS->n-1;
    }
}

//Arrète le job
void stopJob(int numeroJob) {
    jobSeq * jS = getJobs();
    int i=chercherJob(numeroJob);
    if(i==-1) { //Job introuvable
        printf("%sImpossible de mettre en arrière plan: le job %d est introuvable!%s\n",Rouge_gras,numeroJob,Blanc);
    }
    jS->tab[i].status = 0;
}

//Met en arrière plan le programme
void bg(int numeroJob) {
    jobSeq * jS = getJobs();
    int i=chercherJob(numeroJob);
    if(i==-1) { //Job introuvable
        printf("%sImpossible de mettre en arrière plan: le job %d est introuvable!%s\n",Rouge_gras,numeroJob,Blanc);
    }
    else {
        if (jS->tab[i].status == 1) { //Tache déjà en arrière plan
            printf("%sImpossible de mettre en arrière plan: le job %d est déjà en arrière plan!%s\n",Rouge_gras,numeroJob,Blanc);
        }
        else {
            jS->tab[i].status = 1;
            kill(jS->tab[i].pid,SIGCONT); //Envoie un signal au programme pour lui dire de reprendre l'exécution en tache de fond
        }
    }
}

//Met au premier plan le programme
void fg(int numeroJob) {
    jobSeq * jS = getJobs();
    int i=chercherJob(numeroJob);
    if(i==-1) { //Job introuvable
        printf("%sImpossible de mettre au premier plan: le job %d est introuvable!%s\n",Rouge_gras,numeroJob,Blanc);
    }
    else {
        if(jS->tab[i].status==0) { //Si le programme en arrière plan est arrété en le lance
            jS->tab[i].status = 1;
            kill(jS->tab[i].pid,SIGCONT);
        }
        int status;
        mettrePidPremierPlan(jS->tab[i].pid,jS->tab[i].nomCommande); //Le programme est maintenant au premier plan
        waitpid(jS->tab[i].pid,&status,0|WUNTRACED);
        if (!WIFSTOPPED(status) && !WIFSIGNALED(status)) { //Si le programme est mort naturellement (sans signal)
			enleverJob(chercherNumJobPid(jS->tab[i].pid)); //On l'enlève de la table des jobs
		}
        enleverPidPremierPlan();
    }
}

//Stop le programme
void stop(int numeroJob) {
    jobSeq * jS = getJobs();
    int i=chercherJob(numeroJob);
    if(i==-1) { //Job introuvable
        printf("%sImpossible de stopper: le job %d est introuvable!%s\n",Rouge_gras,numeroJob,Blanc);
    }
    else {
        if(jS->tab[i].status==1) { //Si le programme est en cours d'exécution on l'arrète
            jS->tab[i].status = 0;
            kill(jS->tab[i].pid,SIGSTOP);
        }
    }
}
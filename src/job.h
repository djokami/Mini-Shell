#ifndef __JOB_H
#define __JOB_H

#define NombreMaxJob 50

//Un job
typedef struct job {
    int pid;
    int numJob;
    int status;
    char * nomCommande; //La commande exécutée
    // 0 = arrété; 1 = en cours d'exécution (en arrière plan)
 
} job;

//Le tableau des jobs
typedef struct jobSeq {
    struct job tab[NombreMaxJob];
    int n;//Nombre de job
} jobSeq;

//Pour gérer la séquence des job et donc éviter qu'elle soit en variable globale
jobSeq * jobSequence(int opt);
void initialiserJobs();
jobSeq * getJobs();

//Affichage des jobs
void afficherJobs ();

//Ajout d'un job dans la structure
void ajouterJob (int pid,char * nom, int status);

//Enlever le job
void enleverJob (int  numJob);

//Arrète le job (il existe tjs mais est arrété)
void stopJob( int numeroJob);

//Mettre en arrière plan le programme de job numeroJob
void bg( int numeroJob);

//Mettre au premier plan le programme de job numeroJob
void fg( int numeroJob);

//Stop l'exécution d'un programme
void stop( int numeroJob);

//Renvoie l'indice du tableau où se trouve le job recherché
int chercherJob(int numeroJob);

//Retourner le numero de job correspondant au pid
int chercherNumJobPid(int pid);
#endif
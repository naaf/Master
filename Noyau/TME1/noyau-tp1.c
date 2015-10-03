#include <setjmp.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>


/****************************************/
/* Pour la commutation avec les signaux */
/*          2eme partie du TP           */
/****************************************/

	#define MASK_ALARM()    sigprocmask(SIG_BLOCK, &mask_alarm, 0)
	#define UNMASK_ALARM()  sigprocmask(SIG_UNBLOCK, &mask_alarm, 0)
	#define delay 400000

	void commut();

	sigset_t mask_alarm;

	void handler_clock(int n) {
		commut();
	}

	void initSig() {
		struct sigaction sa;

		sigemptyset(&mask_alarm);
		sigaddset(&mask_alarm, SIGALRM);
	
		sigaction(SIGALRM, 0, &sa);
		sa.sa_handler = handler_clock;
		sa.sa_flags |= SA_RESTART;
		sigaction(SIGALRM, &sa, 0);
		ualarm(delay, delay);
	}

/*************************/
/* Gestion des processus */
/*************************/

// On utilisera cette variable "globale" pour retenir l'adresse du haut de la pile
char *top_stack;

// la dernière entrée du tableau (NPROC) est utilisée pour sauvegarder le main
#define SNONE  0
#define SSLEEP 1
#define SRUN   2
#define MAX_STACK_SIZE 1000
#define NPROC 10

// Cette structure contient tous ce qui necessaire a la gestion d'un processus
struct proc_s {
	int       flag;
	sigjmp_buf   buf;
	char      stack[MAX_STACK_SIZE];
	int       size;
} tproc[NPROC+1];

// La derniere entree du tableau (NPROC) est utilisee pour sauvegarder le main
struct proc_s tproc[NPROC+1];

// Identifiant du processus courant
int cur = -1;

int election(int old) {

	// #####################################
	// ############ A COMPLETER ############
	// #####################################
	
	// Cette fonction implemente un simple round robin :
	// => les taches accedent au CPU les une après les autres sans se doubler
	// => on cherche (circulairement) dans le tableau le prochain processus pret (
	// Valeur de retour :
	// - index du processus elu qui doit etre a l'etat SRUN
	// - NPROC (id du main) s'il n'y a plus de processus
	//
	// Attention : cette fonction de modifie pas la variable "cur". 
	// Elle ne fait que retourner l'index d'un processus.
	int i = (old + 1) % (NPROC + 1);
	while(i != old){
		if(tproc[i].flag == SRUN){
			return i;		
		}

		i = (i + 1) % (NPROC + 1);
	} 
	return NPROC;

}

int new_proc(int (*nomFonction)(int), int argFonction) {

	// #####################################
	// ############ A COMPLETER ############
	// #####################################
	
	// Au demarrage :
	// 1) chercher une entree libre dans la table proc
	// 2) initialiser la structure
	// 3) sauvegarder le processus
	// 4) retourner 0
	// Apres le retour :
	// 1) demarrer la fonction
	// Apres la fin de la foncton (fin du processus)
	// 2) liberer l'entree (etat et desallocation pile)
	// 3) forcer le commut pour laisser la place a un autre processus.
	int i;
	for(i = 0; i <= NPROC; i++){
		if(tproc[i].flag == SNONE){
			tproc[i].flag = SRUN;
			break;		
		}
	}
	if(sauvegarderProcessus(i) == 0 ){
		return 0;
	}else{
		nomFonction(argFonction);
		tproc[i].flag = SNONE;
		commut();
		
	}
 	

}

/***************************************/
/* Fonctions permettant la commutation */
/***************************************/

int sauvegarderProcessus(int idx) {

	// #####################################
	// ############ A COMPLETER ############
	// #####################################

	// Lors de l'appel a la fonction :
	// 1) sauvegarder la pile
	// 2) sauvegarder de la taille de pile. Pourquoi la variable locale ne suffit pas ?
	// 3) sauvegarder les registres
	// 4) retourner 0
	int n=0;
	int val;
	if(val=sigsetjmp(tproc[idx].buf,mask_alarm)==0){
		
		tproc[idx].size =  (void*)top_stack - (void*)&n;
		//printf("proc %d size %d \n",idx,tproc[idx].size);
		memcpy(tproc[idx].stack, &n, tproc[idx].size);

		return 0;
	}
	
	else{
		// En cas de restauration :
		// 1) restaurer la pile
		// 2) UNMASK_ALARM() dans la deuxieme partie (avec les signaux)
		// 3) retourne la valeur fournie par le longjmp
		
		memcpy(top_stack-tproc[cur].size, tproc[cur].stack, tproc[cur].size);
		UNMASK_ALARM(); 
		return val;
	}
	
	

}

int restaurerProcessus(int idx) {
	
	// #####################################
	// ############ A COMPLETER ############
	// #####################################

	// 1) enregistre l'index du nouveau processus (idx) comme processus courant
	// 2) lance la restauration des pointeurs
	// ?) la restauration de la pile peut elle avoir lieu dans cette fonction ?
	cur= idx;
	siglongjmp(tproc[idx].buf, idx);
}

void commut() {
	MASK_ALARM();
	if(sauvegarderProcessus(cur) == 0)
		restaurerProcessus(election(cur));
	UNMASK_ALARM();
}

int f(int p) {
	int n = 0;
	int j;
	for(j=0; j<15; j++) { // 20
		printf("f(%d) -> %d\n", p, n++);
		usleep(500000);
		// La ligne suivante est a commenter dans la deuxième partie du TP
		//commut();
	}
	return 0;
}

int g(int p) {
	int n = 0;
	int j;
	for(j=0; j<12; j++) {// 50
		printf("g(%d) -> %d\n", p, n++);
		usleep(300000);
		// La ligne suivante est a commenter dans la deuxième partie du TP
		//if(j%2 == 0) commut();
	}
	return 0;
}

int main() {
	
	// #####################################
	// ############ A COMPLETER ############
	// #####################################
	
	//  sauvegarder l'adresse du sommet de la pile dans la variable globale "top_stack"
	char toto;
	top_stack= &toto;

	new_proc(g,0);
	new_proc(f,4);

	// Comme explique plus haut le main a pour id NPROC (derniere case du
	// tableau)
	cur = NPROC;

	// Dans un deuxieme temps vous decommenter la ligne suivante pour activer les
	//signaux
	initSig();
	commut();

	printf("exit!!!\n");
	return 0;
}

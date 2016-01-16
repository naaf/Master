#include <stdio.h>
#include <stdlib.h>
#include <synch.h>
#include <sched.h>
#include <proc.h>
#include <callo.h>

/**
 * @fn int tsleep(int pri, void *obs)
 * Permet au processus de s'endormir sur une ressource
 * @param pri : priorit� au reveil
 * @param obs : ressource observ�e
 * @return rend 0
 */
int tsleep(int pri, void *obs) {
  // TME - completer
  int p = GetElecProc();
  if (p == -1) {
    printf("tsleep - no current process\n");
    exit(1);
  }
   Tproc[p].p_flag = SLEEP;
   Tproc[p].p_pri = pri;
   Tproc[p].p_ptr = obs;
   commut(NULL);
  
  return 0;
}

/**
 * @fn int twakeup(void *obs)
 * Permet au processus de reveiller l'ensemble des processus endormis sur une certaine ressource 
 * @param obs : ressource observ�e
 * @return rend -1 en cas d'erreur sinon 0
 */
int twakeup(void *obs) {
  // TME - completer
  struct proc *p;
  int wake = 0;
  for( p = &Tproc[0]; p < &Tproc[MAXPROC+1]; p++){
    if(p->p_ptr == obs){
      p->p_flag = RUN;
      p->p_ptr = 0;
      if(p->p_pri < Tproc[getfid()].p_pri){
	wake = 1;
	break;
      }
    }
  }
  
   if(wake)
    commut(NULL);
  return 0;
}


/**
 * @fn int tsleep_time(int sec)
 * Permet au processus de s'endormir pendant une dur�e exprim�e en seconde
 * @param sec : dur�e de l'endormissement
 * @return rend -1 en cas d'erreur sinon 0
 */
int tsleep_time(int sec) {
  
  int p = GetElecProc();
  
  if (p == -1) {
    fprintf(stderr,"sleep - no current process\n");
    exit(1);
  }
  
  printf("SLEEP(%d);\n\n", sec);
  fflush(stdout);
  
  timeout((void*)twakeup, &Tproc[p], sec*Ticks_par_sec); 
  tsleep(MAXUSERPRIO, &Tproc[p]);
  
  printf("SLEEP(%d)... reveil\n\n", sec);
  fflush(stdout);

  return 0;
}

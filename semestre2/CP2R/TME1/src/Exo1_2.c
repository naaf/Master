///*
// * Q2.c
// *
// *  Created on: 18 janv. 2016
// *      Author: 3100837
// */
//

/**
 *  la memoire n'est pas  partage entre les processus
 */

//#include <stdio.h>
//#include <stdlib.h>
//
//int main(void) {
//
//	int i = 0;
//	pid_t ret = fork();
//
//	if (ret == 0) { // fils
//		sleep(4);
//		printf("fils i = %d", i);
//
//	} else { //pere
//		printf(" pere entrer une valeur(i) ==> ");
//		scanf("%d", &i);
//		printf(" i == > %d \n", i);
//
//	}
//
//	return EXIT_SUCCESS;
//}
//

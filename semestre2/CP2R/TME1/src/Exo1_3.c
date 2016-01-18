//
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <pthread.h>
//
//int i = 0;
//
//void *THREAD_Routine(void *arg) {
//	sleep(10);
//	printf(" thread pid %d \n ", getpid());
//	printf(" thread (i) == > %d \n", i);
//	return NULL;
//}
//
//int main(void) {
//
//	pthread_t thread_id;
//
//	if (pthread_create(&thread_id, NULL, THREAD_Routine, NULL) != 0) {
//		perror("pthread_create");
//		exit(EXIT_FAILURE);
//
//	}
//	printf("pere %d entrer valeur (i) ==> ",getpid());
//	scanf("%d", &i);
//
//	if (pthread_join(thread_id, NULL) != 0) {
//		perror("ptread_join");
//		exit(EXIT_FAILURE);
//	}
//	printf(" pere pid %d (i) == > %d \n", getpid(), i);
//	return EXIT_SUCCESS;
//}
//

/*
 * robotsQuiRicochent.h
 *
 *  Created on: 8 mars 2016
 *      Author: naaf
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#define PORT_SRV 2016

#define CONNEXION "CONNEXION"
#define SORT "SORT"
#define ENCHERE "ENCHERE"
#define SOLUTION "SOLUTION"
/**
 * connexion avec le serveur
 * return socket TCP
 */
int connex_socket(char* addrIp, int port);



/** Send method **/
void connex_session(int socket, char* nameUser);
void deconnex_session(int socket, char* nameUser);
void tour_sendreflexion(int socket, char* user, int nbcoups);
void enchere_sendcoups(int socket, char* user, int nbcoups);
void tour_sendsolution(int socket, char* user, char* deplacements);





/** receive method **/
char* session_getresponse(int socket);
char* session_gettour(int socket);
char* session_getplateau(int socket);
char* session_getbilan(int socket);
char* reflexion_getresponse(int socket);
char* enchere_getresponse(int socket);
char* solution_getresponse(int socket);




#endif /* COMMUNICATION_H_ */

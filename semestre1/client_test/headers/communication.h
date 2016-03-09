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
#define BIENVENUE "BIENVENUE"
#define CONNECTE "CONNECTE"
#define SORT "SORT"
#define DECONNEXION "DECONNEXION"
#define SESSION "SESSION"
#define VAINQUEUR "VAINQUEUR"
#define TOUR "TOUR"
#define SOLUTION "SOLUTION"
#define TUASTROUVE "TUASTROUVE"
#define ILATROUVE "ILATROUVE"
#define FINREFLEXION "FINREFLEXION"
#define ENCHERE "ENCHERE"
#define VALIDATION "VALIDATION"
#define ECHEC "ECHEC"
#define NOUVELLEENCHERE "NOUVELLEENCHERE"
#define FINENCHERE "FINENCHERE"
#define SASOLUTION "SASOLUTION"
#define BONNE "BONNE"
#define MAUVAISE "MAUVAISE"
#define FINRESO "FINRESO"
#define TROPLONG "TROPLONG"

#define CHAT "CHAT"

/**
 * connexion avec le serveur
 * return socket TCP
 */
int connex_socket(char* addrIp, int port);
void send_request(int sc, int argc, ...);
char* read_response(int sc);

/** receive method **/
char* session_getresponse(int socket);
char* session_gettour(int socket);
char* session_getplateau(int socket);
char* session_getbilan(int socket);
char* reflexion_getresponse(int socket);
char* enchere_getresponse(int socket);
char* solution_getresponse(int socket);

#endif /* COMMUNICATION_H_ */

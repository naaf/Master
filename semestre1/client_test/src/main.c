/*
 * main.c
 *
 *  Created on: 8 mars 2016
 *      Author: naaf
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/ricochet_robot.h"
#include "../headers/communication.h"

int main(int argc, char** argv) {
//	int sc;
//	sc = connex_socket("127.0.0.1", 2016);
//
//	send_request(sc, 2, CONNEXION, "Ashraf");
//	read_response(sc);
//
//	send_request(sc, 3, SOLUTION, "Ashraf", "4");
//	read_response(sc);
//
//	send_request(sc, 3, SOLUTION, "Ashraf", "RDRHVDVHVDRB");
//	read_response(sc);
	plateau_t pl;
	int x, y;
	char c;
	c = y = x = 0;
	char* ch = "(3,4,H)(3,4,G)(12,6,H)";
	char* ch1 = "(0r,0r,1b,1b,2j,2j,3v,3v,4c,4c,W)";
	printf("lu %d \n", strlen(ch1));
	sscanf(ch1+1+(4*6), "%d%c,%d%c,", &x,&c, &y, &c);
	sscanf(ch1+1+(5*6), "%c",&c);
	printf("lu %d %d %c\n", x, y, c);

//	sscanf(ch+7, "(%d,%d,%c)", &x, &y, &c);
//	printf("%c\n",ch[7]);
	return EXIT_SUCCESS;
}

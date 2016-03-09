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
	int sc;
	sc = connex_socket("127.0.0.1", 2016);

	send_request(sc, 2, CONNEXION, "Ashraf");
	read_response(sc);

	send_request(sc, 3, SOLUTION, "Ashraf", "4");
	read_response(sc);

	send_request(sc, 3, SOLUTION, "Ashraf", "RDRHVDVHVDRB");
	read_response(sc);

	return EXIT_SUCCESS;
}

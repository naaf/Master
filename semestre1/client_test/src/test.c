/*
 * test.c
 *
 *  Created on: 11 mars 2016
 *      Author: naaf
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/ricochet_robot.h"
#include "../headers/communication.h"

void test_aff(list_user_t* l) {
	user_t *cur;
	cur = l->first;
	printf(" nb_client %d \n", l->nb);
	while (cur != NULL) {
		printf("user %s %d\n", cur->name, cur->score);
		cur = cur->next;
	}
}

void test_users() {
	bilan_t bil;
	bil.list_users.nb = 0;
	bil.list_users.first = NULL;

	adduser("ashraf", &bil.list_users);
	adduser("nasser", &bil.list_users);
	adduser("ali", &bil.list_users);

	test_aff(&bil.list_users);

	printf("------------------------\n");
	removeuser("ashraf1", &bil.list_users);
	test_aff(&bil.list_users);
	printf("------------------------\n");
	user_t *usr = getuser("ali", &bil.list_users);
	printf("usr %s , %d \n", usr->name, usr->score);

	free_list_user(&bil.list_users);
}
void test_parse() {
	plateau_t pl;
	enigme_t eni;
	bilan_t bil;

	char* ch = "(13,4,H)(3,14,G)(2,6,H)";
	char* ch1 = "(13r,5r,1b,1b,12j,2j,3v,13v,4c,4c,R)";
	char* ch2 = "6(saucisse,223)(brouette,0)";

	parse_enigme(ch1, &eni);
	parse_plateau(ch, pl);
	parse_bilan(ch2, &bil);
}

void test_comm() {
	printf("test communication avec srv \n");
	int sc;
	char response[512];
	memset(response, 0, 512);
	sc = connex_socket("127.0.0.1", 2016);

	send_request(sc, 2, CONNEXION, "Ashraf");
	read_response(sc, response);
	printf("recu : %s", response);

	send_request(sc, 3, SOLUTION, "Ashraf", "4");
	read_response(sc, response);
	printf("recu : %s", response);

	send_request(sc, 3, SOLUTION, "Ashraf", "RDRHVDVHVDRB");
	read_response(sc, response);
	printf("recu : %s", response);

	int i = 0, size = 0;
	char **tab = NULL;
	tab = string_to_arraystring(response, &size, '/');
	for (i = 0; i < size; i++) {
		printf("part%d : %s\n", i, tab[i]);
	}
	free_table(tab, size);

}

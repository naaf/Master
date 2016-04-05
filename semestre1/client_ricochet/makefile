LEX = flex
YACC = bison -d
CC = gcc

LFLAGS = -Iinclude -L./lib -lSDL2 -lSDL2_image -lSDL2_ttf -lpthread -lrt
CFLAGS =-W -Wall -ansi -pedantic --Wl,-rpath,./lib
BIN =bin/
OBJ =obj/
INCLUDE =include/
LIB =lib/
SRC =src/
 gcc src/main.c src/SDLS.c src/ricochet_robot.c src/ihm.c src/visualisation.c src/communication.c -o exe -I./include -Wl,-rpath,./lib -L./lib -lSDL2 -lSDL2 -lSDL2_image -lSDL2_ttf -lpthread -lrt


.PHONY: all clean 

APS : aps

all : client_ricochet

test :
	./aps test.txt

client_ricochet: $(OBJ)
	$(CC)  -o $@ $^ $(LFLAGS)

./objet/%.o: ./src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean: 
	rm -rf $(OBJ)*.o 

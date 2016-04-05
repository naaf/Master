
CC = gcc

LFLAGS =  -L./lib -lSDL2 -lSDL2_image -lSDL2_ttf -lpthread -lrt
CFLAGS =-W -Wall -ansi -pedantic 
BIN =bin/
OBJ =obj/
INCLUDE =include/
LIB =lib/
SRC =src/



.PHONY: all clean 

all : client_ricochet


client_ricochet: $(OBJ)
	$(CC)  -o $@ $^ $(LFLAGS)

./objet/%.o: ./src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean: 
	rm -rf $(OBJ)*.o 

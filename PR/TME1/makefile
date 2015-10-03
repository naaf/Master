CC=gcc
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=
EXEC=./bin/max_calcul


all: $(EXEC)

./bin/max_calcul: ./objet/max_calcul.o ./objet/max_func.o ./include/max_func.h
	$(CC) -o $@ $^ $(LDFLAGS)

./objet/%.o: ./src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)



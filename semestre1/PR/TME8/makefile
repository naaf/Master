#			Programmation Repartie (4I400)
#		Makefile pour TM 8. Socket et multi-diffusion


# Documentation: http://www.gnu.org/software/make/manual/make.html
CC =gcc
LFLAGS =
CFLAGS =-W -Wall -ansi -pedantic -Iinclude

DIR=.
BIN=$(DIR)/bin/
OBJ=$(DIR)/obj/
INCLUDE=$(DIR)/include/
LIB=$(DIR)/lib/
SRC=$(DIR)/src/

.SUFFIXES:

HC=
O=

.PHONY: all clean test test-sonar test-mychat
all: $(BIN)sonar $(BIN)mychat

test: test-sonar test-mychat

test-sonar: $(BIN)sonar
	-$$PWD/bin/sonar

test-mychat: $(BIN)mychat
	-$$PWD/bin/mychat 225.0.0.10 2001 $$USER

clean: 
	rm -rf $(OBJ)*.o $(BIN)*

$(BIN)%: $(OBJ)%.o $O
	@if [ -d $(BIN) ]; then : ; else mkdir $(BIN); fi
	$(CC) $(LFLAGS) -o $@ $^

$(OBJ)%.o: $(SRC)%.c $(HC)
	@if [ -d $(OBJ) ]; then : ; else mkdir $(OBJ); fi
	$(CC) $(CFLAGS) -o $@ -c $<

$(INCLUDE)%.h:
	@if [ -d $(INCLUDE) ]; then : ; else mkdir $(INCLUDE); fi


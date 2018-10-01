CC=gcc
CFLAGS= -Wall -std=c11 -Werror
EXEC=automata
OBJECT =  main.c automate.c
all: $(EXEC)

automata: $(OBJECT)
	$(CC) -o $@ $(OBJECT) $(CFLAGS)
clean :
		rm $(EXEC) 

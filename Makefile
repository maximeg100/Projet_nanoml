opt = -Wall -c

projet_nanoml: main.o lexer.o analyseur.o rendu.o
	gcc main.o lexer.o analyseur.o rendu.o -o projet_nanoml

main.o: main.c
	gcc ${opt} main.c

lexer.o: lexer.c
	gcc ${opt} lexer.c

analyseur.o: analyseur.c
	gcc ${opt} analyseur.c

rendu.o: rendu.c
	gcc ${opt} rendu.c

clean:
	rm -f *.o projet_nanoml
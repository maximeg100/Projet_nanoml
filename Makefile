# Nom de ton programme final
projet_nanoml: main.o lexer.o analyseur.o rendu.o
	gcc main.o lexer.o analyseur.o rendu.o -o projet_nanoml

# Compilation de chaque fichier .c en .o
main.o: main.c
	gcc -Wall -c main.c

lexer.o: lexer.c
	gcc -Wall -c lexer.c

analyseur.o: analyseur.c
	gcc -Wall -c analyseur.c

rendu.o: rendu.c
	gcc -Wall -c rendu.c

# Pour tout effacer et recommencer propre
clean:
	rm -f *.o projet_nanoml
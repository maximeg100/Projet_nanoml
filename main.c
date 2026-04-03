#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "analyseur.h"
#include "rendu.h"
#include "nanoml_types.h"

// Rappel : mon_token_courant est une globale dans ton analyseur.c
extern t_token mon_token_courant; 

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fichier.nml>\n", argv[0]);
        return EXIT_FAILURE;
    }
    //ouvre le fichier
    initialiser_lexer(argv[1]);
    //charge le premier token
    avancer(); 
    t_noeud* racine = analyser_texte_enrichi();
    if (racine != NULL) {
        printf("STRUCTURE DE L'ARBRE\n\n");
        afficher_arbre_debug(racine, 0);
        printf("RENDU FINAL\n");
        char prefixe[100] = ""; 
        parcourir_et_afficher(racine, prefixe);
    }

    return 0;
}

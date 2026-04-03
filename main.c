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

    // 2. Initialisation du Lexer (ouvre le fichier)
    initialiser_lexer(argv[1]);

    // 3. Charger le tout premier jeton pour démarrer
    avancer(); 

    // 4. Lancer l'analyse syntaxique (crée l'arbre)
    printf("debut d'analyse\n");
    t_noeud* racine = analyser_texte_enrichi();
    printf("analyse finie avec succes\n\n");

    if (racine != NULL) {
        printf("=== DEBUG : STRUCTURE DE L'ARBRE ===\n");
        afficher_arbre_debug(racine, 0);
        printf("====================================\n\n");

        printf("=== RENDU FINAL ===\n");
        char prefixe[100] = ""; 
        parcourir_et_afficher(racine, prefixe);
    }

    return 0;
}


//a ameliorer
//soucis de nombre de | dans l'affichage a cote de MON PREMIER TEST
//soucis de nombre de | dans document>section
//la phrase ne passe pas a la ligne (soucis dans afficher_mot surement
//enlever les espaces entre (troisieme ligne de doc et debut section / fin de section et fin de doc)
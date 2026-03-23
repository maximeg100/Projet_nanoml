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
    printf("--- Analyse en cours ---\n");
    t_noeud* racine = analyser_texte_enrichi();
    printf("--- Analyse terminee avec succes ---\n\n");

    // 5. Lancer le rendu graphique
    printf("--- Rendu Final (50 colonnes) ---\n");
    // On commence avec un préfixe vide ""
    parcourir_et_afficher(racine, ""); 
    printf("\n--- Fin du programme ---\n");

    return EXIT_SUCCESS;
}


//a ameliorer
//soucis de nombre de | dans l'affichage a cote de MON PREMIER TEST
//soucis de nombre de | dans document>section
//la phrase ne passe pas a la ligne (soucis dans afficher_mot surement
//enlever les espaces entre (troisieme ligne de doc et debut section / fin de section et fin de doc)
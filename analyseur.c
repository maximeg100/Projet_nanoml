#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analyseur.h"
#include "lexer.h"

t_token mon_token_courant;

// Utilitaire pour avancer dans le lexer
void avancer() {
    mon_token_courant = mon_token_suivant();
}

// Fonction pour vérifier et consommer une balise précise
void consommer_balise(t_type_token type_attendu, const char* nom_attendu) {
    // 1. On vérifie si le type (ex: OUVRANTE) et le nom (ex: "section") correspondent
    if (mon_token_courant.mon_type == type_attendu && strcmp(mon_token_courant.ma_valeur, nom_attendu) == 0) {
        // Si c'est bon, on passe au mot suivant dans le fichier
        avancer();
    } else {
        // Sinon, on affiche une erreur et on arrête tout (Erreur Syntaxique)
        fprintf(stderr, "Erreur : attendu '%s', trouvé '%s'\n", nom_attendu, mon_token_courant.ma_valeur);
        exit(EXIT_FAILURE);
    }
}

t_noeud* creer_noeud(t_type_noeud type) {
    t_noeud* nouveau = malloc(sizeof(t_noeud));
    if (nouveau == NULL){
        fprintf(stderr, "erreur d'allocation memoire");
        exit(EXIT_FAILURE);
    }
    nouveau->mon_type = type;
    nouveau->mon_contenu = NULL;
    nouveau->mon_premier_fils = NULL;
    nouveau->mon_frere_suivant = NULL;

    return nouveau;

}

// --- Implémentation de la grammaire ---

t_noeud* analyser_document() {
}

t_noeud* analyser_contenu() {
    // Ici, tu dois faire une boucle qui teste le TOKEN_COURANT
    // Si c'est <section>, appeler analyser_section()
    // Si c'est <titre>, appeler analyser_titre() 
    // ... et attacher les résultats via 'mon_frere_suivant'
    return NULL; 
}

// À toi de compléter les autres fonctions (analyser_section, analyser_liste, etc.)
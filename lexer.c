#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// Variables globales pour la lecture du fichier
FILE* mon_fichier_source;
char mon_caractere_actuel;


// Vérifie si un caractère est un espace, une tabulation ou un retour à la ligne
int est_espace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Vide une chaîne de caractères manuellement
void vider_chaine(char* s, int taille) {
    for (int i = 0; i < taille; i++) {
        s[i] = '\0';
    }
}
/**
 * Ouvre le fichier et prépare la lecture du premier caractère.
 * @param nom_fichier Le chemin vers le fichier .nanoml
 */
void initialiser_lexer(char* nom_fichier) {
    // 1. Ouvrir le fichier en mode lecture ("r")
    mon_fichier_source = fopen(nom_fichier, "r" );
    // 2. Vérifier si l'ouverture a réussi (si NULL -> erreur)
    if (mon_fichier_source == NULL){
        fprintf(stderr, "erreur d'ouverture du fichier '%s'", nom_fichier);
        exit(EXIT_FAILURE);
    }
    // 3. Lire le tout premier caractère du fichier pour amorcer la lecture
    mon_caractere_actuel = fgetc(mon_fichier_source);

}

/**
 * Ferme proprement le fichier source.
 */
void terminer_lexer() {
    // 1. Fermer le pointeur mon_fichier_source
    int returnCode = fclose(mon_fichier_source);
    if ( returnCode == EOF ) {
        fprintf( stderr, "Erreur durant la fermeture du fichier" );
        exit(EXIT_FAILURE);
    }
}

/**
 * Le "Cœur" du Lexer : analyse le texte et retourne le prochain jeton (Token).
 * Cette fonction distingue les balises <...> du texte brut.
 * @return Un t_token rempli (type et valeur)
 */
t_token mon_token_suivant() {
    t_token mon_jeton;
    int i = 0;

    // On vide la zone de texte du jeton
    vider_chaine(mon_jeton.ma_valeur, 256);

    // 1. On saute les espaces et retours à la ligne du fichier source
    while (mon_caractere_actuel != EOF && est_espace(mon_caractere_actuel)) {
        mon_caractere_actuel = fgetc(mon_fichier_source);
    }

    // 2. Si on arrive au bout du fichier
    if (mon_caractere_actuel == EOF) {
        mon_jeton.mon_type = TOKEN_FIN_FICHIER;
        return mon_jeton;
    }

    // 3. CAS A : On rencontre une balise
    if (mon_caractere_actuel == '<') {
        mon_caractere_actuel = fgetc(mon_fichier_source); // On avance après le '<'

        if (mon_caractere_actuel == '/') {
            // C'est une fermeture </...
            mon_jeton.mon_type = TOKEN_BALISE_FERMANTE;
            mon_caractere_actuel = fgetc(mon_fichier_source); // On avance après le '/'
        } else {
            // C'est une ouverture <...
            mon_jeton.mon_type = TOKEN_BALISE_OUVRANTE;
        }

        // On lit le nom de la balise (ex: "section") tant qu'on ne voit pas '>' ou '/' [cite: 155]
        while (mon_caractere_actuel != '>' && mon_caractere_actuel != '/' && mon_caractere_actuel != EOF) {
            mon_jeton.ma_valeur[i] = mon_caractere_actuel;
            i++;
            mon_caractere_actuel = fgetc(mon_fichier_source);
        }

        // Cas spécial du <br/> 
        if (mon_caractere_actuel == '/') {
            mon_jeton.mon_type = TOKEN_BALISE_AUTO_FERMANTE;
            mon_caractere_actuel = fgetc(mon_fichier_source); // On saute le '/'
        }

        // On termine en sautant le '>' final
        if (mon_caractere_actuel == '>') {
            mon_caractere_actuel = fgetc(mon_fichier_source);
        }
    } 
    // 4. CAS B : On rencontre du texte
    else {
        mon_jeton.mon_type = TOKEN_TEXTE;
        // On lit tout jusqu'au prochain '<' 
        while (mon_caractere_actuel != '<' && mon_caractere_actuel != EOF) {
            mon_jeton.ma_valeur[i] = mon_caractere_actuel;
            i++;
            mon_caractere_actuel = fgetc(mon_fichier_source);
        }
    }

    return mon_jeton;
}
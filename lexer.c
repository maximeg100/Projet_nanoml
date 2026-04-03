#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

FILE* mon_fichier_source;
char mon_caractere_actuel;

int est_espace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

void vider_chaine(char* s, int taille) {
    for (int i = 0; i < taille; i++) {
        s[i] = '\0';
    }
}

void initialiser_lexer(char* nom_fichier) {
    mon_fichier_source = fopen(nom_fichier, "r");
    if (mon_fichier_source == NULL) {
        fprintf(stderr, "erreur d'ouverture du fichier '%s'", nom_fichier);
        exit(EXIT_FAILURE);
    }
    mon_caractere_actuel = fgetc(mon_fichier_source);
}

void terminer_lexer() {
    if (fclose(mon_fichier_source) == EOF) {
        fprintf(stderr, "Erreur durant la fermeture du fichier");
        exit(EXIT_FAILURE);
    }
}

t_token mon_token_suivant() {
    t_token mon_jeton;
    int i = 0;
    vider_chaine(mon_jeton.ma_valeur, 256);
    //saute les espaces
    while (mon_caractere_actuel != EOF && est_espace(mon_caractere_actuel)) {
        mon_caractere_actuel = fgetc(mon_fichier_source);
    }
    if (mon_caractere_actuel == EOF) {
        mon_jeton.mon_type = TOKEN_FIN_FICHIER;
        return mon_jeton;
    }
    if (mon_caractere_actuel == '<') {
        mon_caractere_actuel = fgetc(mon_fichier_source);
        if (mon_caractere_actuel == '/') {
            mon_jeton.mon_type = TOKEN_BALISE_FERMANTE;
            mon_caractere_actuel = fgetc(mon_fichier_source);
        } else {
            mon_jeton.mon_type = TOKEN_BALISE_OUVRANTE;
        }
        while (mon_caractere_actuel != '>' && mon_caractere_actuel != '/' && mon_caractere_actuel != EOF) {
            mon_jeton.ma_valeur[i++] = mon_caractere_actuel;
            mon_caractere_actuel = fgetc(mon_fichier_source);
        }
        if (mon_caractere_actuel == '/') {
            mon_jeton.mon_type = TOKEN_BALISE_AUTO_FERMANTE;
            mon_caractere_actuel = fgetc(mon_fichier_source);
        }
        if (mon_caractere_actuel == '>') {
            mon_caractere_actuel = fgetc(mon_fichier_source);
        }
    } 
    else {
        mon_jeton.mon_type = TOKEN_TEXTE;
        while (mon_caractere_actuel != '<' && !est_espace(mon_caractere_actuel) && mon_caractere_actuel != EOF) {
            mon_jeton.ma_valeur[i++] = mon_caractere_actuel;
            mon_caractere_actuel = fgetc(mon_fichier_source);
        }
    }

    return mon_jeton;
}
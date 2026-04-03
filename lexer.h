#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

typedef enum {
    TOKEN_BALISE_OUVRANTE,
    TOKEN_BALISE_FERMANTE,
    TOKEN_BALISE_AUTO_FERMANTE,
    TOKEN_TEXTE,
    TOKEN_FIN_FICHIER,
    TOKEN_ERREUR
} t_type_token;

typedef struct {
    t_type_token mon_type;
    char ma_valeur[256]; // Stocke "section", "titre" ou le texte brut
} t_token;


void initialiser_lexer(char* nom_fichier);
t_token mon_token_suivant();
void terminer_lexer();

#endif
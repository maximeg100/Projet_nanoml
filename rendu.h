#ifndef RENDU_H
#define RENDU_H

#include "analyseur.h"

// Variable globale pour suivre la colonne actuelle (de 0 à 50)
extern int colonne_actuelle;

void afficher_bordure_horizontale(const char* prefixe);
void terminer_ligne(const char* prefixe);
void afficher_mot(const char* mot, const char* prefixe);
void transformer_en_majuscules(char* texte);
void parcourir_et_afficher(t_noeud* n, char* prefixe);
void afficher_arbre_debug(t_noeud* n, int niveau);

#endif
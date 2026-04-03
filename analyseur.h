#ifndef ANALYSEUR_H
#define ANALYSEUR_H

#include "nanoml_types.h"
#include "lexer.h"

t_noeud* analyser_texte_enrichi();
t_noeud* analyser_document();
t_noeud* analyser_annexes();
t_noeud* analyser_contenu();
t_noeud* analyser_section();
t_noeud* analyser_titre();
t_noeud* analyser_liste();
t_noeud* analyser_item();
t_noeud* analyser_liste_texte();
t_noeud* analyser_texte_liste();
t_noeud* analyser_texte();
t_noeud* analyser_mot_enrichi();
t_noeud* analyser_mot_important();
void avancer();
void consommer_balise(t_type_token type_attendu, const char* nom_attendu);
t_noeud* creer_noeud(t_type_noeud type);
int est_du_texte();

#endif
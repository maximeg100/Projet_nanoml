#include "nanoml_types.h"

// Fonctions de gestion de l'arbre
t_noeud* creer_noeud(t_type_noeud type);
void ajouter_fils(t_noeud* parent, t_noeud* nouveau_fils);

// Fonctions d'analyse syntaxique (basées sur la grammaire [cite: 34])
t_noeud* analyser_texte_enrichi(); // <texte enrichi> ::= <document> <annexes>
t_noeud* analyser_document();       // <document> ::= '<document>' <contenu> '</document>'
t_noeud* analyser_annexes();        // <annexes> ::= { '<annexe>' <contenu> </annexe>' }
t_noeud* analyser_contenu();        // { <section> | <titre> | <mot enrichi> | <liste> }
t_noeud* analyser_section();        // '<section>' <contenu> </section>'
t_noeud* analyser_titre();          // '<titre>' <texte> </titre>'
t_noeud* analyser_liste();          // '<liste>' { <item> } </liste>'
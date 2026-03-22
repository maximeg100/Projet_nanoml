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

t_noeud* analyser_texte_enrichi() {
    t_noeud* doc = analyser_document();
    doc->mon_frere_suivant = analyser_annexes();
    return doc;
}


t_noeud* analyser_document() {
    consommer_balise(TOKEN_BALISE_OUVRANTE, "document");
    t_noeud* mon_noeud_doc = creer_noeud(TYPE_DOCUMENT);
    mon_noeud_doc->mon_premier_fils = analyser_contenu();
    consommer_balise(TOKEN_BALISE_FERMANTE, "document");
    return mon_noeud_doc;
}


t_noeud* analyser_annexe() {
    t_noeud* premier_fils = NULL;
    t_noeud* dernier_ajoute = NULL;
    while (mon_token_courant.mon_type == TOKEN_BALISE_OUVRANTE && strcmp(mon_token_courant.ma_valeur, "annexe") == 0) {
        consommer_balise(TOKEN_BALISE_OUVRANTE, "annexe");
        t_noeud* mon_noeud_anx = creer_noeud(TYPE_ANNEXE);
        mon_noeud_anx->mon_premier_fils = analyser_contenu();
        consommer_balise(TOKEN_BALISE_FERMANTE, "annexe");
        if (premier_fils == NULL) {
            premier_fils = mon_noeud_anx;
        } else {
            dernier_ajoute->mon_frere_suivant = mon_noeud_anx;
        }
        dernier_ajoute = mon_noeud_anx;
    }
    return premier_fils;
}

t_noeud* analyser_contenu() {
    t_noeud* premier_fils = NULL;
    t_noeud* dernier_ajoute = NULL;
    while (mon_token_courant.mon_type != TOKEN_BALISE_FERMANTE && mon_token_courant.mon_type != TOKEN_FIN_FICHIER) {
        t_noeud* nouveau_noeud = NULL;
        if (mon_token_courant.mon_type == TOKEN_BALISE_OUVRANTE) {
            if (strcmp(mon_token_courant.ma_valeur, "section") == 0) {
                nouveau_noeud = analyser_section();
            } 
            else if (strcmp(mon_token_courant.ma_valeur, "titre") == 0) {
                nouveau_noeud = analyser_titre();
            } 
            else if (strcmp(mon_token_courant.ma_valeur, "liste") == 0) {
                nouveau_noeud = analyser_liste();
            }
            else {
                nouveau_noeud = analyser_mot_enrichi();
            }
        } 
        else {
            nouveau_noeud = analyser_mot_enrichi();
        }
        // Chaînage des frères pour l'arbre n-aire
        if (nouveau_noeud != NULL) {
            if (premier_fils == NULL) {
                premier_fils = nouveau_noeud;
            } else {
                dernier_ajoute->mon_frere_suivant = nouveau_noeud;
            }
            dernier_ajoute = nouveau_noeud;
        }
    }
    return premier_fils;
}

t_noeud* analyser_section(){
    consommer_balise(TOKEN_BALISE_OUVRANTE, "section");
    t_noeud* mon_noeud_sec = creer_noeud(TYPE_SECTION);
    mon_noeud_sec->mon_premier_fils = analyser_contenu();
    consommer_balise(TOKEN_BALISE_FERMANTE, "section");
    return mon_noeud_sec;
}

t_noeud* analyser_titre(){
    consommer_balise(TOKEN_BALISE_OUVRANTE, "titre");
    t_noeud* mon_noeud_tit = creer_noeud(TYPE_TITRE);
    mon_noeud_tit->mon_premier_fils = analyser_texte();
    consommer_balise(TOKEN_BALISE_FERMANTE, "titre");
    return mon_noeud_tit;
}


t_noeud* analyser_liste() {
    consommer_balise(TOKEN_BALISE_OUVRANTE, "liste");
    t_noeud* mon_noeud_lst = creer_noeud(TYPE_LISTE);
    t_noeud* premier_item = NULL;
    t_noeud* dernier_item = NULL;
    while (mon_token_courant.mon_type == TOKEN_BALISE_OUVRANTE && strcmp(mon_token_courant.ma_valeur, "item") == 0) {
        t_noeud* nouvel_item = analyser_item();
        if (premier_item == NULL) {
            premier_item = nouvel_item;
            mon_noeud_lst->mon_premier_fils = premier_item;
        } else {
            dernier_item->mon_frere_suivant = nouvel_item;
        }
        dernier_item = nouvel_item;
    }
    consommer_balise(TOKEN_BALISE_FERMANTE, "liste");
    return mon_noeud_lst;
}

// Fonction utilitaire pour savoir si c'est du texte
int est_du_texte() {
    // 1. Texte brut
    if (mon_token_courant.mon_type == TOKEN_TEXTE) {
        return 1;
    }
    // 2. Balise <important>
    if (mon_token_courant.mon_type == TOKEN_BALISE_OUVRANTE && 
        strcmp(mon_token_courant.ma_valeur, "important") == 0) {
        return 1;
    }
    // 3. Balise <br/>
    if (mon_token_courant.mon_type == TOKEN_BALISE_AUTO_FERMANTE && 
        strcmp(mon_token_courant.ma_valeur, "br") == 0) {
        return 1;
    }
    return 0; // Ce n'est pas du texte
}

t_noeud* analyser_item() {
    consommer_balise(TOKEN_BALISE_OUVRANTE, "item");
    t_noeud* mon_noeud_item = creer_noeud(TYPE_ITEM);
    t_noeud* premier_bloc = NULL;
    t_noeud* deuxieme_bloc = NULL;
    // On regarde le premier jeton pour choisir la règle (9 ou 10)
    if (mon_token_courant.mon_type == TOKEN_BALISE_OUVRANTE && strcmp(mon_token_courant.ma_valeur, "liste") == 0) {
        // CAS : <liste_texte>
        premier_bloc = analyser_liste();
        if (est_du_texte()) {
            deuxieme_bloc = analyser_texte();
        }
    } 
    else if (est_du_texte()) {
        // CAS : <texte_liste>
        premier_bloc = analyser_texte();
        if (mon_token_courant.mon_type == TOKEN_BALISE_OUVRANTE && strcmp(mon_token_courant.ma_valeur, "liste") == 0) {
            deuxieme_bloc = analyser_liste();
        }
    }
    // On branche les morceaux sous l'item
    mon_noeud_item->mon_premier_fils = premier_bloc;
    if (premier_bloc != NULL) {
        premier_bloc->mon_frere_suivant = deuxieme_bloc;
    }
    consommer_balise(TOKEN_BALISE_FERMANTE, "item");
    return mon_noeud_item;
}

t_noeud* analyser_texte() {
    t_noeud* premier_mot = NULL;
    t_noeud* dernier_mot = NULL;
    while (est_du_texte()) {
        t_noeud* nouveau_mot = analyser_mot_enrichi();
        if (premier_mot == NULL) {
            premier_mot = nouveau_mot;
        } else {
            dernier_mot->mon_frere_suivant = nouveau_mot;
        }
        dernier_mot = nouveau_mot;
    }
    return premier_mot;
}



//autres fonctions 
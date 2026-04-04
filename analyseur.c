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

void consommer_balise(t_type_token type_attendu, const char* nom_attendu) {
    if (mon_token_courant.mon_type == type_attendu && 
        strcmp(mon_token_courant.ma_valeur, nom_attendu) == 0) {
        avancer();
    } else {
        char format_recu[300];
        char format_attendu[300];
        if (type_attendu == TOKEN_BALISE_FERMANTE) {
            sprintf(format_attendu, "</%s>", nom_attendu);
        } else {
            sprintf(format_attendu, "<%s>", nom_attendu);
        }

        // permet de formater la balise vraiment recue 
        // au lieu de Erreur syntaxique : attendu document, reçu document
        //on recoit Erreur syntaxique : attendu </document>, reçu <document>
        if (mon_token_courant.mon_type == TOKEN_BALISE_FERMANTE) {
            sprintf(format_recu, "</%s>", mon_token_courant.ma_valeur);
        } else if (mon_token_courant.mon_type == TOKEN_BALISE_OUVRANTE) {
            sprintf(format_recu, "<%s>", mon_token_courant.ma_valeur);
        } else if (mon_token_courant.mon_type == TOKEN_TEXTE) {
            sprintf(format_recu, "TEXTE('%s')", mon_token_courant.ma_valeur);
        } else {
            sprintf(format_recu, "FIN DE FICHIER");
        }

        fprintf(stderr, "Erreur syntaxique : attendu '%s', reçu '%s'\n", 
                format_attendu, format_recu);
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

void liberer_arbre(t_noeud* n) {
    if (n == NULL) return;
    liberer_arbre(n->mon_premier_fils);
    liberer_arbre(n->mon_frere_suivant);
    if (n->mon_contenu != NULL) {
        free(n->mon_contenu);
    }
    free(n);
}

//implementation de la grammaire

t_noeud* analyser_texte_enrichi() {
    t_noeud* racine = creer_noeud(TYPE_DOCUMENT_GLOBAL);
    racine->mon_premier_fils = analyser_document();
    racine->mon_premier_fils->mon_frere_suivant = analyser_annexes();
    if (mon_token_courant.mon_type != TOKEN_FIN_FICHIER) {
        fprintf(stderr, "Erreur Syntaxique : Texte en trop après la fin du document (trouvé '%s')\n", mon_token_courant.ma_valeur);
        exit(EXIT_FAILURE);
    }
    return racine;
}


t_noeud* analyser_document() {
    consommer_balise(TOKEN_BALISE_OUVRANTE, "document");
    t_noeud* mon_noeud_doc = creer_noeud(TYPE_DOCUMENT);
    mon_noeud_doc->mon_premier_fils = analyser_contenu();
    consommer_balise(TOKEN_BALISE_FERMANTE, "document");
    return mon_noeud_doc;
}


t_noeud* analyser_annexes() {
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

    while (mon_token_courant.mon_type != TOKEN_BALISE_FERMANTE && 
           mon_token_courant.mon_type != TOKEN_FIN_FICHIER) {
        
        t_noeud* nouveau_noeud = NULL;

        if (mon_token_courant.mon_type == TOKEN_BALISE_OUVRANTE) {
            if (strcmp(mon_token_courant.ma_valeur, "section") == 0) 
                nouveau_noeud = analyser_section();
            else if (strcmp(mon_token_courant.ma_valeur, "titre") == 0) 
                nouveau_noeud = analyser_titre();
            else if (strcmp(mon_token_courant.ma_valeur, "liste") == 0) 
                nouveau_noeud = analyser_liste();
            else if (strcmp(mon_token_courant.ma_valeur, "important") == 0)
                nouveau_noeud = analyser_texte();
            else 
                nouveau_noeud = analyser_texte();
        } 
        else if (mon_token_courant.mon_type == TOKEN_TEXTE || 
                 mon_token_courant.mon_type == TOKEN_BALISE_AUTO_FERMANTE) {
            nouveau_noeud = analyser_texte();
        }
        // chainage des freres
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

// fonction utilitaire pour savoir si c'est du texte
int est_du_texte() {
    // texte
    if (mon_token_courant.mon_type == TOKEN_TEXTE) {
        return 1;
    }
    // <important>
    if (mon_token_courant.mon_type == TOKEN_BALISE_OUVRANTE && strcmp(mon_token_courant.ma_valeur, "important") == 0) {
        return 1;
    }
    // <br>
    if (mon_token_courant.mon_type == TOKEN_BALISE_AUTO_FERMANTE && strcmp(mon_token_courant.ma_valeur, "br") == 0) {
        return 1;
    }
    return 0; // Ce n'est pas du texte
}

t_noeud* analyser_item() {
    consommer_balise(TOKEN_BALISE_OUVRANTE, "item");
    t_noeud* mon_noeud_item = creer_noeud(TYPE_ITEM);
    t_noeud* contenu_item = NULL;
    //on regarde le prmeier token pour voir quelle regle on applique
    if (mon_token_courant.mon_type == TOKEN_BALISE_OUVRANTE && strcmp(mon_token_courant.ma_valeur, "liste") == 0) {
        contenu_item = analyser_liste_texte();
    } 
    else {
        //si c'est du texte
        contenu_item = analyser_texte_liste();
    }
    // On branche le résultat sous l'item
    mon_noeud_item->mon_premier_fils = contenu_item;
    consommer_balise(TOKEN_BALISE_FERMANTE, "item");
    return mon_noeud_item;
}

t_noeud* analyser_liste_texte() {
    //analyse liste
    t_noeud* n_liste = analyser_liste();
    //si il y a du texte ensuite on l'attache 
    if (est_du_texte()) {
        n_liste->mon_frere_suivant = analyser_texte_liste();
    }
    return n_liste;
}

t_noeud* analyser_texte_liste() {
    //analyse texte
    t_noeud* n_texte = analyser_texte();
    //si il y a une liste ensuite on l'attache
    if (mon_token_courant.mon_type == TOKEN_BALISE_OUVRANTE && 
        strcmp(mon_token_courant.ma_valeur, "liste") == 0) {
        n_texte->mon_frere_suivant = analyser_liste_texte();
    }
    return n_texte;
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

char* copier_chaine(const char* source) {
    int longueur = 0;
    while (source[longueur] != '\0') {
        longueur++;
    }
    char* destination = malloc((longueur + 1) * sizeof(char));
    if (destination == NULL) {
        fprintf(stderr, "Erreur : échec allocation mémoire chaîne\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i <= longueur; i++) {
        destination[i] = source[i];
    }

    return destination;
}

t_noeud* analyser_mot_enrichi() {
    t_noeud* n = NULL;
    // <mot_important>
        if (mon_token_courant.mon_type == TOKEN_BALISE_OUVRANTE && strcmp(mon_token_courant.ma_valeur, "important") == 0) {
        n = analyser_mot_important();
    } 
    // '<br/>'
    else if (mon_token_courant.mon_type == TOKEN_BALISE_AUTO_FERMANTE && strcmp(mon_token_courant.ma_valeur, "br") == 0) {
        n = creer_noeud(TYPE_BR);
        n->mon_contenu = copier_chaine("br"); 
        avancer(); 
    } 
    // <mot_simple>
    else if (mon_token_courant.mon_type == TOKEN_TEXTE) {
        n = creer_noeud(TYPE_TEXTE_BRUT);
        n->mon_contenu = copier_chaine(mon_token_courant.ma_valeur);
        avancer();
    }
    return n;
}

t_noeud* analyser_mot_important() {
    consommer_balise(TOKEN_BALISE_OUVRANTE, "important");
    t_noeud* n = creer_noeud(TYPE_IMPORTANT);
    n->mon_premier_fils = analyser_texte();
    consommer_balise(TOKEN_BALISE_FERMANTE, "important");
    return n;
}

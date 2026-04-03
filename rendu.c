#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nanoml_types.h"

// pour suivre la colonne actuelle et ne pas depasser 50
int colonne_actuelle = 0;


void terminer_ligne(const char* prefixe) {
    // ligne deja vide
    if (colonne_actuelle == 0){
        return; 
    }
    int nb_pipes = strlen(prefixe);
    int borne_droite = 50 - nb_pipes;
    while (colonne_actuelle < borne_droite) {
        printf(" ");
        colonne_actuelle++;
    }
    for (int i = 0; i < nb_pipes; i++) printf("|");
    printf("\n");
    colonne_actuelle = 0;
}

void afficher_bordure_horizontale(const char* prefixe) {
    //si on ecrivait du texte on finit la ligne 
    if (colonne_actuelle != 0) {
        terminer_ligne(prefixe);
    }
    printf("%s", prefixe);
    printf("+");
    int nb_parents = strlen(prefixe);
    int tirets = 50 - (2 * nb_parents) - 2;

    for (int i = 0; i < tirets; i++) {
        printf("-");
    }
    printf("+");
    for (int i = 0; i < nb_parents; i++) {
        if (prefixe[i] == '|') printf("|");
    }
    printf("\n");
    colonne_actuelle = 0;
}

void transformer_en_majuscules(t_noeud* n) {
    if (n == NULL) return;
    // on transforme le texte du noeud actuel
    if (n->mon_contenu != NULL) {
        for (int i = 0; n->mon_contenu[i] != '\0'; i++) {
            if (n->mon_contenu[i] >= 'a' && n->mon_contenu[i] <= 'z') {
                n->mon_contenu[i] -= 32;
            }
        }
    }
    // fils
    transformer_en_majuscules(n->mon_premier_fils);
    // frere
    transformer_en_majuscules(n->mon_frere_suivant);
}


// Vérifie si l'octet commence par les bits "10"
int est_octet_suite(unsigned char c) {
    // 0xC0 correspond à 11000000 en binaire (le masque)
    // 0x80 correspond à 10000000 en binaire (le motif attendu)
    return (c & 0xC0) == 0x80;
}

void afficher_mot(const char* mot, const char* prefixe) {
    int nb_pipes = strlen(prefixe); 
    int borne_droite = 50 - nb_pipes;
    // debut de ligne
    if (colonne_actuelle == 0) {
        printf("%s", prefixe);
        colonne_actuelle = nb_pipes;
    }
    // affichage et decoupe si necessaire
    for (int i = 0; mot[i] != '\0'; i++) {
        // on change de ligne si on allait depasser
        if (colonne_actuelle >= borne_droite) {
            terminer_ligne(prefixe);
            printf("%s", prefixe);
            colonne_actuelle = nb_pipes;
        }
        printf("%c", mot[i]);
        // on n'incrémente la colonne que pour les caractères "visuels"
        if (!est_octet_suite((unsigned char)mot[i])) {
            colonne_actuelle++;
        }
    }
    // espace apres le mot
    if (colonne_actuelle < borne_droite) {
        printf(" ");
        colonne_actuelle++;
    } else {
        terminer_ligne(prefixe);
    }
}


void parcourir_et_afficher(t_noeud* n, char* prefixe) {
    if (n == NULL){
        return;
    }
    switch (n->mon_type) {
        case TYPE_DOCUMENT_GLOBAL:
            parcourir_et_afficher(n->mon_premier_fils, prefixe);
            break;
        case TYPE_SECTION:
        case TYPE_ANNEXE:
        case TYPE_DOCUMENT: {
            // bordure du haut
            afficher_bordure_horizontale(prefixe);
            // apres une bordure, on part d'une ligne propre
            colonne_actuelle = 0; 
            char nouveau_prefixe[100];
            sprintf(nouveau_prefixe, "%s|", prefixe);
            // affiche le contenu
            parcourir_et_afficher(n->mon_premier_fils, nouveau_prefixe);
            // on ferme la dernière ligne de texte si besoin
            terminer_ligne(nouveau_prefixe); 
            // bordure du bas
            afficher_bordure_horizontale(prefixe);
            colonne_actuelle = 0; 
            break;
        }
        case TYPE_TITRE:
            transformer_en_majuscules(n->mon_premier_fils);
            parcourir_et_afficher(n->mon_premier_fils, prefixe);
            terminer_ligne(prefixe);
            break;
        case TYPE_LISTE:
            parcourir_et_afficher(n->mon_premier_fils, prefixe);
            break;
        case TYPE_ITEM:
            terminer_ligne(prefixe);
            afficher_mot("  #", prefixe); 
            parcourir_et_afficher(n->mon_premier_fils, prefixe);
            terminer_ligne(prefixe);
            break;
        case TYPE_IMPORTANT:
            afficher_mot("*", prefixe);
            parcourir_et_afficher(n->mon_premier_fils, prefixe);
            afficher_mot("*", prefixe);
            break;
        case TYPE_TEXTE_BRUT:
            if (n->mon_contenu) afficher_mot(n->mon_contenu, prefixe);
            break;
        case TYPE_BR:
            terminer_ligne(prefixe);
            break;
    }
    parcourir_et_afficher(n->mon_frere_suivant, prefixe);
}
//permet de traduire pour une meilleure vision de l'arbre
const char* nom_du_type(t_type_noeud type) {
    switch (type) {
        case TYPE_DOCUMENT_GLOBAL: return "GLOBAL";
        case TYPE_DOCUMENT:        return "DOC";
        case TYPE_ANNEXE:          return "ANNEXE";
        case TYPE_SECTION:         return "SECTION";
        case TYPE_TITRE:           return "TITRE";
        case TYPE_LISTE:           return "LISTE";
        case TYPE_ITEM:            return "ITEM";
        case TYPE_IMPORTANT:       return "IMPORTANT";
        case TYPE_BR:              return "BR";
        case TYPE_TEXTE_BRUT:      return "TEXTE";
        default:                   return "INCONNU";
    }
}

void afficher_arbre_debug(t_noeud* n, int niveau) {
    if (n == NULL) return;
    for (int i = 0; i < niveau; i++) printf("  ");
    printf("[%s]", nom_du_type(n->mon_type));
    if (n->mon_contenu != NULL) {
        printf(" : \"%s\"", n->mon_contenu);
    }
    printf("\n");
    if (n->mon_premier_fils != NULL) {
        afficher_arbre_debug(n->mon_premier_fils, niveau + 1);
    }
    if (n->mon_frere_suivant != NULL) {
        afficher_arbre_debug(n->mon_frere_suivant, niveau);
    }
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nanoml_types.h"

// Variable globale ou passée en paramètre pour suivre la colonne actuelle
int colonne_actuelle = 0;

void afficher_bordure_horizontale(const char* prefixe) {
    int nb_parents = strlen(prefixe);
    
    // 1. On affiche les pipes des parents à gauche
    printf("%s", prefixe);
    
    // 2. On affiche le coin gauche
    printf("+");
    
    // 3. Calcul des tirets : 
    // Total(50) - parents_gauche - parents_droite - 2 coins
    // Comme parents_gauche == parents_droite == nb_parents :
    int tirets = 50 - (2 * nb_parents) - 2;
    
    for (int i = 0; i < tirets; i++) {
        printf("-");
    }
    
    // 4. On affiche le coin droit
    printf("+");
    
    // 5. On affiche les pipes des parents à droite pour fermer
    for (int i = 0; i < nb_parents; i++) {
        printf("|");
    }
    printf("\n");
}

void transformer_en_majuscules(char* texte) {
    if (texte == NULL) return;
    for (int i = 0; texte[i] != '\0'; i++) {
        if ( 97 <= texte[i] && texte[i] <= 122 ) {
            texte[i] -= 32;
        }
    }
}

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

// Vérifie si l'octet commence par les bits "10"
int est_octet_suite(unsigned char c) {
    // 0xC0 correspond à 11000000 en binaire (le masque)
    // 0x80 correspond à 10000000 en binaire (le motif attendu)
    return (c & 0xC0) == 0x80;
}

void afficher_mot(const char* mot, const char* prefixe) {
    int nb_pipes = strlen(prefixe); 
    int borne_droite = 50 - nb_pipes;

    // 1. Initialisation du début de ligne
    if (colonne_actuelle == 0) {
        printf("%s", prefixe);
        colonne_actuelle = nb_pipes;
    }

    // 2. Affichage et découpe
    for (int i = 0; mot[i] != '\0'; i++) {
        // Si on atteint la bordure de droite, on change de ligne
        if (colonne_actuelle >= borne_droite) {
            terminer_ligne(prefixe);
            printf("%s", prefixe);
            colonne_actuelle = nb_pipes;
        }

        printf("%c", mot[i]);

        // On n'incrémente la colonne que pour les caractères "visuels"
        if (!est_octet_suite((unsigned char)mot[i])) {
            colonne_actuelle++;
        }
    }

    // 3. Espace après le mot
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
        case TYPE_DOCUMENT:
        case TYPE_SECTION:
        case TYPE_ANNEXE: {
            afficher_bordure_horizontale(prefixe);
            char nouveau_prefixe[100];
            sprintf(nouveau_prefixe, "%s|", prefixe);
            parcourir_et_afficher(n->mon_premier_fils, nouveau_prefixe);
            terminer_ligne(nouveau_prefixe); 
            afficher_bordure_horizontale(prefixe);
            break;
        }
        case TYPE_TITRE:
            t_noeud* curseur = n->mon_premier_fils;
            while(curseur != NULL) {
                if (curseur->mon_contenu) transformer_en_majuscules(curseur->mon_contenu);
                curseur = curseur->mon_frere_suivant;
            }
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
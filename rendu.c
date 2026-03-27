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

void afficher_mot(const char* mot, const char* prefixe) {
    int longueur = strlen(mot);
    int nb_pipes = strlen(prefixe); 
    
    // La colonne où commencent les pipes de fermeture (ex: 48 si 2 pipes)
    int borne_droite = 50 - nb_pipes;

    // 1. Si on est en début de ligne, on affiche le préfixe
    if (colonne_actuelle == 0) {
        printf("%s", prefixe);
        colonne_actuelle = nb_pipes;
    }

    // 2. CONDITION DE SAUT DE LIGNE :
    // Si le mot + un espace dépasse la borne_droite, on ferme et on saute
    if (colonne_actuelle + longueur >= borne_droite) {
        
        // On remplit d'espaces jusqu'à la borne_droite
        while (colonne_actuelle < borne_droite) {
            printf(" ");
            colonne_actuelle++;
        }
        
        // On affiche les pipes de fermeture (ex: ||)
        for (int i = 0; i < nb_pipes; i++) printf("|");
        
        // Retour à la ligne et ré-affichage du préfixe
        printf("\n%s", prefixe);
        colonne_actuelle = nb_pipes;
    }

    // 3. On affiche le mot
    printf("%s", mot);
    colonne_actuelle += longueur;

    // 4. On ajoute un espace si le PROCHAIN mot a une chance de tenir
    // (On ne met pas d'espace si on est déjà au bord)
    if (colonne_actuelle < borne_droite - 1) {
        printf(" ");
        colonne_actuelle++;
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
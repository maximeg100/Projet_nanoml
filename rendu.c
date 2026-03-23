#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nanoml_types.h"

// Variable globale ou passée en paramètre pour suivre la colonne actuelle
int colonne_actuelle = 0;

void afficher_bordure_horizontale(const char* prefixe) {
    int nb_parents = strlen(prefixe);
    printf("%s", prefixe);
    printf("+");
    int tirets = 50 - (2 * nb_parents) - 2;
    for (int i = 0; i < tirets; i++) {
        printf("-");
    }
    printf("+");
    for (int i = 0; i < nb_parents; i++) {
        printf("|");
    }
    printf("\n");
}

void terminer_ligne(const char* prefixe) {
    int nb_boites = strlen(prefixe);
    if (colonne_actuelle == 0) {
        printf("%s", prefixe);
        colonne_actuelle = nb_boites;
    }
    while (colonne_actuelle < (50 - nb_boites)) {
        printf(" ");
        colonne_actuelle++;
    }
    for (int i = 0; i < nb_boites; i++) {
        printf("|");
    }
    printf("\n");
    colonne_actuelle = 0;
}

void afficher_mot(const char* mot, const char* prefixe) {
    int longueur_mot = strlen(mot);
    int len_prefixe = strlen(prefixe);
    int limite_droite = 50 - len_prefixe - 1;

    if (colonne_actuelle == 0) {
        printf("%s|", prefixe);
        colonne_actuelle = len_prefixe + 1;
    }
    if (colonne_actuelle + longueur_mot > limite_droite) {
        while (colonne_actuelle < limite_droite) {
            printf(" ");
            colonne_actuelle++;
        }
        for (int i = 0; i <= len_prefixe; i++) {
            printf("|");
        }
        printf("\n%s|", prefixe);
        colonne_actuelle = len_prefixe + 1;
    }
    printf("%s", mot);
    colonne_actuelle += longueur_mot;
    if (colonne_actuelle < limite_droite - 1) {
        printf(" ");
        colonne_actuelle++;
    }
}
void transformer_en_majuscules(char* texte) {
    if (texte == NULL){
        return;
    }
    for (int i = 0; texte[i] != '\0'; i++) {
        if (texte[i] >= 'a' && texte[i] <= 'z') {
            texte[i] = texte[i] - 32;
        }
    }
}

void parcourir_et_afficher(t_noeud* n, char* prefixe) {
    if (n == NULL) return;

    // Sauvegarde de la colonne actuelle avant de traiter le noeud
    // pour savoir si on doit fermer une ligne de texte
    
    switch (n->mon_type) {
        case TYPE_DOCUMENT:
        case TYPE_SECTION:
        case TYPE_ANNEXE: {
            terminer_ligne(prefixe); // On ferme ce qui était en cours
            afficher_bordure_horizontale(prefixe);
            
            // On prépare le nouveau préfixe pour les enfants
            char nouveau_prefixe[100];
            sprintf(nouveau_prefixe, "%s|", prefixe);
            
            // On affiche tout ce qu'il y a dedans
            parcourir_et_afficher(n->mon_premier_fils, nouveau_prefixe);
            
            terminer_ligne(nouveau_prefixe); 
            afficher_bordure_horizontale(prefixe);
            break;
        }

        case TYPE_TITRE: {
            // Un titre est souvent en majuscules
            // On traite ses fils (le texte du titre)
            t_noeud* curseur = n->mon_premier_fils;
            while(curseur != NULL) {
                if (curseur->mon_contenu){
                    transformer_en_majuscules(curseur->mon_contenu);
                }
                curseur = curseur->mon_frere_suivant;
            }
            parcourir_et_afficher(n->mon_premier_fils, prefixe);
            terminer_ligne(prefixe);
            break;
        }

        case TYPE_LISTE: {
            // Une liste n'affiche rien elle-même, elle passe juste le relais
            parcourir_et_afficher(n->mon_premier_fils, prefixe);
            break;
        }

        case TYPE_ITEM: {
            terminer_ligne(prefixe);
            // On affiche la puce '#'
            afficher_mot("#", prefixe); 
            parcourir_et_afficher(n->mon_premier_fils, prefixe);
            terminer_ligne(prefixe);
            break;
        }

        case TYPE_IMPORTANT: {
            // Exemple : entourer de '*' pour le rendu
            afficher_mot("*", prefixe);
            parcourir_et_afficher(n->mon_premier_fils, prefixe);
            afficher_mot("*", prefixe);
            break;
        }

        case TYPE_BR: {
            terminer_ligne(prefixe);
            break;
        }

        case TYPE_TEXTE_BRUT: {
            if (n->mon_contenu != NULL) {
                afficher_mot(n->mon_contenu, prefixe);
            }
            break;
        }
    }
    parcourir_et_afficher(n->mon_frere_suivant, prefixe);
}
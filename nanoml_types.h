#ifndef NANOML_TYPES_H
#define NANOML_TYPES_H

// differents types d'element de la grammaire
typedef enum {
    TYPE_DOCUMENT_GLOBAL,
    TYPE_DOCUMENT,      // <document>
    TYPE_ANNEXE,        // <annexe>
    TYPE_SECTION,       // <section>
    TYPE_TITRE,         // <titre>
    TYPE_LISTE,         // <liste>
    TYPE_ITEM,          // <item>
    TYPE_IMPORTANT,     // <important> 
    TYPE_BR,            // <br/> 
    TYPE_TEXTE_BRUT     // Contenu textuel pur 
} t_type_noeud;

// structure de l'arbre
typedef struct s_noeud {
    t_type_noeud mon_type;
    char* mon_contenu;              // Stocke le texte si c'est un mot/titre
    struct s_noeud* mon_premier_fils; // pointeur vers le premier fils
    struct s_noeud* mon_frere_suivant; // pointeur vers l'élément suivant au même niveau
} t_noeud;

#endif
# Projet NanoML : Analyseur et Rendu Graphique ASCII

Ce projet est un outil de traitement de fichiers au format NanoML (un dérivé simplifié du XML). Il permet de transformer un texte balisé en une structure d'arbre en mémoire pour générer une représentation graphique en boîtes ASCII de 50 colonnes.

# Binome
Cinar Songul
Guilbert Maxime


# Fonctionnalités
Analyseur Syntaxique : Construction d'un arbre n-aire pour représenter la hiérarchie du document et conserver l'imbrication des balises.
Moteur de Rendu ASCII : Affichage dynamique de boîtes avec bordures automatiques (`+---+`, `|`).
Gestion du Texte :
    Découpe des mots longs : Segmentation automatique des mots dépassant la largeur autorisée pour garantir l'intégrité visuelle.
    Support de l'UTF-8 : Utilisation de masques binaires (`0xC0`) pour gérer les caractères accentués sans décaler les bordures.
    Formatage spécial : Transformation récursive des titres en majuscules et gestion des balises `<important>` ou `<br/>`.


## Architecture du Projet
Le projet respecte un découpage modulaire strict:
 Lexer (`lexer.c/h`) : Transforme le flux de caractères en jetons (Tokens). Il a été optimisé pour lire le texte mot par mot.
 Analyseur (`analyseur.c/h`) : Consomme les jetons et bâtit l'arbre syntaxique en utilisant les pointeurs `mon_premier_fils` et `mon_frere_suivant`.
 Rendu (`rendu.c/h`) : Parcourt l'arbre de manière récursive pour gérer l'affichage et les marges via un système de préfixe.
 Types (`nanoml_types.h`) : Définition de la structure `t_noeud` et des types d'éléments.

# Compilation
Le projet est développé en langage C standard. Utilisez la commande suivante pour compiler sans avertissement:
```bash
make
./projet_nanoml {nom_fichier}.nml
```

## Limitations et Défis Techniques

 Rupture de flux textuel : Un comportement inattendu lors de la rencontre d'espaces peut limiter l'affichage au premier mot de certains blocs ; ce point constitue une piste d'amélioration pour la synchronisation Lexer/Analyseur.
 Profondeur d'imbrication : Le préfixe de bordure est actuellement limité à 100 caractères, ce qui restreint la gestion de structures extrêmement profondes.


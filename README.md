# Projet Pokemon Game (C++ / SFML)

Un moteur de jeu 2D style RPG (type Pokemon) développé en **C++** avec la bibliothèque **SFML 3.0**.
Ce projet gère l'affichage de cartes basées sur des tuiles (Tilemaps), la gestion de caméra dynamique, les animations de sprites et les collisions.

## Fonctionnalités Actuelles

* **Moteur de Tilemap :** Chargement et affichage de cartes via des fichiers CSV (exportés depuis Tiled).
* **Système de Calques (Layers) :**
    * *Sol :* Couche de fond (Herbe, terre, eau).
    * *Décor :* Couche supérieure avec transparence (Arbres, rochers) permettant de passer "derrière" les objets.
* **Caméra Intelligente :**
    * Suivi fluide du joueur.
    * **Clamping :** La caméra se bloque aux bords de la carte (on ne voit jamais le vide/noir).
    * **Zoom :** Zoom avant/arrière avec la molette de la souris (limité pour ne pas sortir de la map).
* **Joueur (Nouveau) :**
    * **Sprite Animé :** Utilisation d'une feuille de sprites (Sprite Sheet) au lieu d'un carré de couleur.
    * **Animation 4 Directions :** Le personnage s'anime (marche) vers le Haut, le Bas, la Gauche et la Droite en fonction du mouvement.
    * **Mise à l'échelle (Scale) :** Redimensionnement dynamique du sprite (ex: division par 2) pour correspondre à la taille des tuiles.
    * **Déplacements :** Contrôles ZQSD fluides.
    * **Collisions :** Impossible de sortir des limites de la carte.

## Technologies & Outils

* **Langage :** C++ (Standard C++17 ou supérieur recommandé).
* **Bibliothèque Graphique :** [SFML 3.0.2](https://www.sfml-dev.org/) (Gestion fenêtre, graphismes, événements).
* **Level Design :** [Tiled Map Editor](https://www.mapeditor.org/) (Création des maps et export en CSV).
* **Graphismes :** Piskel ou Paint.NET pour le nettoyage des Sprite Sheets.

## Structure du Projet

Voici l'organisation des fichiers :

```text
MonProjetPokemon/
├── assets/                  # Ressources du jeu
│   ├── textures/            # Tilesets (ex: free_pixel_16_woods.png)
│   └── maps/                # Fichiers sources Tiled (.tmx) et exports (.csv)
├── lib/                     # Correspond à SFML
├── src/                     # Code Source (.cpp)
│   ├── main.cpp             # Point d'entrée, boucle de jeu, animation et gestion de la map
│   ├── Game.cpp             # (A venir) Gestion globale
│   └── ...
├── include/                 # Fichiers En-tête (.hpp)
├── Makefile                 # Script de compilation (ou CMakeLists.txt)
├── mappokemon_decord.csv    # Données du calque décor
├── mappokemon_sol.csv       # Données du calque sol
├── free_pixel_16_woods.png  # Png pour la map (Open source : [https://zedpxl.itch.io/pixelart-forest-asset-pack](https://zedpxl.itch.io/pixelart-forest-asset-pack))
├── player_sheet.png         # Sprite Sheet du joueur (Fond transparent)
└── README.md                # Documentation
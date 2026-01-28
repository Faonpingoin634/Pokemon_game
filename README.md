# 🎮 Pokémon Game Engine (C++ / SFML)

Un moteur de jeu 2D style RPG (type Pokémon) développé en **C++** avec la bibliothèque **SFML 3.0**.

Le projet a récemment évolué d'un script unique vers une **architecture Orientée Objet** propre (Classes séparées). Il intègre désormais des collisions complexes (sol et décor), une caméra intelligente et une transition vers le mode combat.

## 🚀 Fonctionnalités Actuelles

### 🌍 Moteur & Carte
* **Tilemap Engine :** Chargement de cartes via fichiers CSV.
* **Système Double Couche :**
    * *Calque Sol :* Fond de carte (Terre, Eau, Sable).
    * *Calque Décor :* Objets (Arbres, Barrières, Fleurs).
* **Collisions Avancées :**
    * Système de **Whitelist** : Le code définit les tuiles traversables. Tout le reste (Arbres, Eau, Murs invisibles) devient solide.
    * Le joueur "glisse" contre les murs (gestion indépendante des axes X et Y).

### 🎥 Caméra Intelligente
* **Suivi Fluide :** La caméra reste centrée sur le joueur.
* **Clamping (Verrouillage) :** La caméra ne sort **jamais** des limites de la carte. Fini les bandes noires autour du jeu !
* **Zoom Sécurisé :** Le dézoom est bloqué mathématiquement pour ne pas dépasser la taille de la carte.

### 🚶 Joueur & Animation
* **Sprite Animé :** Gestion propre via la classe `Player`.
* **4 Directions :** Animation de marche (Haut, Bas, Gauche, Droite) fluide.
* **Gestion d'état :** Le joueur s'arrête sur la bonne frame quand il ne bouge plus.

### ⚔️ Système de Combat (WIP)
* **Hautes Herbes (ID 80) :**
    * Détection automatique quand le joueur marche dans les herbes.
    * **Timer RNG :** 40% de chance de rencontre toutes les 0.4 secondes de marche.


## 🛠 Architecture Technique

Le code a été entièrement refactorisé pour être modulaire et extensible :

* **`src/main.cpp` :** Le Chef d'orchestre. Il gère la boucle de jeu (Game Loop), les entrées clavier et l'alternance entre les états (Exploration/Battle).
* **`src/Map.cpp` / `include/Map.hpp` :**
    * Gère le chargement des fichiers `.csv`.
    * Gère l'affichage optimisé (VertexArray).
    * Contient la logique `isSolidAt(x, y)` pour vérifier les murs et l'eau.
* **`src/Player.cpp` / `include/Player.hpp` :**
    * Encapsule le Sprite SFML.
    * Gère la logique d'animation (`update(dt)`) et les déplacements.

## 📂 Structure du Projet

```text
MonProjetPokemon/
├── assets/                  # Ressources (Textures, CSV...)
├── lib/                     # Bibliothèques SFML
├── include/                 # Fichiers En-tête (.hpp)
│   ├── Map.hpp              # Définition de la Carte
│   ├── Player.hpp           # Définition du Joueur
│   └── ...
├── src/                     # Code Source (.cpp)
│   ├── main.cpp             # Boucle principale
│   ├── Map.cpp              # Implémentation Carte & Collisions
│   └── Player.cpp           # Implémentation Joueur & Animation
├── Makefile                 # Script de compilation automatisé
├── README.md                # Documentation
└── *.png / *.csv            # Fichiers de ressources (racine)
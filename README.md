# 🎮 Pokémon C++ Engine

![C++](https://img.shields.io/badge/Language-C%2B%2B17-blue?logo=c%2B%2B)
![SFML](https://img.shields.io/badge/Engine-SFML%203.0-green?logo=sfml)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey)
![Status](https://img.shields.io/badge/Status-Playable%20Demo-orange)
![License](https://img.shields.io/badge/License-MIT-blue)

Un jeu RPG en 2D inspiré de Pokémon, développé entièrement en **C++** avec la bibliothèque **SFML 3**. 
Ce projet démontre une architecture orientée objet solide (Game States, Entity Component, Event Handling).

---

## ✨ Fonctionnalités

### 🌍 Exploration & Monde
* **Moteur de Tuiles (Tile Engine) :** Chargement de cartes via fichiers CSV (Calques Sol & Décor).
* **Collisions :** Gestion des obstacles (arbres, eau) et des bordures de map.
* **Caméra Dynamique :** Suivi fluide du joueur avec verrouillage sur les bords de la carte (Clamping).
* **Hautes Herbes :** Système de rencontres aléatoires (RNG) basé sur le mouvement.

### ⚔️ Système de Combat
* **Tour par Tour :** Logique de combat complète (Attaques, Soins, Gestion des PV).
* **IA Ennemie :** L'adversaire choisit ses attaques aléatoirement.
* **Interface Graphique (GUI) :** Barres de vie, curseur de sélection et logs de combat textuels.
* **Bestiaire :** 5 Créatures implémentées dont 4 ennemies avec stats unique.

### 🎵 Audio & Ambiance
* **Gestion Audio Dynamique :** Transition fluide entre la musique d'exploration et de combat.
* **Easter Egg Caché :** Un événement spécial se déclenche sur une tuile précise de la carte (Case 102)... À vos risques et périls ! 🕺

### ⬅️⬆️➡️⬇️ Déplacement et choix en Combat
* **Gestion du déplacment :** Hors de la phase de combat vous pouvez vous déplacer sur la map avec Z, Q, S, D.
* **Gestion du combat :** En combat vous pouvez naviguer dans les attaques avec Q, D mais aussi avec les flèches directionelle comme pour le menu.

---

## 🛠️ Installation & Compilation

### Prérequis
* Un compilateur C++ compatible **C++17** (ex: `g++`).
* La bibliothèque **SFML 3.0** (Version statique recommandée).
* `Make` (pour utiliser le Makefile).

### Structure des Dossiers
Assurez-vous que votre dossier ressemble à ceci avant de lancer :

```text
MonProjet/
├── Makefile
├── Mappokemon_sol.csv
├── Mappokemon_decord.csv
├── free_pixel_16_woods.png
├── player_sheet.png
├── README.md
├── lib/
├── src/               # Code Source (.cpp)
├── include/           # En-têtes (.hpp)
└── assets/            # Ressources
    ├── audio/         # .ogg
    ├── textures/      # .png
    └── fonts/         # .ttf

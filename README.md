"# Pok-mon_game" 

MonProjetPokemon/
├── assets/                  # Tes images et polices (Indispensable)
│   ├── textures/            # ex: pikachu.png, background.png
│   └── fonts/               # ex: arial.ttf
├── src/                     # Tout ton code source (.cpp)
│   ├── main.cpp             # Juste le lancement du jeu
│   ├── Game.cpp             # La boucle principale
│   ├── BattleSystem.cpp     # La logique du combat
│   ├── Creature.cpp         # Les monstres
│   └── Skill.cpp            # Les attaques
├── include/                 # Tes fichiers d'en-tête (.hpp)
│   ├── Game.hpp
│   ├── BattleSystem.hpp
│   ├── Creature.hpp
│   └── Skill.hpp
├── CMakeLists.txt           # Pour compiler (ou Makefile) 
├── README.md                # OBLIGATOIRE [cite: 78]
└── .gitignore               # Pour ne pas envoyer les fichiers compilés
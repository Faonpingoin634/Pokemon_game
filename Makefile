# 1. Le Compilateur par défaut
CXX = g++

# 2. Les fichiers sources
SRC = src/main.cpp \
      src/Map.cpp \
      src/Player.cpp \
      src/BattleSystem.cpp \
      src/Creature.cpp \
      src/Game.cpp

# Transformation automatique (.cpp -> .o)
OBJ = $(SRC:.cpp=.o)

# Base des options de compilation pour tout le monde
CXXFLAGS = -Wall -Wextra -std=c++17 -I./include
LDFLAGS = 

# --- DÉTECTION DU SYSTÈME D'EXPLOITATION ---

ifeq ($(OS),Windows_NT)
    # ===== CONFIGURATION WINDOWS =====
    EXEC = monjeu.exe
    CXXFLAGS += -DSFML_STATIC
    # Chemins et librairies statiques spécifiques à Windows
    LDFLAGS += -Llib/Windows \
               -Wl,--whole-archive -lsfml-graphics-s -lsfml-window-s -lsfml-audio-s -lsfml-network-s -lsfml-system-s -Wl,--no-whole-archive \
               -Wl,--start-group -lvorbisfile -lvorbisenc -lvorbis -logg -lFLAC -lfreetype -Wl,--end-group \
               -lopengl32 -lwinmm -lgdi32 -lws2_32
    # Commande de nettoyage pour Windows (si cmd.exe)
    CLEAN_CMD = del /Q /S src\*.o 2>nul || rm -f src/*.o
else
    # Si on n'est pas sur Windows, on demande au système son nom
    UNAME_S := $(shell uname -s)
    EXEC = monjeu
    CLEAN_CMD = rm -f src/*.o

    ifeq ($(UNAME_S),Linux)
        # ===== CONFIGURATION LINUX =====
        # Utilisation des librairies dynamiques standards
        LDFLAGS += -Llib/Linux -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system
    endif

    ifeq ($(UNAME_S),Darwin)
        # ===== CONFIGURATION macOS =====
        # Sur Mac, on préfère souvent clang++
        CXX = clang++
        LDFLAGS += -Llib/macOS -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system
    endif
endif

# --- RÈGLES ---

all: $(EXEC)

# Création de l'exécutable final
$(EXEC): $(OBJ)
	@echo "Linking..."
	$(CXX) $(OBJ) -o $(EXEC) $(LDFLAGS)
	@echo "Done! Lance le jeu avec ./$(EXEC)"

# Règle générique : Comment transformer n'importe quel .cpp en .o
.cpp.o:
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Nettoyage
clean:
	@echo "Cleaning object files..."
	$(CLEAN_CMD)

re: clean all
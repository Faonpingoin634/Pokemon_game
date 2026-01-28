# 1. Le Compilateur
CXX = g++

# 2. Le nom de ton jeu
EXEC = monjeu.exe

# 3. Les fichiers sources (AJOUTE LES NOUVEAUX FICHIERS ICI)
SRC = src/main.cpp \
      src/Map.cpp \
      src/Player.cpp \
      src/BattleSystem.cpp \
      src/Creature.cpp

# 4. Transformation automatique (.cpp -> .o)
OBJ = $(SRC:.cpp=.o)

# 5. Options de compilation (Header, Version C++, Static)
CXXFLAGS = -Wall -Wextra -std=c++17 -DSFML_STATIC -I./include

# 6. Options d'édition de liens (Libraries SFML et dépendances Windows)
LDFLAGS = -Llib/Windows \
          -Wl,--whole-archive -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system -Wl,--no-whole-archive \
          -Wl,--start-group -lvorbisfile -lvorbisenc -lvorbis -logg -lFLAC -lfreetype -Wl,--end-group \
          -lopengl32 -lwinmm -lgdi32 -lws2_32

# --- RÈGLES ---

all: $(EXEC)

# Création de l'exécutable final en assemblant tous les .o
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
	rm -f src/*.o

re: clean all
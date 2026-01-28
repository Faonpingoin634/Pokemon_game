#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Map {
public:
    // Constructeur
    Map();
    
    // Charge les textures et les fichiers CSV
    bool load(const std::string& tilesetPath, const std::string& solPath, const std::string& decorPath);
    
    // Dessine la map
    void draw(sf::RenderWindow& window);

    // Vérifie si une position est un mur ou de l'eau
    bool isSolidAt(float x, float y);
    
    // Vérifie si on est dans les hautes herbes
    bool isGrassAt(float x, float y);

    // Getters pour la taille de la map
    int getWidth() const { return mapWidth; }
    int getHeight() const { return mapHeight; }

private:
    // Fonctions internes (cachées)
    std::vector<int> loadCsv(const std::string& filename);
    void updateGeometry(sf::VertexArray& vertices, const std::vector<int>& tiles);
    bool isTileSolid(int tileId);

    sf::Texture tileset;
    std::vector<int> levelSol;
    std::vector<int> levelDecor;
    sf::VertexArray vaSol;
    sf::VertexArray vaDecor;
    
    int mapWidth;
    int mapHeight;
    const int TILE_SIZE = 16;
};
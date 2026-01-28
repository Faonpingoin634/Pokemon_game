#include "../include/Map.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Map::Map() : mapWidth(0), mapHeight(0) {
    vaSol.setPrimitiveType(sf::PrimitiveType::Triangles);
    vaDecor.setPrimitiveType(sf::PrimitiveType::Triangles);
}

bool Map::load(const std::string& tilesetPath, const std::string& solPath, const std::string& decorPath) {
    if (!tileset.loadFromFile(tilesetPath)) return false;
    tileset.setSmooth(false);

    levelSol = loadCsv(solPath);
    levelDecor = loadCsv(decorPath);

    if (levelSol.empty() || levelDecor.empty()) return false;

    updateGeometry(vaSol, levelSol);
    updateGeometry(vaDecor, levelDecor);

    return true;
}

void Map::draw(sf::RenderWindow& window) {
    window.draw(vaSol, &tileset);
    window.draw(vaDecor, &tileset);
}

bool Map::isTileSolid(int tileId) {
    switch (tileId) {
        // --- TOUT CE QUI EST TRAVERSABLE ---
        case -1: case 80: 
        case 94: case 60: case 116: case 72: case 102: case 59: case 115:
        case 127: case 128: case 82: case 81: 
        case 17: case 18: case 19: case 20:
        case 69: case 73: case 144: case 187: 
        case 207: case 208: 
            return false;
        
        default: return true;
    }
}

bool Map::isSolidAt(float x, float y) {
    int tileX = (int)(x / TILE_SIZE);
    int tileY = (int)(y / TILE_SIZE);
    int index = tileX + tileY * mapWidth;

    if (tileX < 0 || tileX >= mapWidth || tileY < 0 || tileY >= mapHeight) return true; 

    // Si le décor OU le sol est solide, on bloque
    return isTileSolid(levelDecor[index]) || isTileSolid(levelSol[index]);
}

bool Map::isGrassAt(float x, float y) {
    int tileX = (int)(x / TILE_SIZE);
    int tileY = (int)(y / TILE_SIZE);
    int index = tileX + tileY * mapWidth;

    if (index >= 0 && index < (int)levelDecor.size()) {
        return levelDecor[index] == 80;
    }
    return false;
}

std::vector<int> Map::loadCsv(const std::string& filename) {
    std::vector<int> level;
    std::ifstream file(filename);
    std::string line;
    mapWidth = 0; mapHeight = 0; 

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        int rowWidth = 0;
        while (std::getline(ss, cell, ',')) {
            if (!cell.empty() && cell != "\r" && cell != "\n") {
                try { level.push_back(std::stoi(cell)); rowWidth++; } catch (...) {}
            }
        }
        if (mapWidth == 0) mapWidth = rowWidth;
        mapHeight++;
    }
    return level;
}

void Map::updateGeometry(sf::VertexArray& vertices, const std::vector<int>& tiles) {
    vertices.resize(mapWidth * mapHeight * 6);
    int tilesPerRow = tileset.getSize().x / TILE_SIZE;

    for (int i = 0; i < mapWidth; ++i) {
        for (int j = 0; j < mapHeight; ++j) {
            int tileNumber = tiles[i + j * mapWidth];
            if (tileNumber < 0) {
                for (int k = 0; k < 6; k++) vertices[(i + j * mapWidth) * 6 + k].position = {0,0};
                continue;
            }
            int tu = tileNumber % tilesPerRow; int tv = tileNumber / tilesPerRow;
            sf::Vertex* tri = &vertices[(i + j * mapWidth) * 6];
            float x = i * TILE_SIZE; float y = j * TILE_SIZE;
            float u = tu * TILE_SIZE; float v = tv * TILE_SIZE;

            tri[0].position={x,y}; tri[0].texCoords={u,v};
            tri[1].position={x+TILE_SIZE,y}; tri[1].texCoords={u+TILE_SIZE,v};
            tri[2].position={x,y+TILE_SIZE}; tri[2].texCoords={u,v+TILE_SIZE};
            tri[3].position={x,y+TILE_SIZE}; tri[3].texCoords={u,v+TILE_SIZE};
            tri[4].position={x+TILE_SIZE,y}; tri[4].texCoords={u+TILE_SIZE,v};
            tri[5].position={x+TILE_SIZE,y+TILE_SIZE}; tri[5].texCoords={u+TILE_SIZE,v+TILE_SIZE};
        }
    }
}
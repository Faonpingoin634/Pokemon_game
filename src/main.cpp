#include "../include/SFML/Graphics.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

// --- 1. FONCTION DE CHARGEMENT CSV ---
std::vector<int> loadCsv(const std::string& filename, int& width, int& height) {
    std::vector<int> level;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERREUR : Impossible d'ouvrir " << filename << std::endl;
        return level;
    }
    std::string line;
    width = 0; height = 0;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        int rowWidth = 0;
        while (std::getline(ss, cell, ',')) {
            if (!cell.empty() && cell != "\r" && cell != "\n") {
                try { level.push_back(std::stoi(cell)); rowWidth++; } catch (...) {}
            }
        }
        if (width == 0) width = rowWidth;
        height++;
    }
    return level;
}

// --- 2. FONCTION DE GEOMETRIE ---
void loadMapGeometry(sf::VertexArray& vertices, const std::vector<int>& tiles, const sf::Texture& tileset, sf::Vector2u tileSize, int width, int height) {
    vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    vertices.resize(width * height * 6);
    int tilesPerRow = tileset.getSize().x / tileSize.x;

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            int tileNumber = tiles[i + j * width];
            if (tileNumber < 0) {
                for (int k = 0; k < 6; k++) {
                    vertices[(i + j * width) * 6 + k].position = {0,0};
                    vertices[(i + j * width) * 6 + k].texCoords = {0,0};
                }
                continue;
            }
            int tu = tileNumber % tilesPerRow;
            int tv = tileNumber / tilesPerRow;
            sf::Vertex* tri = &vertices[(i + j * width) * 6];
            float x = (float)i * tileSize.x;
            float y = (float)j * tileSize.y;
            float u = (float)tu * tileSize.x;
            float v = (float)tv * tileSize.y;

            tri[0].position = {x, y};                 tri[0].texCoords = {u, v};
            tri[1].position = {x + tileSize.x, y};    tri[1].texCoords = {u + tileSize.x, v};
            tri[2].position = {x, y + tileSize.y};    tri[2].texCoords = {u, v + tileSize.y};
            tri[3].position = {x, y + tileSize.y};    tri[3].texCoords = {u, v + tileSize.y};
            tri[4].position = {x + tileSize.x, y};    tri[4].texCoords = {u + tileSize.x, v};
            tri[5].position = {x + tileSize.x, y + tileSize.y}; tri[5].texCoords = {u + tileSize.x, v + tileSize.y};
        }
    }
}

// --- 3. MAIN ---
int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Pokemon - Player Bound");
    window.setFramerateLimit(60);

    sf::Texture tileset;
    if (!tileset.loadFromFile("free_pixel_16_woods.png")) return -1;
    tileset.setSmooth(false);

    int mapW = 0, mapH = 0;
    std::vector<int> levelSol = loadCsv("mappokemon_sol.csv", mapW, mapH);
    std::vector<int> levelDecor = loadCsv("mappokemon_decord.csv", mapW, mapH);

    sf::VertexArray vaSol, vaDecor;
    if (!levelSol.empty())   loadMapGeometry(vaSol, levelSol, tileset, {16, 16}, mapW, mapH);
    if (!levelDecor.empty()) loadMapGeometry(vaDecor, levelDecor, tileset, {16, 16}, mapW, mapH);

    // --- JOUEUR ---
    sf::RectangleShape player({14.f, 14.f});
    player.setFillColor(sf::Color::Red);
    player.setOrigin({7.f, 7.f}); // Centre du joueur
    player.setPosition({(float)mapW * 16 / 2.0f, (float)mapH * 16 / 2.0f});

    // --- CAMERA ---
    sf::View view = window.getDefaultView();
    float currentZoom = 0.25f;
    view.zoom(currentZoom);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            
            // Zoom avec molette
            else if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
                float zoomFactor = (scroll->delta > 0) ? 0.9f : 1.1f;
                float newZoom = currentZoom * zoomFactor;
                sf::Vector2f viewSize = window.getDefaultView().getSize();
                viewSize.x *= newZoom; viewSize.y *= newZoom;
                float worldW = mapW * 16.0f; float worldH = mapH * 16.0f;

                if ((viewSize.x < worldW && viewSize.y < worldH) || scroll->delta > 0) {
                    view.zoom(zoomFactor);
                    currentZoom = newZoom;
                }
            }
        }

        // --- 1. DÉPLACEMENT DU JOUEUR ---
        float speed = 1.0f;
        sf::Vector2f movement(0.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) movement.x += speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) movement.x -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) movement.y -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) movement.y += speed;
        
        player.move(movement);

        // --- 2. COLLISION BORDS DE LA MAP ---
        sf::Vector2f pos = player.getPosition();
        float worldWidth = mapW * 16.0f;
        float worldHeight = mapH * 16.0f;
        float radius = 7.0f;

        // Si je vais trop à X,Y (moins que 0 + rayon)
        if (pos.x < radius) pos.x = radius;
        if (pos.y < radius) pos.y = radius;
        if (pos.x > worldWidth - radius) pos.x = worldWidth - radius;
        if (pos.y > worldHeight - radius) pos.y = worldHeight - radius;

        player.setPosition(pos);

        // --- 3. LA CAMÉRA SUIT LE JOUEUR ---
        view.setCenter(player.getPosition());

        // --- 4. VERROUILLAGE CAMÉRA ---
        sf::Vector2f viewSize = view.getSize();
        sf::Vector2f center = view.getCenter();

        if (center.x - viewSize.x / 2.0f < 0) center.x = viewSize.x / 2.0f;
        if (center.y - viewSize.y / 2.0f < 0) center.y = viewSize.y / 2.0f;
        if (center.x + viewSize.x / 2.0f > worldWidth)  center.x = worldWidth - viewSize.x / 2.0f;
        if (center.y + viewSize.y / 2.0f > worldHeight) center.y = worldHeight - viewSize.y / 2.0f;

        view.setCenter(center);
        window.setView(view);

        window.clear(sf::Color(20, 20, 30));
        window.draw(vaSol, &tileset);
        window.draw(vaDecor, &tileset);
        window.draw(player);
        window.display();
    }
    return 0;
}
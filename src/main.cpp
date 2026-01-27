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
    width = 0;
    height = 0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        int rowWidth = 0;

        while (std::getline(ss, cell, ',')) {
            // Nettoyage des caractères invisibles (retours à la ligne)
            if (!cell.empty() && cell != "\r" && cell != "\n") {
                try {
                    level.push_back(std::stoi(cell));
                    rowWidth++;
                } catch (...) {
                    // On ignore les erreurs de conversion
                }
            }
        }
        if (width == 0) width = rowWidth;
        height++;
    }
    return level;
}

// --- 2. FONCTION DE CONSTRUCTION GRAPHIQUE ---
void loadMapGeometry(sf::VertexArray& vertices, const std::vector<int>& tiles, const sf::Texture& tileset, sf::Vector2u tileSize, int width, int height) {
    
    vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    vertices.resize(width * height * 6);

    int tilesPerRow = tileset.getSize().x / tileSize.x;

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            
            int tileNumber = tiles[i + j * width];
            
            // Si c'est -1 (vide), on rend le triangle invisible
            if (tileNumber < 0) {
                for (int k = 0; k < 6; k++) {
                    vertices[(i + j * width) * 6 + k].position = sf::Vector2f(0, 0);
                    vertices[(i + j * width) * 6 + k].texCoords = sf::Vector2f(0, 0);
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

            tri[0].position = sf::Vector2f(x, y);                 tri[0].texCoords = sf::Vector2f(u, v);
            tri[1].position = sf::Vector2f(x + tileSize.x, y);    tri[1].texCoords = sf::Vector2f(u + tileSize.x, v);
            tri[2].position = sf::Vector2f(x, y + tileSize.y);    tri[2].texCoords = sf::Vector2f(u, v + tileSize.y);

            tri[3].position = sf::Vector2f(x, y + tileSize.y);    tri[3].texCoords = sf::Vector2f(u, v + tileSize.y);
            tri[4].position = sf::Vector2f(x + tileSize.x, y);    tri[4].texCoords = sf::Vector2f(u + tileSize.x, v);
            tri[5].position = sf::Vector2f(x + tileSize.x, y + tileSize.y); tri[5].texCoords = sf::Vector2f(u + tileSize.x, v + tileSize.y);
        }
    }
}

// --- 3. MAIN ---
int main() {
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Pokemon - Animated Player");
    window.setFramerateLimit(60);

    // 1. Charger le Tileset (Carte)
    sf::Texture tileset;
    if (!tileset.loadFromFile("free_pixel_16_woods.png")) {
        std::cerr << "Erreur: tileset introuvable." << std::endl;
        return -1;
    }
    tileset.setSmooth(false);

    // 2. Charger le SpriteSheet (Joueur)
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("player_sheet.png")) {
        std::cerr << "Erreur: player_sheet.png introuvable." << std::endl;
        return -1;
    } 
    playerTexture.setSmooth(false);

    // --- CONFIGURATION SPRITE JOUEUR ---
    sf::Sprite playerSprite(playerTexture);
    
    sf::Vector2i charSize(64, 64); 
    playerSprite.setTextureRect(sf::IntRect({0, 0}, charSize));
    
    playerSprite.setOrigin({(float)charSize.x / 2.f, (float)charSize.y - 2.f}); 
    // {0.5f, 0.5f} veut dire "50% largeur, 50% hauteur"
    playerSprite.setScale({0.5f, 0.5f});

    // Animation variables
    int animFrame = 0;      
    int animDirection = 0;  // 0=Bas, 1=Gauche, 2=Droite, 3=Haut
    float animTimer = 0.0f; 
    sf::Clock clock;        

    // Chargement Map
    int mapW = 0, mapH = 0;
    std::vector<int> levelSol = loadCsv("mappokemon_sol.csv", mapW, mapH);
    std::vector<int> levelDecor = loadCsv("mappokemon_decord.csv", mapW, mapH);
    sf::VertexArray vaSol, vaDecor;
    if (!levelSol.empty()) loadMapGeometry(vaSol, levelSol, tileset, {16, 16}, mapW, mapH);
    if (!levelDecor.empty()) loadMapGeometry(vaDecor, levelDecor, tileset, {16, 16}, mapW, mapH);

    // Position initiale
    playerSprite.setPosition({(float)mapW * 16 / 2.0f, (float)mapH * 16 / 2.0f});

    sf::View view = window.getDefaultView();
    float currentZoom = 0.25f;
    view.zoom(currentZoom);

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            
            // --- GESTION DU ZOOM ---
            else if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
                float zoomFactor = (scroll->delta > 0) ? 0.9f : 1.1f;
                float newZoom = currentZoom * zoomFactor;

                // Calcul limites zoom
                sf::Vector2f viewSize = window.getDefaultView().getSize();
                viewSize.x *= newZoom;
                viewSize.y *= newZoom;
                float worldW = mapW * 16.0f;
                float worldH = mapH * 16.0f;

                if ((viewSize.x < worldW && viewSize.y < worldH) || scroll->delta > 0) {
                    view.zoom(zoomFactor);
                    currentZoom = newZoom;
                }
            }
        }

        // --- 1. DÉPLACEMENT & DIRECTION ---
        float speed = 60.0f * dt; 
        sf::Vector2f movement(0.f, 0.f);
        bool isMoving = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            movement.x += speed; animDirection = 2; isMoving = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) { 
            movement.x -= speed; animDirection = 1; isMoving = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { 
            movement.y += speed; animDirection = 0; isMoving = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) { 
            movement.y -= speed; animDirection = 3; isMoving = true;
        }
        
        playerSprite.move(movement);

        // --- 2. ANIMATION ---
        if (isMoving) {
            animTimer += dt;
            if (animTimer > 0.15f) {
                animTimer = 0.0f;
                animFrame++;
                if (animFrame > 3) animFrame = 0; 
            }
        } else {
            animFrame = 0; 
        }

        int tx = animFrame * charSize.x;
        int ty = animDirection * charSize.y;
        playerSprite.setTextureRect(sf::IntRect({tx, ty}, charSize));

        // --- 3. COLLISIONS MAP ---
        sf::Vector2f pos = playerSprite.getPosition();
        float worldW = mapW * 16.0f;
        float worldH = mapH * 16.0f;
        float margin = 8.0f; 

        if (pos.x < margin) pos.x = margin;
        if (pos.y < margin) pos.y = margin;
        if (pos.x > worldW - margin) pos.x = worldW - margin;
        if (pos.y > worldH - margin) pos.y = worldH - margin;

        playerSprite.setPosition(pos);

        // --- 4. CAMERA ---
        view.setCenter(playerSprite.getPosition());
        
        sf::Vector2f vs = view.getSize();
        sf::Vector2f c = view.getCenter();
        if (c.x - vs.x/2 < 0) c.x = vs.x/2;
        if (c.y - vs.y/2 < 0) c.y = vs.y/2;
        if (c.x + vs.x/2 > worldW) c.x = worldW - vs.x/2;
        if (c.y + vs.y/2 > worldH) c.y = worldH - vs.y/2;
        view.setCenter(c);
        window.setView(view);

        // --- DESSIN ---
        window.clear(sf::Color(20, 20, 30));
        window.draw(vaSol, &tileset);
        window.draw(vaDecor, &tileset);
        window.draw(playerSprite);
        window.display();
    }
    return 0;
}
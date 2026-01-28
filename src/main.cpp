#include "../include/SFML/Graphics.hpp"
#include "../include/Map.hpp"
#include "../include/Player.hpp"
#include <iostream>
#include <ctime>
#include <cstdlib>

enum class GameState { Exploration, Battle };

int main() {
    std::srand(std::time(nullptr));
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Pokemon - Camera Fix");
    window.setFramerateLimit(60);

    // --- 1. INITIALISATION ---
    Map map;
    // Charge les fichiers
    if (!map.load("free_pixel_16_woods.png", "mappokemon_sol.csv", "mappokemon_decord.csv")) {
        return -1;
    }

    Player player;
    if (!player.load("player_sheet.png")) {
        return -1;
    }
    // Place le joueur au milieu de la map au début
    player.setPosition({(float)map.getWidth() * 16 / 2.0f, (float)map.getHeight() * 16 / 2.0f});

    // --- 2. VARIABLES ---
    sf::View view = window.getDefaultView();
    float currentZoom = 0.25f;
    view.zoom(currentZoom);

    GameState currentState = GameState::Exploration;
    sf::Clock clock;
    
    float grassTimer = 0.0f;

    // --- 3. BOUCLE DE JEU ---
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            
            // Zoom (Molette)
            if (currentState == GameState::Exploration) {
                if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
                    float zoomFactor = (scroll->delta > 0) ? 0.9f : 1.1f;

                    sf::Vector2f nextViewSize = view.getSize() * zoomFactor;

                    float worldW = map.getWidth() * 16.0f;
                    float worldH = map.getHeight() * 16.0f;

                    if (zoomFactor < 1.0f || (nextViewSize.x <= worldW && nextViewSize.y <= worldH)) {
                        view.zoom(zoomFactor);
                        currentZoom *= zoomFactor;
                    }
                }
            }
            
            // Debug : Quitter combat avec B
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::B && currentState == GameState::Battle) 
                    currentState = GameState::Exploration;
            }
        }

        if (currentState == GameState::Exploration) {
            // --- A. INPUTS ---
            float speed = 60.0f * dt;
            sf::Vector2f input(0.f, 0.f);
            bool isMoving = false;
            int dir = 0; 

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { input.x += speed; dir = 2; isMoving = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) { input.x -= speed; dir = 1; isMoving = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { input.y += speed; dir = 0; isMoving = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) { input.y -= speed; dir = 3; isMoving = true; }

            // --- B. COLLISIONS JOUEUR ---
            sf::Vector2f nextPos = player.getPosition();
            
            // Axe X
            nextPos.x += input.x;
            if (!map.isSolidAt(nextPos.x, nextPos.y)) player.setPosition({nextPos.x, player.getPosition().y});
            
            // Axe Y
            nextPos = player.getPosition(); 
            nextPos.y += input.y;
            if (!map.isSolidAt(nextPos.x, nextPos.y)) player.setPosition({player.getPosition().x, nextPos.y});

            // Limites Joueur (ne sort pas de la map)
            sf::Vector2f pos = player.getPosition();
            float maxW = map.getWidth() * 16.0f;
            float maxH = map.getHeight() * 16.0f;
            
            if (pos.x < 8) pos.x = 8;
            if (pos.y < 8) pos.y = 8;
            if (pos.x > maxW - 8) pos.x = maxW - 8;
            if (pos.y > maxH - 8) pos.y = maxH - 8;
            
            player.setPosition(pos);

            // --- C. ANIMATION ---
            player.setMoving(isMoving, dir); 
            player.update(dt); 

            // --- D. HAUTES HERBES ---
            if (isMoving && map.isGrassAt(pos.x, pos.y)) {
                grassTimer += dt;
                if (grassTimer > 0.4f) {
                    grassTimer = 0.0f;
                    if (std::rand() % 100 < 40) {
                        std::cout << "COMBAT !" << std::endl;
                        currentState = GameState::Battle;
                        window.setView(window.getDefaultView()); // Reset zoom pour le combat
                    }
                }
            } else { grassTimer = 0.0f; }

            // --- E. CAMERA ---
            view.setCenter(player.getPosition());
            
            // 2. On récupère les dimensions du monde et de la vue
            sf::Vector2f viewSize = view.getSize();
            sf::Vector2f center = view.getCenter();
            
            float worldW = map.getWidth() * 16.0f;
            float worldH = map.getHeight() * 16.0f;

            // 3. On corrige si la caméra dépasse les bords
            // Bord Gauche
            if (center.x - viewSize.x / 2.0f < 0) center.x = viewSize.x / 2.0f;
            // Bord Haut
            if (center.y - viewSize.y / 2.0f < 0) center.y = viewSize.y / 2.0f;
            // Bord Droit
            if (center.x + viewSize.x / 2.0f > worldW) center.x = worldW - viewSize.x / 2.0f;
            // Bord Bas
            if (center.y + viewSize.y / 2.0f > worldH) center.y = worldH - viewSize.y / 2.0f;

            // 4. On applique la correction
            view.setCenter(center);
            window.setView(view);

            // --- DESSIN ---
            window.clear(sf::Color(20, 20, 30));
            map.draw(window);
            player.draw(window);
        }
        else if (currentState == GameState::Battle) {
            window.clear(sf::Color::Red);
        }

        window.display();
    }
    return 0;
}
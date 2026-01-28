#include "../include/SFML/Graphics.hpp"
#include "../include/Map.hpp"
#include "../include/Player.hpp"
#include "../include/BattleSystem.hpp"
#include "../include/Creature.hpp"
#include <iostream>
#include <ctime>
#include <cstdlib>

enum class GameState { Exploration, Battle };

int main() {
    std::srand(std::time(nullptr));
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Pokemon - Battle System");
    window.setFramerateLimit(60);

    // --- 1. INITIALISATION ---
    Map map;
    if (!map.load("free_pixel_16_woods.png", "mappokemon_sol.csv", "mappokemon_decord.csv")) {
        std::cerr << "Erreur chargement Map" << std::endl;
        return -1;
    }

    Player player;
    if (!player.load("player_sheet.png")) { 
        std::cerr << "Erreur chargement Player" << std::endl;
        return -1;
    }
    // Place le joueur au milieu
    player.setPosition({(float)map.getWidth() * 16 / 2.0f, (float)map.getHeight() * 16 / 2.0f});

    // --- INITIALISATION COMBAT ---
    BattleSystem battleSystem;
    
    Creature pikachu("Pikachu", 100, 20);
    Creature dracaufeu("Dracaufeu", 120, 25);

    // --- 2. VARIABLES ---
    sf::View view = window.getDefaultView();
    float currentZoom = 0.25f;
    view.zoom(currentZoom); // On applique le zoom de départ

    GameState currentState = GameState::Exploration;
    sf::Clock clock;
    float grassTimer = 0.0f;

    // --- 3. BOUCLE DE JEU ---
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // GESTION DES EVENTS
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            
            // --- GESTION INPUT COMBAT ---
            if (currentState == GameState::Battle) {
                if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                    battleSystem.handleInput(key->code);
                }
            }
            // --- GESTION INPUT EXPLORATION ---
            else if (currentState == GameState::Exploration) {
                // Zoom
                if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
                    float zoomFactor = (scroll->delta > 0) ? 0.9f : 1.1f;
                    sf::Vector2f nextViewSize = view.getSize() * zoomFactor;
                    float worldW = map.getWidth() * 16.0f;
                    float worldH = map.getHeight() * 16.0f;

                    if (zoomFactor < 1.0f || (nextViewSize.x <= worldW && nextViewSize.y <= worldH)) {
                        view.zoom(zoomFactor); currentZoom *= zoomFactor;
                    }
                }
            }
        }

        // --- UPDATE & DRAW ---
        if (currentState == GameState::Exploration) {
            float speed = 60.0f * dt;
            sf::Vector2f input(0.f, 0.f);
            bool isMoving = false;
            int dir = 0; 

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { input.x += speed; dir = 2; isMoving = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) { input.x -= speed; dir = 1; isMoving = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { input.y += speed; dir = 0; isMoving = true; }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) { input.y -= speed; dir = 3; isMoving = true; }

            // Collisions
            sf::Vector2f nextPos = player.getPosition();
            nextPos.x += input.x;
            if (!map.isSolidAt(nextPos.x, nextPos.y)) player.setPosition({nextPos.x, player.getPosition().y});
            nextPos = player.getPosition();
            nextPos.y += input.y;
            if (!map.isSolidAt(nextPos.x, nextPos.y)) player.setPosition({player.getPosition().x, nextPos.y});

            // Clamping
            sf::Vector2f pos = player.getPosition();
            float maxW = map.getWidth() * 16.0f;
            float maxH = map.getHeight() * 16.0f;
            if (pos.x < 8) pos.x = 8;
            if (pos.y < 8) pos.y = 8;
            if (pos.x > maxW - 8) pos.x = maxW - 8;
            if (pos.y > maxH - 8) pos.y = maxH - 8;
            player.setPosition(pos);

            // Animation
            player.setMoving(isMoving, dir); 
            player.update(dt); 

            // Hautes Herbes -> Déclenchement Combat
            if (isMoving && map.isGrassAt(pos.x, pos.y)) {
                grassTimer += dt;
                if (grassTimer > 0.4f) {
                    grassTimer = 0.0f;
                    if (std::rand() % 100 < 40) { // 40% de chance
                        std::cout << "COMBAT !" << std::endl;
                        currentState = GameState::Battle;

                        dracaufeu.restoreHealth();
                        pikachu.restoreHealth();
                        
                        battleSystem.startBattle(pikachu, dracaufeu);
                        
                        // IMPORTANT : On passe en vue par défaut pour le combat !
                        window.setView(window.getDefaultView()); 
                    }
                }
            } else { grassTimer = 0.0f; }

            // Caméra Follow (Seulement en exploration)
            view.setCenter(player.getPosition());
            sf::Vector2f viewSize = view.getSize();
            sf::Vector2f center = view.getCenter();
            float worldW = map.getWidth() * 16.0f; float worldH = map.getHeight() * 16.0f;
            if (center.x - viewSize.x / 2.0f < 0) center.x = viewSize.x / 2.0f;
            if (center.y - viewSize.y / 2.0f < 0) center.y = viewSize.y / 2.0f;
            if (center.x + viewSize.x / 2.0f > worldW) center.x = worldW - viewSize.x / 2.0f;
            if (center.y + viewSize.y / 2.0f > worldH) center.y = worldH - viewSize.y / 2.0f;
            view.setCenter(center);
            window.setView(view);

            // Dessin Exploration
            window.clear(sf::Color(20, 20, 30));
            map.draw(window);
            player.draw(window);
        }
        else if (currentState == GameState::Battle) {
            // On s'assure que la caméra regarde TOUT l'écran, pas juste un zoom
            window.setView(window.getDefaultView());

            // --- LOGIQUE COMBAT ---
            battleSystem.update();

            if (battleSystem.isBattleOver()) {
                currentState = GameState::Exploration;
                
                view = window.getDefaultView(); 
                view.zoom(currentZoom);
                view.setCenter(player.getPosition());
                window.setView(view);
            }

            window.clear(sf::Color::Black); 
            battleSystem.draw(window);
        }

        window.display();
    }
    return 0;
}
#pragma once
#include <SFML/Graphics.hpp>
#include "Map.hpp"
#include "Player.hpp"
#include "BattleSystem.hpp"
#include "Creature.hpp"
#include <memory>

// 1. AJOUTE "MainMenu" ICI
enum class GameState { MainMenu, Exploration, Battle };

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

    void handleExploration(float dt);
    void handleBattle(float dt);
    
    // 2. AJOUTE CETTE FONCTION
    void handleMenuInput(sf::Keyboard::Key key);

    sf::RenderWindow window;
    sf::View view;
    float currentZoom;

    GameState currentState;

    Map map;
    Player player;
    Creature pikachu;
    Creature dracaufeu;
    BattleSystem* battleSystem;
    float grassTimer;

    // 3. VARIABLES DU MENU (AJOUTE TOUT ÇA)
    sf::Font font;         // 1. La police d'abord
    sf::Text titleText;    // 2. Les textes ensuite
    sf::Text playText;
    sf::Text quitText;
    int menuSelection;
};
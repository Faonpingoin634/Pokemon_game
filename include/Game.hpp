#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> 
#include "Map.hpp"
#include "Player.hpp"
#include "BattleSystem.hpp"
#include "Creature.hpp"
#include <memory>

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
    void handleMenuInput(sf::Keyboard::Key key);

    sf::RenderWindow window;
    sf::View view;
    float currentZoom;

    GameState currentState;

    Map map;
    Player player;
    
    // --- POKÉMON ---
    Creature pikachu;
    Creature dracaufeu;
    Creature togepi;
    Creature noctowl;
    Creature deoxys;

    BattleSystem* battleSystem;
    float grassTimer;

    // --- AUDIO ---
    sf::Music musicExploration; 
    sf::Music musicBattle;

    // --- MENU ---
    sf::Font font;
    sf::Text titleText;
    sf::Text playText;
    sf::Text quitText;
    int menuSelection;
};
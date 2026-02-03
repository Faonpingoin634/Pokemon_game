#pragma once
#include <SFML/Graphics.hpp>
#include "Creature.hpp"

class BattleSystem {
public:
    BattleSystem();

    void startBattle(Creature& player, Creature& opponent);
    void update();
    void draw(sf::RenderWindow& window);
    void handleInput(sf::Keyboard::Key key);
    bool isBattleOver() const;

private:
    Creature* playerCreature;
    Creature* enemyCreature;
    bool isPlayerTurn;
    bool battleEnded;
    int currentSelectionIndex;

    sf::Texture backgroundTexture;
    sf::Texture playerTexture;
    sf::Texture enemyTexture;

    sf::Sprite backgroundSprite;
    sf::Sprite playerSprite;
    sf::Sprite enemySprite;

    sf::RectangleShape playerHpBar;
    sf::RectangleShape playerHpBackground;
    sf::RectangleShape enemyHpBar;
    sf::RectangleShape enemyHpBackground;
    sf::RectangleShape selectionCursor;

    sf::Font font;

    sf::Text playerHpText;
    sf::Text enemyHpText;
    sf::Text attack1Text;
    sf::Text attack2Text;
    sf::Text attack3Text;
    sf::Text battleLog;

    sf::Clock turnTimer;
    bool waitingForEnemy = false;

    bool isAnimating = false;
    sf::Vector2f originalPos;
    float animTime;
    sf::Sprite* attackingSprite = nullptr;

    void executeTurn(int moveIndex);

    void updateHealthUI();
};
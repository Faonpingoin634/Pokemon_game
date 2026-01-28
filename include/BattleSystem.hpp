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
    sf::Sprite backgroundSprite;

    sf::Texture playerTexture;
    sf::Sprite playerSprite;
    sf::RectangleShape playerHpBar;
    sf::RectangleShape playerHpBackground;
    sf::Text playerHpText;

    sf::Texture enemyTexture;
    sf::Sprite enemySprite;
    sf::RectangleShape enemyHpBar;
    sf::RectangleShape enemyHpBackground;
    sf::Text enemyHpText;

    sf::Font font;
    sf::Text attack1Text;
    sf::Text attack2Text;
    sf::Text attack3Text;
    sf::Text battleLog;

    sf::RectangleShape selectionCursor;

    void executeTurn(int attackIndex);
    void executeEnemyTurn();
    void updateHealthUI();
};
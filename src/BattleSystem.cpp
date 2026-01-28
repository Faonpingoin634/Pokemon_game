#include "BattleSystem.hpp"
#include <string>
#include <random>

BattleSystem::BattleSystem() {
    backgroundTexture.loadFromFile("assets/textures/battle_background.png");
    backgroundSprite.setTexture(backgroundTexture);

    playerTexture.loadFromFile("assets/textures/mawile_back.png");
    playerSprite.setTexture(playerTexture);
    playerSprite.setPosition({150.f, 280.f});
    playerSprite.setScale({2.f, 2.f});

    enemyTexture.loadFromFile("assets/textures/ditto_front.png");
    enemySprite.setTexture(enemyTexture);
    enemySprite.setPosition({550.f, 100.f});
    enemySprite.setScale({2.f, 2.f});

    enemyHpBackground.setSize({200.f, 20.f});
    enemyHpBackground.setFillColor(sf::Color::Red);
    enemyHpBackground.setPosition({520.f, 50.f});

    enemyHpBar.setSize({200.f, 20.f});
    enemyHpBar.setFillColor(sf::Color::Green);
    enemyHpBar.setPosition({520.f, 50.f});

    playerHpBackground.setSize({200.f, 20.f});
    playerHpBackground.setFillColor(sf::Color::Red);
    playerHpBackground.setPosition({550.f, 350.f});

    playerHpBar.setSize({200.f, 20.f});
    playerHpBar.setFillColor(sf::Color::Green);
    playerHpBar.setPosition({550.f, 350.f});

    font.openFromFile("assets/fonts/arial.ttf");

    attack1Text.setFont(font);
    attack1Text.setString("Morsure");
    attack1Text.setCharacterSize(24);
    attack1Text.setFillColor(sf::Color::Black);
    attack1Text.setPosition({130.f, 490.f});

    attack2Text.setFont(font);
    attack2Text.setString("Soin");
    attack2Text.setCharacterSize(24);
    attack2Text.setFillColor(sf::Color::Black);
    attack2Text.setPosition({380.f, 490.f});

    attack3Text.setFont(font);
    attack3Text.setString("Charge");
    attack3Text.setCharacterSize(24);
    attack3Text.setFillColor(sf::Color::Black);
    attack3Text.setPosition({630.f, 490.f});

    battleLog.setFont(font);
    battleLog.setCharacterSize(20);
    battleLog.setFillColor(sf::Color::White);
    battleLog.setPosition({20.f, 450.f});
    battleLog.setString("Combat commence !");

    playerHpText.setFont(font);
    playerHpText.setCharacterSize(18);
    playerHpText.setPosition({550.f, 330.f});

    enemyHpText.setFont(font);
    enemyHpText.setCharacterSize(18);
    enemyHpText.setPosition({520.f, 30.f});

    currentSelectionIndex = 0;
    selectionCursor.setSize({220.f, 50.f});
    selectionCursor.setFillColor(sf::Color::Transparent);
    selectionCursor.setOutlineColor(sf::Color::Red);
    selectionCursor.setOutlineThickness(3.f);
    selectionCursor.setPosition({120.f, 485.f});
}

void BattleSystem::startBattle(Creature& player, Creature& opponent) {
    playerCreature = &player;
    enemyCreature = &opponent;
    isPlayerTurn = true;
    battleEnded = false;
    
    battleLog.setString("Un " + enemyCreature->name + " sauvage apparait !");
    updateHealthUI();
}

void BattleSystem::update() {
    if (battleEnded) return;
}

void BattleSystem::handleInput(sf::Keyboard::Key key) {
    if (battleEnded) return;

    if (key == sf::Keyboard::Key::Right) {
        if (currentSelectionIndex < 2) {
            currentSelectionIndex++;
        }
    }
    else if (key == sf::Keyboard::Key::Left) {
        if (currentSelectionIndex > 0) {
            currentSelectionIndex--;
        }
    }
    else if (key == sf::Keyboard::Key::Enter) {
        executeTurn(currentSelectionIndex);
    }

    if (currentSelectionIndex == 0) {
        selectionCursor.setPosition({120.f, 485.f});
    }
    else if (currentSelectionIndex == 1) {
        selectionCursor.setPosition({370.f, 485.f});
    }
    else if (currentSelectionIndex == 2) {
        selectionCursor.setPosition({620.f, 485.f});
    }
}

void BattleSystem::executeTurn(int attackIndex) {
    if (!isPlayerTurn) return;

    int damage = 0;
    std::string actionName = "";

    if (attackIndex == 0) {
        damage = 20;
        actionName = "Morsure";
    }
    else if (attackIndex == 1) {
        playerCreature->healthPoints += 30;
        if (playerCreature->healthPoints > playerCreature->maxHealth) {
            playerCreature->healthPoints = playerCreature->maxHealth;
        }
        actionName = "Soin";
        battleLog.setString("Tu te soignes !");
    }
    else if (attackIndex == 2) {
        damage = 10;
        actionName = "Charge";
    }

    if (damage > 0) {
        enemyCreature->takeDamage(damage);
        battleLog.setString("Tu utilises " + actionName + " !");
    }

    updateHealthUI();

    if (!enemyCreature->isAlive()) {
        battleLog.setString("Victoire !");
        battleEnded = true;
        return;
    }

    isPlayerTurn = false;
    
    executeEnemyTurn();
}

void BattleSystem::executeEnemyTurn() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 99);

    int roll = distrib(gen);
    int damage = 0;
    std::string logText;

    if (roll < 20) {
        damage = 0;
        logText = enemyCreature->name + " rate son attaque !";
    }
    else if (roll < 80) {
        damage = 10;
        logText = enemyCreature->name + " attaque !";
    }
    else {
        damage = 25;
        logText = "Coup critique de " + enemyCreature->name + " !";
    }

    if (damage > 0) {
        playerCreature->takeDamage(damage);
    }
    
    battleLog.setString(logText);
    updateHealthUI();

    if (!playerCreature->isAlive()) {
        battleLog.setString("Defaite... Game Over.");
        battleEnded = true;
    } else {
        isPlayerTurn = true;
    }
}

void BattleSystem::updateHealthUI() {
    if (!playerCreature || !enemyCreature) return;

    float enemyPercent = static_cast<float>(enemyCreature->healthPoints) / static_cast<float>(enemyCreature->maxHealth);
    float playerPercent = static_cast<float>(playerCreature->healthPoints) / static_cast<float>(playerCreature->maxHealth);

    if (enemyPercent < 0.f) enemyPercent = 0.f;
    if (playerPercent < 0.f) playerPercent = 0.f;

    enemyHpBar.setSize({200.f * enemyPercent, 20.f});
    playerHpBar.setSize({200.f * playerPercent, 20.f});

    enemyHpText.setString(std::to_string(enemyCreature->healthPoints) + "/" + std::to_string(enemyCreature->maxHealth));
    playerHpText.setString(std::to_string(playerCreature->healthPoints) + "/" + std::to_string(playerCreature->maxHealth));
}

void BattleSystem::draw(sf::RenderWindow& window) {
    window.draw(backgroundSprite);
    
    if (playerCreature) window.draw(playerSprite);
    if (enemyCreature) window.draw(enemySprite);

    window.draw(playerHpBackground);
    window.draw(playerHpBar);
    window.draw(playerHpText);

    window.draw(enemyHpBackground);
    window.draw(enemyHpBar);
    window.draw(enemyHpText);

    window.draw(attack1Text);
    window.draw(attack2Text);
    window.draw(attack3Text);
    
    window.draw(selectionCursor);
    window.draw(battleLog);
}

bool BattleSystem::isBattleOver() const {
    return battleEnded;
}
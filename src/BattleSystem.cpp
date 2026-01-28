#include "../include/BattleSystem.hpp"
#include <iostream>
#include <string>

BattleSystem::BattleSystem() : 
    backgroundSprite(backgroundTexture),
    playerSprite(playerTexture),
    enemySprite(enemyTexture),
    playerHpText(font),
    enemyHpText(font),
    attack1Text(font),
    attack2Text(font),
    attack3Text(font),
    battleLog(font)
{
    // --- 1. CHARGEMENT DES IMAGES ---
    
    // Le Fond
    if (!backgroundTexture.loadFromFile("assets/textures/battle_background.png")) {
        std::cerr << "ERREUR: Impossible de charger battle_background.png" << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture, true);
    sf::Vector2u size = backgroundTexture.getSize();
    float scaleX = 1280.0f / size.x;
    float scaleY = 720.0f / size.y;
    backgroundSprite.setScale({scaleX, scaleY});

    // Le Joueur
    if (!playerTexture.loadFromFile("assets/textures/ditto_front.png")) { 
        std::cerr << "ERREUR: Impossible de charger ditto_front.png" << std::endl;
    }
    playerSprite.setTexture(playerTexture, true);

    playerSprite.setPosition({280.f, 180.f});
    playerSprite.setScale({3.f, 3.f});

    // L'Ennemi
    if (!enemyTexture.loadFromFile("assets/textures/mawile_back.png")) {
        std::cerr << "ERREUR: Impossible de charger mawile_back.png" << std::endl;
    }
    enemySprite.setTexture(enemyTexture, true);

    enemySprite.setPosition({700.f, 10.f});
    enemySprite.setScale({2.5f, 2.5f});

    // La Police
    if (!font.openFromFile("assets/fonts/arial.ttf")) {
         std::cerr << "ERREUR: Impossible de charger arial.ttf" << std::endl;
    }

    // --- 2. CONFIGURATION DE L'INTERFACE (Barres de vie) ---
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

    // --- 3. CONFIGURATION DU TEXTE ---
    playerHpText.setCharacterSize(20);
    playerHpText.setFillColor(sf::Color::White);
    playerHpText.setPosition({550.f, 380.f});

    enemyHpText.setCharacterSize(20);
    enemyHpText.setFillColor(sf::Color::White);
    enemyHpText.setPosition({520.f, 80.f});

    // Variables de base
    isPlayerTurn = true;
    battleEnded = false;
    currentSelectionIndex = 0;
}

void BattleSystem::startBattle(Creature& player, Creature& opponent) {
    playerCreature = &player;
    enemyCreature = &opponent;
    
    isPlayerTurn = true;
    battleEnded = false;
    updateHealthUI();
    
    battleLog.setString("Combat lance !");
    std::cout << "DEBUT COMBAT : " << player.name << " vs " << opponent.name << std::endl;
}

void BattleSystem::update() {
    if (battleEnded) return;
    updateHealthUI();
}

void BattleSystem::handleInput(sf::Keyboard::Key key) {
    if (battleEnded) return;

    if (key == sf::Keyboard::Key::Space) {
        if (isPlayerTurn && playerCreature && enemyCreature) {
            std::cout << "Pikachu attaque !" << std::endl;
            enemyCreature->takeDamage(playerCreature->attackPower);
            isPlayerTurn = false; 
            
            // Riposte immédiate
            if (enemyCreature->isAlive()) {
                std::cout << "Dracaufeu riposte !" << std::endl;
                playerCreature->takeDamage(enemyCreature->attackPower);
                isPlayerTurn = true;
            }
        }
    }
}

bool BattleSystem::isBattleOver() const {
    if (!playerCreature || !enemyCreature) return true;
    if (!playerCreature->isAlive() || !enemyCreature->isAlive()) return true;
    return false;
}

void BattleSystem::updateHealthUI() {
    if (!playerCreature || !enemyCreature) return;

    float enemyPercent = static_cast<float>(enemyCreature->healthPoints) / static_cast<float>(enemyCreature->maxHealth);
    float playerPercent = static_cast<float>(playerCreature->healthPoints) / static_cast<float>(playerCreature->maxHealth);

    // Sécurité pour pas avoir de barre négative
    if (enemyPercent < 0.f) enemyPercent = 0.f;
    if (playerPercent < 0.f) playerPercent = 0.f;

    enemyHpBar.setSize({200.f * enemyPercent, 20.f});
    playerHpBar.setSize({200.f * playerPercent, 20.f});

    playerHpText.setString(std::to_string(playerCreature->healthPoints) + "/" + std::to_string(playerCreature->maxHealth));
    enemyHpText.setString(std::to_string(enemyCreature->healthPoints) + "/" + std::to_string(enemyCreature->maxHealth));
}

void BattleSystem::draw(sf::RenderWindow& window) {
    // 1. D'abord le fond
    window.draw(backgroundSprite);
    
    // 2. Ensuite les Pokémon
    if (enemyCreature) window.draw(enemySprite);
    if (playerCreature) window.draw(playerSprite);

    // 3. L'interface
    window.draw(playerHpBackground);
    window.draw(playerHpBar);
    window.draw(enemyHpBackground);
    window.draw(enemyHpBar);
    
    window.draw(playerHpText);
    window.draw(enemyHpText);
}
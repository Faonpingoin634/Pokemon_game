#include "../include/BattleSystem.hpp"
#include <iostream>
#include <string>
#include <cmath>

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
    // --- 1. CHARGEMENT IMAGES ---
    if (!backgroundTexture.loadFromFile("assets/textures/battle_background.png")) {
        std::cerr << "ERREUR: Impossible de charger battle_background.png" << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture, true);
    sf::Vector2u size = backgroundTexture.getSize();
    float scaleX = 1280.0f / size.x;
    float scaleY = 720.0f / size.y;
    backgroundSprite.setScale({scaleX, scaleY});

    // --- JOUEUR ---
    if (!playerTexture.loadFromFile("assets/textures/ditto_front.png")) { 
        std::cerr << "ERREUR: Impossible de charger ditto_front.png" << std::endl;
    }
    playerSprite.setTexture(playerTexture, true);
    playerSprite.setPosition({280.f, 180.f});
    playerSprite.setScale({3.f, 3.f});

    // --- ENNEMI ---
    if (!enemyTexture.loadFromFile("assets/textures/mawile_back.png")) {
        std::cerr << "ERREUR: Impossible de charger mawile_back.png" << std::endl;
    }
    enemySprite.setTexture(enemyTexture, true);
    enemySprite.setPosition({700.f, 10.f});
    enemySprite.setScale({2.5f, 2.5f});

    if (!font.openFromFile("assets/fonts/arial.ttf")) {
         std::cerr << "ERREUR: Impossible de charger arial.ttf" << std::endl;
    }

    // --- 2. INTERFACE (UI) ---
    // Ennemi
    enemyHpBackground.setSize({200.f, 20.f});
    enemyHpBackground.setFillColor(sf::Color::Red);
    enemyHpBackground.setPosition({520.f, 50.f});

    enemyHpBar.setSize({200.f, 20.f});
    enemyHpBar.setFillColor(sf::Color::Green);
    enemyHpBar.setPosition({520.f, 50.f});

    enemyHpText.setCharacterSize(20);
    enemyHpText.setFillColor(sf::Color::White);
    enemyHpText.setPosition({520.f, 80.f});

    // Joueur
    playerHpBackground.setSize({200.f, 20.f});
    playerHpBackground.setFillColor(sf::Color::Red);
    playerHpBackground.setPosition({550.f, 350.f});

    playerHpBar.setSize({200.f, 20.f});
    playerHpBar.setFillColor(sf::Color::Green);
    playerHpBar.setPosition({550.f, 350.f});

    playerHpText.setCharacterSize(20);
    playerHpText.setFillColor(sf::Color::White);
    playerHpText.setPosition({550.f, 380.f});

    // Initialisation variables
    isPlayerTurn = true;
    battleEnded = false;
    currentSelectionIndex = 0;
    isAnimating = false;
    waitingForEnemy = false;
    attackingSprite = nullptr;
}

void BattleSystem::startBattle(Creature& player, Creature& opponent) {
    playerCreature = &player;
    enemyCreature = &opponent;
    
    isPlayerTurn = true;
    battleEnded = false;
    isAnimating = false;
    waitingForEnemy = false;
    
    updateHealthUI();
    
    battleLog.setString("Combat lance !");
    std::cout << "DEBUT COMBAT : " << player.name << " vs " << opponent.name << std::endl;
}

void BattleSystem::update() {
    if (battleEnded) return;

    float dt = 0.016f; 

    // --- 1. ANIMATION ---
    if (isAnimating && attackingSprite) {
        animTime += dt * 5.0f; // Vitesse anim
        
        float offset = std::sin(animTime * 3.14f) * 50.0f; 
        float dir = (attackingSprite == &playerSprite) ? 1.0f : -1.0f;
        attackingSprite->setPosition({originalPos.x + (offset * dir), originalPos.y - (offset * 0.5f * dir)});

        // FIN ANIMATION
        if (animTime >= 1.0f) { 
            attackingSprite->setPosition(originalPos);
            isAnimating = false;
            attackingSprite = nullptr;

            if (isPlayerTurn) {
                // Le joueur vient de finir son attaque
                enemyCreature->takeDamage(playerCreature->attackPower); // <--- DÉGÂTS APPLIQUÉS ICI
                std::cout << "PV Ennemi: " << enemyCreature->healthPoints << std::endl;

                if (!enemyCreature->isAlive()) {
                    battleEnded = true;
                } else {
                    isPlayerTurn = false;
                    waitingForEnemy = true;
                    turnTimer.restart();
                }
            } 
            else {
                // L'ennemi vient de finir son attaque
                playerCreature->takeDamage(enemyCreature->attackPower); // <--- DÉGÂTS APPLIQUÉS ICI
                std::cout << "PV Joueur: " << playerCreature->healthPoints << std::endl;

                if (!playerCreature->isAlive()) {
                    battleEnded = true;
                } else {
                    waitingForEnemy = false;
                    isPlayerTurn = true;
                }
            }
        }
    }

    // --- 2. LOGIQUE ENNEMI (Déclenchement) ---
    if (waitingForEnemy && !isPlayerTurn && !isAnimating) {
        // Délai de 0.5 secondes avant que l'ennemi n'attaque
        if (turnTimer.getElapsedTime().asSeconds() > 0.5f) {
            isAnimating = true;
            animTime = 0.0f;
            attackingSprite = &enemySprite;
            originalPos = enemySprite.getPosition();
            // On n'applique PAS les dégâts ici, mais à la fin de l'animation
        }
    }
    updateHealthUI();
}

void BattleSystem::handleInput(sf::Keyboard::Key key) {
    if (battleEnded || waitingForEnemy || isAnimating) return; 

    if (key == sf::Keyboard::Key::Enter || key == sf::Keyboard::Key::Space) {
        if (isPlayerTurn && playerCreature && enemyCreature) {
            // On lance l'animation du joueur
            isAnimating = true;
            animTime = 0.0f;
            attackingSprite = &playerSprite;
            originalPos = playerSprite.getPosition();
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

    if (enemyPercent < 0.f) enemyPercent = 0.f;
    if (playerPercent < 0.f) playerPercent = 0.f;

    enemyHpBar.setSize({200.f * enemyPercent, 20.f});
    playerHpBar.setSize({200.f * playerPercent, 20.f});

    playerHpText.setString(std::to_string(playerCreature->healthPoints) + "/" + std::to_string(playerCreature->maxHealth));
    enemyHpText.setString(std::to_string(enemyCreature->healthPoints) + "/" + std::to_string(enemyCreature->maxHealth));
}

void BattleSystem::draw(sf::RenderWindow& window) {
    window.draw(backgroundSprite);
    if (enemyCreature) window.draw(enemySprite);
    if (playerCreature) window.draw(playerSprite);
    window.draw(playerHpBackground);
    window.draw(playerHpBar);
    window.draw(enemyHpBackground);
    window.draw(enemyHpBar);
    window.draw(playerHpText);
    window.draw(enemyHpText);
}
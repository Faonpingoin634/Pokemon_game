#include "../include/BattleSystem.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib> 

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
    // --- CHARGEMENT ---
    if (!backgroundTexture.loadFromFile("assets/textures/battle_background.png")) {
        std::cerr << "ERREUR: Impossible de charger battle_background.png" << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture, true);
    sf::Vector2u size = backgroundTexture.getSize();
    float scaleX = 1280.0f / size.x;
    float scaleY = 720.0f / size.y;
    backgroundSprite.setScale({scaleX, scaleY});

    if (!playerTexture.loadFromFile("assets/textures/ditto_front.png")) std::cerr << "Err Ditto" << std::endl;
    playerSprite.setTexture(playerTexture, true);
    playerSprite.setPosition({280.f, 180.f});
    playerSprite.setScale({3.f, 3.f});

    if (!enemyTexture.loadFromFile("assets/textures/mawile_back.png")) std::cerr << "Err Mawile" << std::endl;
    enemySprite.setTexture(enemyTexture, true);
    enemySprite.setPosition({700.f, 10.f});
    enemySprite.setScale({2.5f, 2.5f});

    if (!font.openFromFile("assets/fonts/arial.ttf")) std::cerr << "Err Font" << std::endl;

    // --- UI ---
    enemyHpBackground.setSize({200.f, 20.f});
    enemyHpBackground.setFillColor(sf::Color::Red);
    enemyHpBackground.setPosition({520.f, 50.f});

    enemyHpBar.setSize({200.f, 20.f});
    enemyHpBar.setFillColor(sf::Color::Green);
    enemyHpBar.setPosition({520.f, 50.f});

    enemyHpText.setCharacterSize(20);
    enemyHpText.setFillColor(sf::Color::White);
    enemyHpText.setPosition({520.f, 80.f});

    playerHpBackground.setSize({200.f, 20.f});
    playerHpBackground.setFillColor(sf::Color::Red);
    playerHpBackground.setPosition({550.f, 350.f});

    playerHpBar.setSize({200.f, 20.f});
    playerHpBar.setFillColor(sf::Color::Green);
    playerHpBar.setPosition({550.f, 350.f});

    playerHpText.setCharacterSize(20);
    playerHpText.setFillColor(sf::Color::White);
    playerHpText.setPosition({550.f, 380.f});

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
    
    // 1. CONFIGURATION DU JOUEUR
    playerSprite.setTexture(player.texture, true);
    playerSprite.setScale(player.sprite.getScale());
    playerSprite.setPosition(player.sprite.getPosition());

    // 2. CONFIGURATION DE L'ENNEMI
    enemySprite.setTexture(opponent.texture, true);
    enemySprite.setScale(opponent.sprite.getScale());
    enemySprite.setPosition(opponent.sprite.getPosition());

    // Réinitialisation de l'état du combat
    isPlayerTurn = true;
    battleEnded = false;
    isAnimating = false;
    waitingForEnemy = false;
    
    updateHealthUI();
    std::cout << "DEBUT COMBAT : " << player.name << " vs " << opponent.name << std::endl;
}
void BattleSystem::executeTurn(int moveIndex) {
    Move selectedMove = playerCreature->moves[moveIndex];
    bool shouldAnimate = false; 

    // --- LOGIQUE JOUEUR ---
    if (selectedMove.type == HEAL) {
        playerCreature->heal(selectedMove.power);
        std::cout << "JOUEUR se soigne ! PV : " << playerCreature->healthPoints << std::endl;
        shouldAnimate = false;
    } 
    else {
        int damage = selectedMove.power;
        enemyCreature->takeDamage(damage);
        shouldAnimate = true;
    }

    updateHealthUI();

    if (shouldAnimate) {
        isAnimating = true;
        animTime = 0.f;
        attackingSprite = &playerSprite;
        originalPos = playerSprite.getPosition();
    } else {
        isAnimating = false;
        attackingSprite = nullptr;
    }
    
    isPlayerTurn = false;
    waitingForEnemy = true;
    turnTimer.restart();
}

void BattleSystem::update() {
    if (battleEnded) return;
    float dt = 0.016f; 

    // ANIMATION JOUEUR
    if (isAnimating && attackingSprite) {
        animTime += dt * 5.0f;
        float offset = std::sin(animTime * 3.14f) * 50.0f; 
        float dir = (attackingSprite == &playerSprite) ? 1.0f : -1.0f;
        attackingSprite->setPosition({originalPos.x + (offset * dir), originalPos.y - (offset * 0.5f * dir)});

        if (animTime >= 1.0f) { 
            attackingSprite->setPosition(originalPos);
            isAnimating = false;
            attackingSprite = nullptr;

            if (!enemyCreature->isAlive() || !playerCreature->isAlive()) {
                battleEnded = true;
            }
        }
    }

    // --- INTELLIGENCE ARTIFICIELLE (IA) ---
    if (waitingForEnemy && !isAnimating && !battleEnded) {
        if (turnTimer.getElapsedTime().asSeconds() > 1.0f) {
            
            // 1. Choix de l'attaque
            int rng = std::rand() % 100;
            int moveIndex = 0;

            if (rng < 40) moveIndex = 0;      
            else if (rng < 80) moveIndex = 1;  
            else moveIndex = 2;                

            if (moveIndex >= (int)enemyCreature->moves.size()) moveIndex = 0;

            Move enemyMove = enemyCreature->moves[moveIndex];
            bool enemyAnimate = false;

            std::cout << "Ennemi utilise : " << enemyMove.name << std::endl;

            // 2. Application de l'effet
            if (enemyMove.type == HEAL) {
                enemyCreature->heal(enemyMove.power);
                enemyAnimate = false;
            } else {
                int dmg = enemyMove.power;
                playerCreature->takeDamage(dmg);
                enemyAnimate = true;
            }
            
            updateHealthUI();

            // 3. Animation Ennemie
            if (enemyAnimate) {
                isAnimating = true;
                animTime = 0.0f;
                attackingSprite = &enemySprite;
                originalPos = enemySprite.getPosition();
            }

            waitingForEnemy = false;
            isPlayerTurn = true;
        }
    }
}

void BattleSystem::handleInput(sf::Keyboard::Key key) {
    if (battleEnded || isAnimating || waitingForEnemy) return;

    if (key == sf::Keyboard::Key::Right || key == sf::Keyboard::Key::D) {
        currentSelectionIndex++;
        if (currentSelectionIndex > 2) currentSelectionIndex = 0;
    }
    if (key == sf::Keyboard::Key::Left || key == sf::Keyboard::Key::Q) {
        currentSelectionIndex--;
        if (currentSelectionIndex < 0) currentSelectionIndex = 2;
    }

    if (key == sf::Keyboard::Key::Enter || key == sf::Keyboard::Key::Space) {
        if (currentSelectionIndex < (int)playerCreature->moves.size()) {
            executeTurn(currentSelectionIndex);
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
    
    if (isPlayerTurn && !battleEnded && !waitingForEnemy && !isAnimating) {
        
        std::vector<sf::Vector2f> positions = {
            {175.f,  600.f},
            {575.f,  600.f},
            {1050.f, 600.f}
        };

        for (size_t i = 0; i < playerCreature->moves.size(); i++) {
            sf::Text moveText(font);
            moveText.setString(playerCreature->moves[i].name);
            moveText.setCharacterSize(24);
            moveText.setFillColor(sf::Color::Black); 

            if (i < positions.size()) moveText.setPosition(positions[i]);

            if ((int)i == currentSelectionIndex) {
                moveText.setFillColor(sf::Color::Red); 
                moveText.setStyle(sf::Text::Bold);
                
                sf::Text cursor(font);
                cursor.setString(">");
                cursor.setCharacterSize(24);
                cursor.setFillColor(sf::Color::Red);
                if (i < positions.size()) cursor.setPosition({positions[i].x - 20.f, positions[i].y});
                window.draw(cursor);
            }
            window.draw(moveText);
        }
    }
}
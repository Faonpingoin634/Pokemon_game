#include "../include/Game.hpp"
#include <iostream>
#include <ctime>
#include <cstdlib>

Game::Game() 
    : window(sf::VideoMode({1280, 720}), "Pokemon - Battle System"),
      currentZoom(0.25f),
      currentState(GameState::MainMenu),
      pikachu("Pikachu", 100, 20),
      dracaufeu("Dracaufeu", 120, 25),
      battleSystem(nullptr),
      grassTimer(0.0f),
      font(), 
      titleText(font), 
      playText(font), 
      quitText(font),
      menuSelection(0)
{
    window.setFramerateLimit(60);
    std::srand(std::time(nullptr));

    // --- PIKACHU ---
    pikachu.addMove("Charge", 25, ATTACK);
    pikachu.addMove("Vive-attaque", 35, ATTACK);
    pikachu.addMove("Heal", 25, HEAL);

    // --- DRACAUFEU (IA) ---
    dracaufeu.addMove("Griffe", 30, ATTACK);
    dracaufeu.addMove("Lance-Flamme", 40, ATTACK);
    dracaufeu.addMove("Repos", 30, HEAL);


    if (!map.load("free_pixel_16_woods.png", "mappokemon_sol.csv", "mappokemon_decord.csv")) exit(-1);
    if (!player.load("player_sheet.png")) exit(-1);
    
    player.setPosition({(float)map.getWidth() * 16 / 2.0f, (float)map.getHeight() * 16 / 2.0f});
    
    view = window.getDefaultView();
    view.zoom(currentZoom);

    if (!font.openFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "ERREUR: Font introuvable" << std::endl;
    }

    titleText.setFont(font);
    titleText.setString("POKEMON C++");
    titleText.setCharacterSize(80);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setOutlineColor(sf::Color::Blue);
    titleText.setOutlineThickness(4);
    titleText.setPosition({350.f, 100.f});

    playText.setFont(font);
    playText.setString("JOUER");
    playText.setCharacterSize(50);
    playText.setPosition({550.f, 350.f});

    quitText.setFont(font);
    quitText.setString("QUITTER");
    quitText.setCharacterSize(50);
    quitText.setPosition({530.f, 450.f});
}

Game::~Game() {
    if (battleSystem != nullptr) {
        delete battleSystem;
        battleSystem = nullptr;
    }
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        if (currentState == GameState::MainMenu) {
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                handleMenuInput(key->code);
            }
        }
        else if (currentState == GameState::Battle && battleSystem != nullptr) {
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                battleSystem->handleInput(key->code);
            }
        }
        else if (currentState == GameState::Exploration) {
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
    }
}

void Game::handleMenuInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::Z) {
        menuSelection--;
        if (menuSelection < 0) menuSelection = 1;
    }
    else if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S) {
        menuSelection++;
        if (menuSelection > 1) menuSelection = 0;
    }
    else if (key == sf::Keyboard::Key::Enter) {
        if (menuSelection == 0) {
            currentState = GameState::Exploration;
            view = window.getDefaultView();
            view.zoom(currentZoom);
            view.setCenter(player.getPosition());
            window.setView(view);
        }
        else if (menuSelection == 1) {
            window.close();
        }
    }
}

void Game::update(float dt) {
    if (currentState == GameState::Exploration) {
        handleExploration(dt);
    }
    else if (currentState == GameState::Battle && battleSystem != nullptr) {
        battleSystem->update();
        if (battleSystem->isBattleOver()) {
            delete battleSystem;
            battleSystem = nullptr;
            currentState = GameState::Exploration;
            view = window.getDefaultView();
            view.zoom(currentZoom);
            view.setCenter(player.getPosition());
            window.setView(view);
        }
    }
}

void Game::handleExploration(float dt) {
    float speed = 60.0f * dt;
    sf::Vector2f input(0.f, 0.f);
    bool isMoving = false;
    int dir = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { input.x += speed; dir = 2; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) { input.x -= speed; dir = 1; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { input.y += speed; dir = 0; isMoving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) { input.y -= speed; dir = 3; isMoving = true; }

    sf::Vector2f nextPos = player.getPosition();
    nextPos.x += input.x;
    if (!map.isSolidAt(nextPos.x, nextPos.y)) player.setPosition({nextPos.x, player.getPosition().y});
    
    nextPos = player.getPosition();
    nextPos.y += input.y;
    if (!map.isSolidAt(nextPos.x, nextPos.y)) player.setPosition({player.getPosition().x, nextPos.y});

    sf::Vector2f pos = player.getPosition();
    float maxW = map.getWidth() * 16.0f;
    float maxH = map.getHeight() * 16.0f;
    if (pos.x < 8) pos.x = 8;
    if (pos.y < 8) pos.y = 8;
    if (pos.x > maxW - 8) pos.x = maxW - 8;
    if (pos.y > maxH - 8) pos.y = maxH - 8;
    player.setPosition(pos);

    player.setMoving(isMoving, dir);
    player.update(dt);

    if (isMoving && map.isGrassAt(pos.x, pos.y)) {
        grassTimer += dt;
        if (grassTimer > 0.4f) {
            grassTimer = 0.0f;
            if (std::rand() % 100 < 40) {
                currentState = GameState::Battle;
                dracaufeu.restoreHealth();
                pikachu.restoreHealth();
                battleSystem = new BattleSystem();
                battleSystem->startBattle(pikachu, dracaufeu);
                window.setView(window.getDefaultView());
            }
        }
    } else { grassTimer = 0.0f; }

    view.setCenter(player.getPosition());
    sf::Vector2f viewSize = view.getSize();
    sf::Vector2f center = view.getCenter();
    if (center.x - viewSize.x / 2.0f < 0) center.x = viewSize.x / 2.0f;
    if (center.y - viewSize.y / 2.0f < 0) center.y = viewSize.y / 2.0f;
    if (center.x + viewSize.x / 2.0f > maxW) center.x = maxW - viewSize.x / 2.0f;
    if (center.y + viewSize.y / 2.0f > maxH) center.y = maxH - viewSize.y / 2.0f;
    view.setCenter(center);
    window.setView(view);
}

void Game::render() {
    window.clear(sf::Color::Black);
    if (currentState == GameState::MainMenu) {
        window.setView(window.getDefaultView());
        if (menuSelection == 0) {
            playText.setFillColor(sf::Color::Yellow);
            quitText.setFillColor(sf::Color::White);
        } else {
            playText.setFillColor(sf::Color::White);
            quitText.setFillColor(sf::Color::Yellow);
        }
        window.draw(titleText);
        window.draw(playText);
        window.draw(quitText);
    }
    else if (currentState == GameState::Exploration) {
        window.setView(view);
        window.clear(sf::Color(20, 20, 30));
        map.draw(window);
        player.draw(window);
    }
    else if (currentState == GameState::Battle && battleSystem != nullptr) {
        window.setView(window.getDefaultView());
        battleSystem->draw(window);
    }
    window.display();
}
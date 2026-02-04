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
      togepi("Togepi", 60, 10),
      noctowl("Noctowl", 110, 22),
      deoxys("Deoxys", 250, 60),
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

    // --- CONFIGURATION POKEMON ---
    pikachu.addMove("Charge", 25, ATTACK);
    pikachu.addMove("Vive-attaque", 35, ATTACK);
    pikachu.addMove("Heal", 25, HEAL);
    if (!pikachu.texture.loadFromFile("assets/textures/ditto_front.png")) std::cerr << "Err Pikachu" << std::endl;
    pikachu.sprite.setTexture(pikachu.texture);
    pikachu.sprite.setScale({3.f, 3.f});
    pikachu.sprite.setPosition({280.f, 180.f});

    dracaufeu.addMove("Griffe", 30, ATTACK);
    dracaufeu.addMove("Lance-Flamme", 40, ATTACK);
    dracaufeu.addMove("Repos", 30, HEAL);
    if (!dracaufeu.texture.loadFromFile("assets/textures/mawile_back.png")) std::cerr << "Err Dracaufeu" << std::endl; 
    dracaufeu.sprite.setTexture(dracaufeu.texture);
    dracaufeu.sprite.setScale({2.5f, 2.5f});
    dracaufeu.sprite.setPosition({700.f, 10.f});

    togepi.addMove("Metronome", 15, ATTACK);
    togepi.addMove("Charme", 10, ATTACK);
    togepi.addMove("Voeu", 40, HEAL);
    if (!togepi.texture.loadFromFile("assets/textures/togepi.png")) std::cerr << "Err Togepi" << std::endl;
    togepi.sprite.setTexture(togepi.texture);
    togepi.sprite.setScale({2.5f, 2.5f});
    togepi.sprite.setPosition({750.f, 20.f});

    noctowl.addMove("Picpic", 25, ATTACK);
    noctowl.addMove("Psyko", 35, ATTACK);
    noctowl.addMove("Atterrissage", 30, HEAL);
    if (!noctowl.texture.loadFromFile("assets/textures/noctowl.png")) std::cerr << "Err Noctowl" << std::endl;
    noctowl.sprite.setTexture(noctowl.texture);
    noctowl.sprite.setScale({2.5f, 2.5f});
    noctowl.sprite.setPosition({725.f, -10.f});

    deoxys.addMove("Psycho Boost", 80, ATTACK);
    deoxys.addMove("Vitesse Extreme", 40, ATTACK);
    deoxys.addMove("Soin", 50, HEAL);
    if (!deoxys.texture.loadFromFile("assets/textures/deoxys.png")) std::cerr << "Err Deoxys" << std::endl;
    deoxys.sprite.setTexture(deoxys.texture);
    deoxys.sprite.setScale({2.5f, 2.5f});
    deoxys.sprite.setPosition({700.f, 20.f});


    // --- AUDIO ---
    // 1. Musique d'exploration
    if (!musicExploration.openFromFile("assets/audio/1-11.-Route-101.ogg")) {
        std::cerr << "ERREUR : Impossible de charger Route-101.ogg !" << std::endl;
    }
    musicExploration.setLooping(true);
    musicExploration.setVolume(50);
    musicExploration.play();

    // 2. Musique de combat (Celle que tu viens d'ajouter)
    if (!musicBattle.openFromFile("assets/audio/battle.ogg")) {
        std::cerr << "ERREUR : Impossible de charger battle.ogg !" << std::endl;
    }
    musicBattle.setLooping(true);
    musicBattle.setVolume(60);


    // --- MAP & JOUEUR ---
    if (!map.load("assets/textures/free_pixel_16_woods.png", "assets/mappokemon_sol.csv", "assets/mappokemon_decord.csv")) {
        if (!map.load("free_pixel_16_woods.png", "mappokemon_sol.csv", "mappokemon_decord.csv")) {
            std::cerr << "Erreur chargement Map" << std::endl;
        }
    }

    if (!player.load("assets/textures/player_sheet.png")) {
        if (!player.load("player_sheet.png")) exit(-1);
    }
    
    player.setPosition({(float)map.getWidth() * 16 / 2.0f, (float)map.getHeight() * 16 / 2.0f});
    
    view = window.getDefaultView();
    view.zoom(currentZoom);

    // --- FONTS ---
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
            
            // --- FIN DU COMBAT ---
            musicBattle.stop();       // On arrête la musique de combat
            musicExploration.play();  // On relance la musique calme

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
                // --- DEBUT DU COMBAT ---
                musicExploration.pause(); // Pause musique calme
                musicBattle.play();       // PLAY musique combat

                currentState = GameState::Battle;
                
                pikachu.restoreHealth();
                dracaufeu.restoreHealth();
                togepi.restoreHealth();
                noctowl.restoreHealth();
                deoxys.restoreHealth();

                Creature* opponent = nullptr;
                
                int luck = std::rand() % 100;
                std::cout << "--------------------" << std::endl;
                std::cout << "Tirage au sort (luck) : " << luck << std::endl;

                if (luck == 0) { 
                    opponent = &deoxys;
                    std::cout << ">>> RESULTAT : DEOXYS choisi !" << std::endl;
                } 
                else {
                    int common = std::rand() % 3;
                    std::cout << "Tirage commun (0-2) : " << common << std::endl;
                    if (common == 0) { opponent = &dracaufeu; std::cout << ">>> RESULTAT : DRACAUFEU choisi !" << std::endl; }
                    else if (common == 1) { opponent = &togepi; std::cout << ">>> RESULTAT : TOGEPI choisi !" << std::endl; }
                    else { opponent = &noctowl; std::cout << ">>> RESULTAT : NOCTOWL choisi !" << std::endl; }
                }
                
                std::cout << "Lancement du combat avec : " << opponent->name << std::endl;
                std::cout << "--------------------" << std::endl;

                battleSystem = new BattleSystem();
                battleSystem->startBattle(pikachu, *opponent);
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
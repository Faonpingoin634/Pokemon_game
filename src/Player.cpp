#include "../include/Player.hpp"

Player::Player() : sprite(texture) {
    charSize = sf::Vector2i(64, 64);
    animFrame = 0;
    animDirection = 0;
    animTimer = 0.0f;
    isMoving = false; // On initialise à l'arrêt
}

bool Player::load(const std::string& texturePath) {
    if (!texture.loadFromFile(texturePath)) return false;
    texture.setSmooth(false);
    
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect({0, 0}, charSize));
    sprite.setOrigin({32.f, 60.f}); 
    sprite.setScale({0.5f, 0.5f});
    return true;
}

// C'est ICI que l'animation se joue !
void Player::update(float dt) {
    if (isMoving) {
        animTimer += dt;
        if (animTimer > 0.15f) { // Change d'image toutes les 0.15s
            animTimer = 0.0f;
            animFrame++;
            if (animFrame > 3) animFrame = 0;
        }
    } else {
        animFrame = 0; // Si on s'arrête, on remet l'image "debout"
    }

    // On met à jour le rectangle de texture
    int tx = animFrame * charSize.x;
    int ty = animDirection * charSize.y;
    sprite.setTextureRect(sf::IntRect({tx, ty}, charSize));
}

void Player::setMoving(bool moving, int direction) {
    isMoving = moving;
    animDirection = direction;
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Player::setPosition(const sf::Vector2f& pos) { sprite.setPosition(pos); }
sf::Vector2f Player::getPosition() const { return sprite.getPosition(); }
void Player::move(const sf::Vector2f& offset) { sprite.move(offset); }
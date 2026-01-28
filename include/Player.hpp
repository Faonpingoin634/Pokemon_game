#pragma once
#include <SFML/Graphics.hpp>

class Player {
public:
    Player();
    bool load(const std::string& texturePath);
    void update(float dt); 
    void draw(sf::RenderWindow& window);
    
    void setPosition(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;
    void move(const sf::Vector2f& offset);

    void setMoving(bool moving, int direction);

private:
    sf::Texture texture;
    sf::Sprite sprite;
    
    sf::Vector2i charSize;
    int animFrame;
    int animDirection;
    float animTimer;
    bool isMoving;
};
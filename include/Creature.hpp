#pragma once
#include <string>
#include <SFML/Graphics.hpp>

class Creature {
public:
    std::string name;
    int healthPoints;
    int maxHealth;
    int attackPower;
    

    
    sf::Texture texture;
    sf::Sprite sprite;

    Creature(std::string newName, int maxHp, int atk);
    
    void takeDamage(int amount);
    bool isAlive() const;
};
#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

// 1. Définition des types
enum MoveType { ATTACK, HEAL };

struct Move {
    std::string name;
    int power;
    MoveType type; // Type de l'attaque
};

class Creature {
public:
    std::string name;
    int healthPoints;
    int maxHealth;
    int attackPower;
    
    std::vector<Move> moves;
    
    sf::Texture texture;
    sf::Sprite sprite;

    Creature(std::string newName, int maxHp, int atk);
    
    void addMove(std::string name, int power, MoveType type);
    
    void takeDamage(int amount);
    void heal(int amount);
    bool isAlive() const;
    void restoreHealth();
};
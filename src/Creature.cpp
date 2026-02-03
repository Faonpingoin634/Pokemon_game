#include "../include/Creature.hpp"

Creature::Creature(std::string newName, int maxHp, int atk) : sprite(texture) {
    name = newName;
    maxHealth = maxHp;
    healthPoints = maxHp;
    attackPower = atk;
}

void Creature::addMove(std::string name, int power, MoveType type) {
    if (moves.size() < 3) {
        moves.push_back({name, power, type});
    }
}

void Creature::takeDamage(int amount) {
    healthPoints -= amount;
    if (healthPoints < 0) {
        healthPoints = 0;
    }
}

void Creature::heal(int amount) {
    healthPoints += amount;
    if (healthPoints > maxHealth) {
        healthPoints = maxHealth;
    }
}

bool Creature::isAlive() const {
    return healthPoints > 0;
}

void Creature::restoreHealth() {
    healthPoints = maxHealth;
}
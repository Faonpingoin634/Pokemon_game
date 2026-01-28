#include "Creature.hpp"

Creature::Creature(std::string newName, int maxHp, int atk) {
    name = newName;
    maxHealth = maxHp;
    healthPoints = maxHp;
    attackPower = atk;
}

void Creature::takeDamage(int amount) {
    healthPoints -= amount;
    if (healthPoints < 0) {
        healthPoints = 0;
    }
}

bool Creature::isAlive() const {
    return healthPoints > 0;
}
#include "potion.h"

class HealthPotion : public Potion {
public:
    using Potion::Potion;

    // Function to apply potions effect
    void applyEffect(Character *character);
};

void HealthPotion::applyEffect(Character *character) {
    int health = character->getHealth();
    
    character->setHealth(health + 20);
}
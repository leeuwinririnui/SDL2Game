#ifndef HEALTH_H
#define HEALTH_H

#include "potion.h"

class HealthPotion : public Potion {
public:
    using Potion::Potion;

    // Function to apply potions effect
    void applyEffect(Character *character);
    void playerUsesPotion(HealthPotion *healthPotion, Player *player, int &healthPotionHolder);
};

#endif
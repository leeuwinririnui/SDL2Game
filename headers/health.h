#ifndef HEALTH_H
#define HEALTH_H

#include "potion.h"

class HealthPotion : public Potion {
public:
    using Potion::Potion;

    // Function to apply potions effect
    void applyEffect(std::unique_ptr<Player>& player);
    void playerUsesPotion(std::unique_ptr<HealthPotion>& healthPotion, std::unique_ptr<Player>& player, int &healthPotionHolder);
};

#endif
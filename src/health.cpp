#include "health.h"

// Apply potions affect to character
void HealthPotion::applyEffect(std::unique_ptr<Player>& player) {
    int health = player->getHealth();
    
    player->setHealth(health + 20);
}

// Handle player colliding with healh potion
void HealthPotion::playerUsesPotion(std::unique_ptr<HealthPotion>& healthPotion, std::unique_ptr<Player>& player, int &healthPotionHolder) {
    if (!healthPotion->getIsVisible()) return;

    SDL_Point potionCenter = healthPotion->getBoundingBoxCenter();
    SDL_Point playerCenter = player->getBoundingBoxCenter();
    
    if (std::abs(potionCenter.x - playerCenter.x) <= 32 && std::abs(potionCenter.y - playerCenter.y) <= 32) {
        player->setRecovering(true);
        healthPotion->applyEffect(player);
        healthPotion->setIsVisible(false);
        healthPotionHolder = -1;
    }
}
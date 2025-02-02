#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

class Player : public Character {
private:
    bool isFlashing = false, isRecovering = false;
    Uint32 lastFlashTime = 0;
    Uint32 lastRecoveryTime = 0;
    Uint32 flashCooldown = 500;

public:
    // Use constructor from Character class
    using Character::Character;

    // Override resetStates method
    void resetStates() override;

    // Methods to handle flashing effect
    void setFlashing(bool isFlashing);
    void setRecovering(bool isRecovering);
    void updateTint(Uint32 currentTime);

    // Getters
    bool getFlashing() const { return isFlashing; }
    bool getRecovering() const { return isRecovering; }
};

// Set flashing state and record timw when flashing starts
void Player::setFlashing(bool isFlashing) { 
    this->isFlashing = isFlashing; 
    lastFlashTime = SDL_GetTicks();
}

void Player::setRecovering(bool isRecovering) {
    this->isRecovering = isRecovering;
    lastRecoveryTime = SDL_GetTicks();
}

// Update tint based on flashing state and elapsed time
void Player::updateTint(Uint32 currentTime) {
    if (currentTime - lastFlashTime >= flashCooldown) {
        isFlashing = false;
    }

    if (currentTime - lastRecoveryTime >= flashCooldown) {
        isRecovering = false;
    }

    if (isFlashing) {
        setRedTint();
    } else if (isRecovering) {
        setBlueTint();
    } else {
        setNoTint();
    }
}

// Reset player's states to default values
void Player::resetStates() {
    lastAttack = 0;
    isAlive = true;
    isRespawning = false;
    isMoving = false;
    isAttacking = false;
    currentFrame = 0;
    direction = 0;
    speed = speed;
    flip = SDL_FLIP_NONE;
    row = idleRows[direction];
    health = 100;
    isFlashing = false;
    updateAnimation();
}

#endif
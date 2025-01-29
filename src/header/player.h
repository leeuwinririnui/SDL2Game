#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

class Player : public Character {
private:
    bool isFlashing = false;
    Uint32 lastFlashTime = 0;
    Uint32 flashCooldown = 500;

public:
    // Use constructor from Character class
    using Character::Character;

    void resetStates() override;

    void setFlashing(bool isFlashing);
    void updateTint(Uint32 currentTime);

    // Getters
    bool getFlashing() const { return isFlashing; }
};

void Player::setFlashing(bool isFlashing) { 
        this->isFlashing = isFlashing; 
        lastFlashTime = SDL_GetTicks();
    }

void Player::updateTint(Uint32 currentTime) {
    if (currentTime - lastFlashTime >= flashCooldown) {
        isFlashing = false;
    }

    if (isFlashing) {
        setRedTint();
    } else {
        setNoTint();
    }
}

void Player::resetStates() {
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
    updateAnimation();
}

#endif
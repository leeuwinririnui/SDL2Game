#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

class Player : public Character {
public:
    // Use constructor from Character class
    using Character::Character;

    void resetStates() override;
};

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
    updateAnimation();
}

#endif
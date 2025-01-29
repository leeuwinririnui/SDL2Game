#ifndef SLIME_H
#define SLIME_H

#include "character.h"

class Slime : public Character {
public:
    // Use constructor from Character class
    using Character::Character;

    void resetStates() override;
};

void Slime::resetStates() {
    isAlive = true;
    isRespawning = false;
    isMoving = false;
    isAttacking = false;
    currentFrame = 0;
    direction = 0;
    speed = (std::rand() % 3) + 1;
    flip = SDL_FLIP_NONE;
    row = idleRows[direction];
    setRandomSpawnPoints();
    updateAnimation();
}

#endif
#ifndef SLIME_H
#define SLIME_H

#include "character.h"

class Slime : public Character {
public:
    // Use constructor from Character class
    using Character::Character;

    // Override resetStates function
    void resetStates() override;
};

// Reset slimes states to default value
void Slime::resetStates() {
    lastAttack = 0;
    isAlive = false;
    isRespawning = false;
    isMoving = false;
    isAttacking = false;
    currentFrame = 4; // TEMP FIX - DEATH ANIMATION PLAYS AT START OF EACH GAME
    direction = 0;
    speed = (std::rand() % 2) + 2;
    flip = SDL_FLIP_NONE;
    row = idleRows[direction];
    setRandomSpawnPoints();
    updateAnimation();
}

#endif
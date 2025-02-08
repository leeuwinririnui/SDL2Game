#ifndef SLIME_H
#define SLIME_H

#include "character.h"

class Player;

class Slime : public Character {
public:
    // Use constructor from Character class
    using Character::Character;

    // Function declarations
    void movement(Player *player, const std::vector<Slime*>& slimes);
    void handleDeath();
    bool isBlocked(int nextX, int nextY, const std::vector<Slime*>& slimes);
    void attack(Player *player, Uint32 currentTime);

    // Override resetStates function
    void resetStates() override;
};

#endif

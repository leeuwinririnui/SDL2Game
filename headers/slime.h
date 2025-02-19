#ifndef SLIME_H
#define SLIME_H

#include "character.h"

class Player;

class Slime : public Character {
public:
    // Use constructor from Character class
    using Character::Character;

    // Function declarations
    void movement(std::unique_ptr<Player>& player, const std::vector<std::unique_ptr<Slime>>& slimes);
    void handleDeath();
    bool isBlocked(int nextX, int nextY, const std::vector<std::unique_ptr<Slime>>& slimes);
    void attack(std::unique_ptr<Player>& player , Uint32 currentTime);

    // Override resetStates function
    void resetStates() override;
};

#endif

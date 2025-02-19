#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include "dust.h"

class Slime;

class Player : public Character {
private:
    bool isFlashing = false, isRecovering = false;
    
    Uint32 lastFlashTime = 0, lastRecoveryTime = 0, flashCooldown = 500;

public:
    // Use constructor and destructor from Character class
    using Character::Character;

    // Override resetStates method
    void resetStates() override;

    // Methods to handle flashing effect
    void setFlashing(bool isFlashing);
    void setRecovering(bool isRecovering);
    void updateTint(Uint32 currentTime);
    void movement(const Uint8 *keyStates, std::unique_ptr<Player>& player, std::unique_ptr<Dust>& dust);
    void attack(const Uint8 *keyStates, std::unique_ptr<Player>& player, Uint32 currentTime);
    void handleDamage(std::unique_ptr<Slime>& slime, int &playerPoints);

    // Getters
    bool getFlashing() const { return isFlashing; }
    bool getRecovering() const { return isRecovering; }
};

#endif
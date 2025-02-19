#include "player.h"
#include "slime.h"

// Set flashing state and record time when flashing starts
void Player::setFlashing(bool isFlashing) { 
    this->isFlashing = isFlashing; 
    lastFlashTime = SDL_GetTicks();
}

// Set recovering state and record time when recovering starts
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
    speed = 6;
    flip = SDL_FLIP_NONE;
    row = idleRows[direction];
    health = 100;
    isFlashing = false;
    updateAnimation();
}

// Handle player movement based on keyboard input
void Player::movement(const Uint8 *keyStates, std::unique_ptr<Player>& player, std::unique_ptr<Dust>& dust) {
    bool isMoving = false;

    SDL_Point playerCenter = player->getBoundingBoxCenter();

    // Only allow if player is not attacking
    if (!player->getAttacking()) {
        if (keyStates[SDL_SCANCODE_UP] && !keyStates[SDL_SCANCODE_DOWN]) {
            player->setDirection(Up); 
            if (posY > minY) {
                player->setPosY(posY - speed); // Move up
                isMoving = true;
            }
            dust->setIsVertical(true);
            dust->setFlip(SDL_FLIP_NONE);
            dust->setPosition(playerCenter.x - 12, posY + 100);
        }

        if (keyStates[SDL_SCANCODE_DOWN] && !keyStates[SDL_SCANCODE_UP]) {
            player->setDirection(Down); 
            if (posY < maxY) {
                player->setPosY(posY + speed); // Move down
                isMoving = true;
            }
            dust->setIsVertical(true);
            dust->setFlip(SDL_FLIP_HORIZONTAL);
            dust->setPosition(playerCenter.x - 36, posY + 35);
        }

        if (keyStates[SDL_SCANCODE_LEFT] && !keyStates[SDL_SCANCODE_RIGHT]) {
            player->setDirection(Left); 
            if (posX > minX) {
                player->setPosX(posX - speed); // Move left
                isMoving = true;
            }
            dust->setIsVertical(false);
            dust->setFlip(SDL_FLIP_NONE);
            dust->setPosition(playerCenter.x + speed, posY + 85);
        }

        if (keyStates[SDL_SCANCODE_RIGHT] && !keyStates[SDL_SCANCODE_LEFT]) {
            player->setDirection(Right); 
            if (posX < maxX) {
                player->setPosX(posX + speed); // Move right
                isMoving = true;
            }
            dust->setIsVertical(false);
            dust->setFlip(SDL_FLIP_HORIZONTAL);
            dust->setPosition(playerCenter.x - player->getFrameWidth() - speed, posY + 85);
        }
    }

    player->setMoving(isMoving); // Update the player's moving state
}

// Handle player attacks
void Player::attack(const Uint8 *keyStates, std::unique_ptr<Player>& player, Uint32 currentTime) {
    if (!player->getAttacking() && keyStates[SDL_SCANCODE_SPACE] && currentTime - player->getLastAttack() >= player->getAttackCooldown()) {
        int playerDirection = player->getDirection();
        int attackDistance = 30;
        int posX = player->getPosX();
        int posY = player->getPosY();
        int minX = player->getMinX();
        int maxX = player->getMaxX();
        int minY = player->getMinY();
        int maxY = player->getMaxY();

        // Move the player slightly in the attack direction
        switch (playerDirection) {
            case Down: 
                if (posY + attackDistance <= maxY) {
                    player->setPosY(posY + attackDistance);
                }
                break;
            case Right: 
                if (posX + attackDistance <= maxX) {
                    player->setPosX(posX + attackDistance);
                }
                break;
            case Up: 
                if (posY - attackDistance >= minY) {
                    player->setPosY(posY - attackDistance);
                }
                break;
            case Left: 
                if (posX - attackDistance >= minX) {
                    player->setPosX(posX - attackDistance);
                }
                break;
        }

        // Set player's attack state
        player->setAttacking(true);
        player->setCurrentFrame(0);
        player->setLastAttack(currentTime);
    }

    // End attack when animation completes
    if (player->getAttacking() && player->getCurrentFrame() == 3) {
        player->setAttacking(false);
        player->setCurrentFrame(0);
    }
}

// Handle damage dealt
void Player::handleDamage(std::unique_ptr<Slime>& slime, int &playerPoints) {
    if (isAttacking && slime->getAlive() && currentFrame < 2) {
        int newHealth = slime->getHealth() - 20;
        slime->setHealth(newHealth); // Kill the slime
        slime->setCurrentFrame(0);
        slime->setAlive(false);
        playerPoints += 10;
    }
}
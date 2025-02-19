#include "slime.h"
#include "player.h"

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

// Check is path is blocked by other slimes
bool Slime::isBlocked(int nextX, int nextY, const std::vector<std::unique_ptr<Slime>>& slimes) {
    for (const std::unique_ptr<Slime>& otherSlimePtr : slimes) {
        if (otherSlimePtr.get() == this) continue;

        SDL_Rect otherBox = otherSlimePtr->getBoundingBox();
        SDL_Rect nextBox = { nextX, nextY, frameWidth,frameHeight };

        // Check if the next position intersects with another another slime's bounding box
        if (SDL_HasIntersection(&nextBox, &otherBox)) {
            return true;
        }
    }
    
    return false;
}

// Handle movement for slime
void Slime::movement(std::unique_ptr<Player>& player, const std::vector<std::unique_ptr<Slime>>& slimes) {
    isMoving = true;

    SDL_Point playerCenter = player->getBoundingBoxCenter();
    SDL_Point slimeCenter = getBoundingBoxCenter();

    // Calculate the difference between the slime and player positions
    int deltaX = playerCenter.x - slimeCenter.x;
    int deltaY = playerCenter.y - slimeCenter.y;
    
    int range = 10;
    
    // Move the slime towards the player
    if (slimeCenter.y < playerCenter.y - range) {
        int moveY = std::min(speed, deltaY); // Move down
        if (!isBlocked(posX, posY + moveY, slimes)) {
            posY += moveY;
            direction = Down; 
        }
    } else if (slimeCenter.y > playerCenter.y + range) {
        int moveY = std::min(speed, -deltaY); // Move up
        if (!isBlocked(posX, posY - moveY, slimes)) {
            posY -= moveY;
            direction = Up; 
        }
    }

    if (slimeCenter.x < playerCenter.x - range) {
        int moveX = std::min(speed, deltaX); // Move right
        if (!isBlocked(posX + moveX, posY, slimes)) {
            posX += speed;
            direction = Right; 
        }
    } else if (slimeCenter.x > playerCenter.x + range) {
        int moveX = std::min(speed, -deltaX); // Move left
        if (!isBlocked(posX - moveX, posY, slimes)) {
            posX -= speed;
            direction = Left; 
        }
    }
}

// Handle slime attacks
void Slime::attack(std::unique_ptr<Player>& player, Uint32 currentTime) {
    if (!isAttacking) {
        isAttacking = true;
        currentFrame = 0;
        speed += 1;
    } else if (isAttacking && currentFrame == 0) {
        isAttacking = false;
        speed -= 1;
    }

    SDL_Point playerCenter = player->getBoundingBoxCenter();
    SDL_Point slimeCenter = getBoundingBoxCenter();

    //  Check if slime can attack player
    if (currentTime - lastAttack >= attackCooldown  && player->getAlive()) {
        if (std::abs(slimeCenter.x - playerCenter.x) <= 30 && std::abs(slimeCenter.y - playerCenter.y) <= 30) {
            player->setFlashing(true);
            player->setHealth(player->getHealth() - 20); // Reduce player health
            lastAttack = currentTime;

            if (player->getHealth() <= 0) {
                player->setAlive(false); // Kill the player if health drops to 0
                player->setCurrentFrame(0);
            }
        }
    }
}

// Handle slime death
void Slime::handleDeath() {
    isMoving = false;

    // Move the slime off-screen when the death animation is finished
    if (currentFrame == 4) {
        posX = -1000; 
    }
}
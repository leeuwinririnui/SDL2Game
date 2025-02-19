#include "potion.h"

// Set x and y position of potion
void Potion::setPosition(Potion* potion, std::vector<std::unique_ptr<Slime>>& slimes, int holder) {
    if (holder != -1 && !slimes[holder]->getAlive() && !slimes[holder]->getRespawning()) {
        int posX = slimes[holder]->getPosX();
        int posY = slimes[holder]->getPosY();

        setPosition(posX, posY);
        setIsVisible(true);
    }
}

// Spawn heath potion with a 1 in 10 chance
void Potion::spawnPotion(std::unique_ptr<Player>& player, std::vector<std::unique_ptr<Slime>>& slimes, int &holder) {
    if (holder != -1) return;

    // 1 in 10 chance to spawn potion
    if (rand() % 10 != 0) return;

    // Give to first alive
    for (int i = 0; i < slimes.size(); i++) {
        if (slimes[i]->getAlive()) {
            holder = i;
            break;
        }
    }
}

// Load image into texture
bool Potion::loadMedia() {
    if (!texture.loadFromFile(path.c_str())) {
        std::cout << "Failed to load texture image " << path.c_str() << std::endl;
        return false;
    }
    return true;
}

SDL_Point Potion::getBoundingBoxCenter() {
    SDL_Rect boundingBox = getBoundingBox();
    SDL_Point center;
    center.x = boundingBox.x + boundingBox.w / 2; 
    center.y = boundingBox.y + boundingBox.h / 2; 
    return center;
}

// Reset potion states
void Potion::resetStates() {
    isVisible = false;
}
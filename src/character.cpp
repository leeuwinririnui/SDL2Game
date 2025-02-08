#include "character.h"

// Constructor
Character::Character(int health, LTexture &texture, const std::string &path, std::vector<SDL_Rect> &clips, 
    int frameWidth, int frameHeight, int row, int offset, int direction, int currentFrame,
    bool isMoving, bool isAttacking, SDL_RendererFlip flip, int scale, int posX, int posY,
    int frameTime, bool isAlive, int padding, int attackCooldown, int speed) 
    : health(health), texture(texture), path(path), clips(clips), frameWidth(frameWidth), 
    frameHeight(frameHeight), row(row), offset(offset), direction(direction), 
    currentFrame(currentFrame), isMoving(isMoving), isAttacking(isAttacking), flip(flip), 
    scale(scale), posX(posX), posY(posY), frameTime(frameTime), isAlive(isAlive), 
    padding(padding), attackCooldown(attackCooldown), speed(speed) {}

// Get bounding box of character
SDL_Rect Character::getBoundingBox() const {
    SDL_Rect boundingBox = {
        posX + padding,
        posY + padding,
        (frameWidth * scale) - (2 * padding),
        (frameHeight * scale) - (2 * padding)
    };
    return boundingBox;
}

// Get center of bounding box
SDL_Point Character::getBoundingBoxCenter() {
    SDL_Rect boundingBox = getBoundingBox();
    SDL_Point center;
    center.x = boundingBox.x + boundingBox.w / 2; 
    center.y = boundingBox.y + boundingBox.h / 2; 
    return center;
}

// Load character's texture from file
bool Character::loadMedia() {
    if (!texture.loadFromFile(path.c_str())) {
        std::cout << "Failed to load texture image " << path.c_str() << std::endl;
        return false;
    }
    return true;
}

// Set up sprite clips for animation
void Character::spriteClip() {
    int x = 0;
    int y = offset + row * frameHeight;

    for (int i = 0; i < clips.size(); i++) {
        clips[i].x = x;
        clips[i].y = y;
        clips[i].w = frameWidth;
        clips[i].h = frameHeight;
        x += frameWidth;
    }
}

// Update animation based on the character's state
void Character::updateAnimation() {
    if (direction == Left) {
        flip = SDL_FLIP_HORIZONTAL;
    } else {
        flip = SDL_FLIP_NONE;
    }

    // Choose row based on state
    if (isAttacking && isAlive) {
        row = attackRows[direction];
    } else if (isMoving && !isAttacking && isAlive) {
        row = moveRows[direction];
    } else if (!isAlive) {
        row = deathRow;
    } else {
        row = idleRows[direction];
    }
}

// Spawn character at a specific position
void Character::spawn(int spawnX, int spawnY) {
    posX = spawnX;
    posY = spawnY;
    isMoving = false;
    isAttacking = false;
    isAlive = true;
    currentFrame = 0;
    updateAnimation();
}

// Start respawn timer
void Character::respawnTimer() {
    if (isAlive || isRespawning) return;
    
    isRespawning = true;
    respawnStartTime = SDL_GetTicks();
    respawnDelay = 2500;
}

// Check if character should respawn
void Character::checkRespawn() {
    if (!isAlive && isRespawning) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - respawnStartTime >= respawnDelay) {
            resetStates();
            setRandomSpawnPoints();
            spawn(getPosX(), getPosY());
        }
    }
}

// Set random spawn points for the character
void Character::setRandomSpawnPoints() {
    int randomX;
    int randomY;

    // Randomly choose one of four sides
    int side = rand() % 4;

    switch (side) {
        case Down: // Bottom
            randomX = rand() % 1000; // X: 0-999
            randomY = rand() % 600 + 700; // Y: 700-1299
            break;
            
        case Right: // Right
            randomX = rand() % 500 + 1000; // X: 1000-1499
            randomY = rand() % 600; // Y: 0-599
            break;
            
        case Up: // Top
            randomX = rand() % 1000; // X: 0-999
            randomY = rand() % 500 + -800; // Y: -800 to -301
            break;
            
        case Left: // Left
            randomX = rand() % 500 + -1000; // X: -1000 to -501
            randomY = rand() % 600; // Y: 0-599
            break;
    }    

    setPosX(randomX);
    setPosY(randomY);
}

// Render character to the screen
void Character::render(SDL_Renderer *renderer, std::vector<SDL_Rect> clips) {
    texture.render(
        posX,
        posY,
        &clips[currentFrame],
        scale,
        flip
    );
}
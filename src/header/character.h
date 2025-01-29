#ifndef CHARACTER
#define CHARACTER

#include "texture.h"

class Character {
protected:
    int health;
    std::string path;
    std::vector<SDL_Rect> &clips;

    int frameWidth, frameHeight, row, offset, direction, currentFrame;
    bool isMoving, isAttacking, isAlive, isRespawning = false;

    SDL_RendererFlip flip;
    int scale, posX, posY, frameTime, padding; 
    float speed;

    // Boundaries
    int minX, maxX, minY, maxY;

    Uint32 lastFrame = 0, respawnStartTime;
    int respawnDelay;

    // Animation rows
    std::vector<int> idleRows, moveRows, attackRows;
    int deathRow;

public:
    LTexture& texture;

    // Constructor
    Character(int health, LTexture &texture, const std::string &path, std::vector<SDL_Rect> &clips, 
            int frameWidth, int frameHeight, int row, int offset, int direction, int currentFrame,
            bool isMoving, bool isAttacking, SDL_RendererFlip flip, int scale, int posX, int posY,
            int frameTime, bool isAlive, int padding, int speed) 
    : health(health), texture(texture), path(path), clips(clips), frameWidth(frameWidth), 
      frameHeight(frameHeight), row(row), offset(offset), direction(direction), 
      currentFrame(currentFrame), isMoving(isMoving), isAttacking(isAttacking), flip(flip), 
      scale(scale), posX(posX), posY(posY), frameTime(frameTime), isAlive(isAlive), 
      padding(padding), speed(speed) {}


    // Methods
    virtual void resetStates() {}
    
    void updateLastFrame() { lastFrame = SDL_GetTicks(); }
    Uint32 getLastFrame() { return lastFrame; }

    void spawn(int spawnX, int spawnY);
    void respawnTimer();
    void checkRespawn();
    
    // Bounding box calculations
    SDL_Rect getBoundingBox() const;
    SDL_Point getBoundingBoxCenter();

    // Methods to calculate center
    int getCenterX() const { return posX + frameWidth / 2; }
    int getCenterY() const { return posY + frameHeight / 2; }

    bool loadMedia();
    void spriteClip();
    void updateAnimation();
    void setRandomSpawnPoints();

    // Boundaries getters and setters
    int getMinX() const { return minX; }
    void setMinX(int x) { minX = x; }

    int getMinY() const { return minY; }
    void setMinY(int y) { minY = y; }

    int getMaxX() const { return maxX; }
    void setMaxX(int x) { maxX = x; }

    int getMaxY() const { return maxY; }
    void setMaxY(int y) { maxY = y; }

    // Setters
    void setHealth(int health) { this->health = health; }
    void setPath(const std::string& path) { this->path = path; }
    void setClips(std::vector<SDL_Rect> &clips) { this->clips = clips; }
    void setFrameWidth(int frameWidth) { this->frameWidth = frameWidth; }
    void setFrameHeight(int frameHeight) { this->frameHeight = frameHeight; }
    void setRow(int row) { this->row = row; }
    void setOffset(int offset) { this->offset = offset; }
    void setDirection(int direction) { this->direction = direction; }
    void setCurrentFrame(int currentFrame) { this->currentFrame = currentFrame; }
    void setMoving(bool isMoving) { this->isMoving = isMoving; }
    void setAttacking(bool isAttacking) { this->isAttacking = isAttacking; }
    void setFlip(SDL_RendererFlip flip) { this->flip = flip; }
    void setScale(int scale) { this->scale = scale; }
    void setPosX(int posX) { this->posX = posX; }
    void setPosY(int posY) { this->posY = posY; }
    void setFrameTime(int frameTime) { this->frameTime = frameTime; }
    void setAlive(bool isAlive) { this->isAlive = isAlive; }
    void setIsRespawning(bool isRespawning) { this->isRespawning = isRespawning; }
    void setSpeed(int speed) { this->speed = speed; }
    void setIdleRows(std::vector<int> rows) { idleRows = rows; }
    void setMoveRows(std::vector<int> rows) { moveRows = rows; }
    void setAttackRows(std::vector<int> rows) { attackRows = rows; }
    void setDeathRow(int deathRow) { this->deathRow = deathRow; }

    // Getters
    int getHealth() const { return health; }
    const std::string& getPath() const { return path; }
    std::vector<SDL_Rect>& getClips() { return clips; }
    int getFrameWidth() const { return frameWidth; }
    int getFrameHeight() const { return frameHeight; }
    int getRow() const { return row; }
    int getOffset() const { return offset; }
    int getDirection() const { return direction; }
    int getCurrentFrame() const { return currentFrame; }
    bool getMoving() const { return isMoving; }
    bool getAttacking() const { return isAttacking; }
    SDL_RendererFlip getFlip() const { return flip; }
    int getScale() const { return scale; }
    int getPosX() const { return posX; }
    int getPosY() const { return posY; }
    int getFrameTime() const { return frameTime; }
    bool getAlive() const { return isAlive; }
    bool getRespawning() const { return isRespawning; }
    int getSpeed() const { return speed; }
    const std::vector<int>& getIdleRows() const { return idleRows; }
    const std::vector<int>& getMoveRows() const { return moveRows; }
    const std::vector<int>& getAttackRows() const { return attackRows; }
    int getDeathRow() const { return deathRow; }
};

// Method implementations
SDL_Rect Character::getBoundingBox() const {
        SDL_Rect boundingBox = {
            posX + padding,
            posY + padding,
            (frameWidth * scale) - (2 * padding),
            (frameHeight * scale) - (2 * padding)
        };
        return boundingBox;
    }

SDL_Point Character::getBoundingBoxCenter() {
    SDL_Rect boundingBox = getBoundingBox();
    SDL_Point center;
    center.x = boundingBox.x + boundingBox.w / 2; 
    center.y = boundingBox.y + boundingBox.h / 2; 
    return center;
}

bool Character::loadMedia() {
    if (!texture.loadFromFile(path.c_str())) {
        std::cout << "Failed to load texture image " << path.c_str() << std::endl;
        return false;
    }
    return true;
}

void Character::spriteClip() {
    int xCordinate = 0;
    int yCordinate = offset + row * frameHeight;

    for (int i = 0; i < clips.size(); i++) {
        clips[i].x = xCordinate;
        clips[i].y = yCordinate;
        clips[i].w = frameWidth;
        clips[i].h = frameHeight;
        xCordinate += frameWidth;
    }
}

void Character::updateAnimation() {
    if (direction == 3) {
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

void Character::spawn(int spawnX, int spawnY) {
    posX = spawnX;
    posY = spawnY;
    isMoving = false;
    isAttacking = false;
    currentFrame = 0;
    updateAnimation();
}

void Character::respawnTimer() {
    if (isAlive || isRespawning) return;
    
    isRespawning = true;
    respawnStartTime = SDL_GetTicks();
    respawnDelay = 5000 + (rand() % 5001);
}

void Character::checkRespawn() {
    if (!isAlive && isRespawning) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - respawnStartTime >= respawnDelay) {
            resetStates();
            setRandomSpawnPoints();
            spawn(getPosX(), getPosY());
            std::cout << "Spawned" << std::endl;
        }
    }
}

void Character::setRandomSpawnPoints() {
    int randomX;
    int randomY;

    // Randomly choose one of four sides
    int side = rand() % 4;

    switch (side) {
        case 0: // Bottom
            randomX = rand() % 1000;
            randomY = rand() % 600 + 700;
            break;
        case 1: // Right
            randomX = rand() % 500 + 1000;
            randomY = rand() % 600;
            break;
        case 2: // Top
            randomX = rand() % 1000;
            randomY = rand() % 500 + -800;
            break;
        case 3: // Left
            randomX = rand() % 500 + -1000;
            randomY = rand() % 600;
            break;
    }

    setPosX(randomX);
    setPosY(randomY);
}

#endif

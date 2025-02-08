#ifndef CHARACTER_H
#define CHARACTER_H

#include "texture.h"

// Enumarator to track directions
enum Direction {
    Down,
    Right, 
    Up, 
    Left
};

class Character {
protected:
    int health;
    std::string path;
    std::vector<SDL_Rect> &clips;

    int frameWidth, frameHeight, row, offset, direction, currentFrame;
    bool isMoving, isAttacking, isAlive, isRespawning = false;

    SDL_RendererFlip flip;
    int scale, posX, posY, padding, attackCooldown, lastAttack = 0; 
    int speed;

    // Boundaries
    int minX, maxX, minY, maxY;

    // Timing variables
    Uint32 lastFrame = 0, respawnStartTime;
    int respawnDelay, frameTime;

    // Animation rows
    std::vector<int> idleRows, moveRows, attackRows;
    int deathRow;

public:
    LTexture &texture;

    // Constructor
    Character(int health, LTexture &texture, const std::string &path, std::vector<SDL_Rect> &clips, 
        int frameWidth, int frameHeight, int row, int offset, int direction, int currentFrame,
        bool isMoving, bool isAttacking, SDL_RendererFlip flip, int scale, int posX, int posY,
        int frameTime, bool isAlive, int padding, int attackCooldown, int speed);
            
    // Functions to set color tint  
    void setRedTint() { SDL_SetTextureColorMod(texture.getSDLTexture(), 255, 100, 100); }
    void setBlueTint() { SDL_SetTextureColorMod(texture.getSDLTexture(), 100, 100, 255); }
    void setNoTint() { SDL_SetTextureColorMod(texture.getSDLTexture(), 255, 255, 255); }

    // Methods
    virtual void resetStates() {}
    void render(SDL_Renderer *renderer, std::vector<SDL_Rect> clips);
    void updateLastFrame() { lastFrame = SDL_GetTicks(); }
    Uint32 getLastFrame() { return lastFrame; }
    void spawn(int spawnX, int spawnY);
    void respawnTimer();
    void checkRespawn();
    bool loadMedia();
    void spriteClip();
    void updateAnimation();
    void setRandomSpawnPoints();
    
    // Bounding box calculations
    SDL_Rect getBoundingBox() const;
    SDL_Point getBoundingBoxCenter();

    // Methods to calculate center
    int getCenterX() const { return posX + frameWidth / 2; }
    int getCenterY() const { return posY + frameHeight / 2; }

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
    void setAttackCooldown(int attackCooldown) { this->attackCooldown = attackCooldown; }
    void setLastAttack(int lastAttack) { this->lastAttack = lastAttack; }
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
    int getAttackCooldown() const { return attackCooldown; }
    int getLastAttack() const { return lastAttack; }
    int getSpeed() const { return speed; }
    const std::vector<int>& getIdleRows() const { return idleRows; }
    const std::vector<int>& getMoveRows() const { return moveRows; }
    const std::vector<int>& getAttackRows() const { return attackRows; }
    int getDeathRow() const { return deathRow; }
};

#endif

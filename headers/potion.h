#ifndef POTION_H
#define POTION_H

#include "texture.h"
#include "player.h"
#include "slime.h"
#include <string>

class Potion {
private:
    std::string path;
    std::string name;
    int points;
    bool isVisible = false;
    
    // Position and dimensions
    int x, y, width, height;
    
public:
    LTexture &texture;

    Potion(LTexture &texture, std::string path, std::string name, int points) 
    : texture(texture), path(path), name(name), points(points) {}

    // Methods
    void setPosition(Potion *healthPotion, std::vector<Slime*> &slimes, int healthPotionHolder);
    void spawnPotion(Player *player, std::vector<Slime*> &slimes, int &holder);
    SDL_Rect getBoundingBox() { return { x, y, width, height }; }
    SDL_Point getBoundingBoxCenter();
    bool loadMedia();
    void resetStates();

    // Getters
    std::string getPath() const { return path; }
    std::string getName() const { return name; }
    int getPoints() const { return points; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getIsVisible() const { return isVisible; }

    // Setters
    void setPath(const std::string &path) { this->path = path; }
    void setName(const std::string &name) { this->name = name; }
    void setPoints(int points) { this->points = points; }
    void setPosition(int x, int y) { this->x = x; this->y = y; }
    void setSize(int width, int height) { this->width = width; this->height = height; }
    void setIsVisible(bool isVisible) { this->isVisible = isVisible; }
};

#endif
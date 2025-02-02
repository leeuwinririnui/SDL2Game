#include "texture.h"
#include "player.h"
#include <string>

class Potion {
private:
    std::string path;
    std::string name;
    int points;
    bool isVisible = false;
    
    // Position and size
    int x, y, width, height;
    
public:
    LTexture &texture;

    Potion(LTexture &texture, std::string path, std::string name, int points) 
    : texture(texture), path(path), name(name), points(points) {}

    // Methods
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
#ifndef DUST_H
#define DUST_H

#include "texture.h"

class Dust {
private:
    // Postion and dimensions
    int x, y, width, height, scale;
    
    // Path to png file
    std::string path;

    SDL_RendererFlip flip;
    bool isVertical;

    // Timing variable
    Uint32 lastFrame = 0;
    int frameTime = 150, currentFrame = 0;
    
public:
    LTexture &texture;

    Dust(LTexture &texture, int x, int y, int width, int height, int scale, std::string path)
    : texture(texture), x(x), y(y), width(width), height(height), scale(scale), path(path) {}

    // Methods
    bool loadMedia();
    void clipDust(std::vector<SDL_Rect> &clips);
    void render(SDL_Renderer *renderer, std::vector<SDL_Rect> clips);
    
    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getScale() const { return scale; }
    Uint32 getLastFrame() const { return lastFrame; }
    int getFrameTime() const { return frameTime; }
    int getCurrentFrame() const { return currentFrame; }
    bool getIsVertical() const { return isVertical; }
    SDL_RendererFlip getFlip() const { return flip; }
    std::string getPath() const { return path; }

    // Setters
    void setPosition(int x, int y) { this->x = x; this->y = y; }
    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    void setWidth(int width) { this->width = width; }
    void setHeight(int height) { this->height = height; }
    void setScale(int scale) { this->scale = scale; }
    void setLastFrame(Uint32 lastFrame) { this->lastFrame = lastFrame; }
    void setFrameTime(int frameTime) { this->frameTime = frameTime; }
    void setCurrentFrame(int frame) { this->currentFrame = frame; }
    void setIsVertical(bool isVertical) { this->isVertical = isVertical; }
    void setFlip(SDL_RendererFlip flip) { this->flip = flip; }
    void setPath(const std::string &path) { this->path = path; }
};

#endif
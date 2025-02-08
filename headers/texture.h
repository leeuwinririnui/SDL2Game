#ifndef TEXTURE_H
#define TEXTURE_H

#include "includes.h"

class LTexture {
private:
    SDL_Renderer* gameRenderer;
    
    // The actual hardware texture
    SDL_Texture* mTexture;

    // Image dimensions
    int mWidth;
    int mHeight;
    
public:
    // Initialize variables
    LTexture(SDL_Renderer*& gameRenderer);

    // Deallocate memory
    ~LTexture();

    // Get texture
    SDL_Texture *getSDLTexture() { return mTexture; }

    // Set game renderer
    void setRenderer(SDL_Renderer *&gameRenderer);

    // Loads image 
    bool loadFromFile(std::string path);

    // Deallocate texture
    void free();

    // Renders texture at given point
    void render(int x, int y, SDL_Rect *clip = nullptr, int scale = 1, SDL_RendererFlip flip = SDL_FLIP_NONE, bool isVertical = false);

    // Get image dimensions
    int getWidth();
    int getHeight();
};

#endif
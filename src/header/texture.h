#ifndef TEXTURE
#define TEXTURE

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
    void setRenderer(SDL_Renderer*& gameRenderer);

    // Loads image 
    bool loadFromFile(std::string path);

    // Deallocate texture
    void free();

    // Renders texture at given point
    void render(int x, int y, SDL_Rect* clip = nullptr, int scale = 1, SDL_RendererFlip flip = SDL_FLIP_NONE);

    // Get image dimensions
    int getWidth();
    int getHeight();
};

// Constructor
LTexture::LTexture(SDL_Renderer*& gameRenderer) {
    this->gameRenderer = gameRenderer;
    this->mTexture = nullptr;
    this->mWidth = 0;
    this->mHeight = 0;
}

// Destructor implementation
LTexture::~LTexture() {
    free();
}

// Set renderer for texture
void LTexture::setRenderer(SDL_Renderer*& gameRenderer) {
    this->gameRenderer = gameRenderer;
}

// Load image from a file and create a texture
bool LTexture::loadFromFile(std::string path) {
    // Free existing texture
    free();

    // Temporary pointer for new texture
    SDL_Texture* newTexture = nullptr;

    // Load image into surface
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        std::cout << "Unable to load image " << path.c_str() << "! SDL image Error: " << IMG_GetError() 
                  << std::endl; 
        return false;
    }

    // Set color key for transparency (e.g., remove cyan background)
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format,0,0xFF,0xFF));

    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(gameRenderer, loadedSurface);

    if (!newTexture) {
        std::cout << "Unable to create texture from " << path.c_str() << "! SDL Error: " << SDL_GetError() 
                  << std::endl; 
        SDL_FreeSurface(loadedSurface);
        return false;
    }

    // Store dimensions of loaded image
    mWidth = loadedSurface->w;
    mHeight = loadedSurface->h;

    // Free surface as it is no longer needed
    SDL_FreeSurface(loadedSurface);

    // Assign new texture to member variable
    mTexture = newTexture;
    return mTexture != nullptr;
}

// Free texture and reset dimensions
void LTexture::free() {
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
        mWidth = 0;
        mHeight = 0;
    }
}

// Render texture at specified position with optional parameters
void LTexture::render(int x, int y, SDL_Rect* clip, int scale,SDL_RendererFlip flip) {
    // Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, mWidth * scale, mHeight * scale };

    if (clip) {
        renderQuad.w = clip->w * scale;
        renderQuad.h = clip->h * scale;
    }

    SDL_RenderCopyEx(gameRenderer, mTexture, clip, &renderQuad, 0.0, nullptr, flip);
}

int LTexture::getWidth() { return mWidth; }
int LTexture::getHeight() { return mHeight; }

#endif
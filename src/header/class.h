#include "includes.h"

class LTexture {
    public:
        // Initialize variables
        LTexture(SDL_Renderer*& gameRenderer);

        // Deallocate memory
        ~LTexture();

        // Set game renderer
        void setRenderer(SDL_Renderer*& gameRenderer);

        // Loads image 
        bool loadFromFile(std::string path);

        // Deallocate texture
        void free();

        // Renders texture at given point
        void render(
            int x, 
            int y, 
            SDL_Rect* clip = nullptr, 
            int scale = 1
        );

        // Get image dimensions
        int getWidth();
        int getHeight();

    private:
        SDL_Renderer* gameRenderer;
        
        // The actual hardware texture
        SDL_Texture* mTexture;

        // Image dimensions
        int mWidth;
        int mHeight;
};

LTexture::LTexture(SDL_Renderer*& gameRenderer) {
    this->gameRenderer = gameRenderer;
    this->mTexture = nullptr;
    this->mWidth = 0;
    this->mHeight = 0;
}

LTexture::~LTexture() {
    free();
}

void LTexture::setRenderer(SDL_Renderer*& gameRenderer) {
    this->gameRenderer = gameRenderer;
}

bool LTexture::loadFromFile(std::string path) {
    free();

    SDL_Texture* newTexture = nullptr;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        std::cout << "Unable to load image " << path.c_str() 
                  << "! SDL image Error: " << IMG_GetError() 
                  << std::endl; 
        return false;
    }

    SDL_SetColorKey(
        loadedSurface,
        SDL_TRUE,
        SDL_MapRGB(
            loadedSurface->format,
            0,
            0xFF,
            0xFF    
        )
    );

    // Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(
        gameRenderer,
        loadedSurface
    );

    if (!newTexture) {
        std::cout << "Unable to create texture from " << path.c_str() 
                  << "! SDL Error: " << SDL_GetError() << std::endl; 
        SDL_FreeSurface(loadedSurface);
        return false;
    }

    mWidth = loadedSurface->w;
    mHeight = loadedSurface->h;

    SDL_FreeSurface(loadedSurface);

    mTexture = newTexture;
    return mTexture != nullptr;
}

void LTexture::free() {
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(
    int x,
    int y, 
    SDL_Rect* clip, 
    int scale
) {
    // Set rendering space and render to screen
    SDL_Rect renderQuad = { 
        x, 
        y, 
        mWidth * scale, 
        mHeight * scale
    };

    if (clip) {
        renderQuad.w = clip->w * scale;
        renderQuad.h = clip->h * scale;
    }

    SDL_RenderCopy(gameRenderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth() {
    return mWidth;
}

int LTexture::getHeight() {
    return mHeight;
}
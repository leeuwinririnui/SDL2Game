#include "includes.h"

// Initialize SDL and create window
bool init(
    SDL_Window*& gameWindow, 
    SDL_Renderer*& gameRenderer,
    std::string title,
    int SCREEN_WIDTH,
    int SCREEN_HEIGHT
) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not intiialize! SDL Error: " 
                  << SDL_GetError() << std::endl;
        return false;
    }

    // Set window flags and create window
    int winFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;
    gameWindow = SDL_CreateWindow(
        title.c_str(), 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, 
        SCREEN_HEIGHT, 
        winFlags
    );
    
    if (!gameWindow) {
        std::cout << "Window could not be created! SDL Error: " 
                  << SDL_GetError() << std::endl;
        SDL_Quit();
        return false; 
    }

    // Create renderer
    gameRenderer = SDL_CreateRenderer(
        gameWindow, 
        -1, 
        SDL_RENDERER_ACCELERATED
    );

    if (!gameRenderer) {
        std::cout << "Renderer could not be created! SDL Error: "
                  << SDL_GetError() << std::endl;
        SDL_DestroyWindow(gameWindow);
        SDL_Quit();
        return false; 
    }

    SDL_SetRenderDrawColor(gameRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Initialize SDL_Image with PNG and JPG loading
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) &imgFlags)) {
        std::cout << "SDL_image could not be initialized! SDL Error: " 
                  << IMG_GetError() << std::endl;

        SDL_DestroyWindow(gameWindow);
        SDL_Quit();
        return false;
    }
    
    return true;
}

// Free media and shut down SDL
void close(
    SDL_Texture*& gameTexture, 
    SDL_Renderer*& gameRenderer, 
    SDL_Window*& gameWindow
) {
    SDL_DestroyTexture(gameTexture);
    gameTexture = nullptr;

    SDL_DestroyWindow(gameWindow);
    gameWindow = nullptr;

    IMG_Quit();
    SDL_Quit();
}
#include "events.h"
#include "init.h"
#include "class.h"
#include <vector>

bool loadMedia(LTexture& gameTexture, std::string path);
void playerClip(std::vector<SDL_Rect>& clips);

int main(int argc, char *argv[]) {
    // Initialze game variables
    int SCREEN_WIDTH = 800;
    int SCREEN_HEIGHT = 600;

    SDL_Window* gameWindow = nullptr;
    SDL_Renderer* gameRenderer = nullptr;

    std::string title = "My Game Window";

    // Initiaize SDL and create window
    if (!init(
        gameWindow, 
        gameRenderer,
        title, 
        SCREEN_WIDTH, 
        SCREEN_HEIGHT
    )) {
        std::cout << "Failed to intialize SDL!" << std::endl;
        return -1;
    }

    std::vector<SDL_Rect> playerSpriteClips(6);
    playerClip(playerSpriteClips);

    LTexture gameSpriteSheetTexture(gameRenderer);

    // Load images to textures
    if (!loadMedia(
        gameSpriteSheetTexture, 
        "./sprites/characters/player.png"
    )) return -1;

    // Flag to control event loop
    bool running = true;

    // Scale for player sprite
    int playerScale = 3;

    // Player coordinates
    int playerX = SCREEN_WIDTH / 2; 
    int playerY = SCREEN_HEIGHT / 2;

    int playerFrame = 0;
    int frameTime = 100;
    Uint32 lastFrameTime = SDL_GetTicks();

    // Game event loop
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                movePlayer(
                    event, 
                    playerX, 
                    playerY, 
                    SCREEN_WIDTH, 
                    SCREEN_HEIGHT
                );
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(gameRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gameRenderer);

        // Update frame when enough time has passed
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastFrameTime >= frameTime) {
            playerFrame = (playerFrame + 1) % 6;
            lastFrameTime = currentTime;
        }

        // Render textures to screen
        gameSpriteSheetTexture.render(
            playerX, 
            playerY, 
            &playerSpriteClips[playerFrame],
            playerScale
        );

        // Update Screen
        SDL_RenderPresent(gameRenderer);
    }

    // Free images

    // Destroy window    
    SDL_DestroyRenderer(gameRenderer);
    SDL_DestroyWindow(gameWindow);
    gameWindow = nullptr;
    gameRenderer = nullptr;

    // Quit SDL 
    IMG_Quit();
    SDL_Quit();

    // Farewell message
    std::cout << "Goodbye! :)" << std::endl;

    return 0;
}

bool loadMedia(LTexture& gameTexture, std::string path) {
    if (!gameTexture.loadFromFile(path.c_str())) {
        std::cout << "Failed to load texture image " 
                  << path.c_str() << std::endl;
        return false;
    }

    return true;
}

void playerClip(std::vector<SDL_Rect>& clips) {
    int xCordinate = 0;
    int yCordinate = 0;

    for (int i = 0; i < 6; i++) {
        clips[i].x = xCordinate;
        clips[i].y = yCordinate;
        clips[i].w = 48;
        clips[i].h = 48;
        xCordinate += 48;
    }
}
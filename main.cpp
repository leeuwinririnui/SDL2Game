#include "init.h"
#include "class.h"
#include <vector>

bool loadMedia(LTexture& gameTexture, std::string path);
void playerClip(std::vector<SDL_Rect> &clips, int spriteRow);
void playerAnimation(
    int &playerRow, 
    int &playerDirection,
    bool &isMoving, 
    bool &isAttacking,
    SDL_RendererFlip &flip,
    int &playerFrame
);

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
    playerClip(playerSpriteClips, 0);

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

    // Boundaries for player movement
    int playerMinX = 0;
    int playerMaxX = SCREEN_WIDTH - 48 * playerScale;
    int playerMinY = 0;
    int playerMaxY = SCREEN_HEIGHT - 48 * playerScale;

    // Player coordinates
    int playerX = SCREEN_WIDTH / 2; 
    int playerY = SCREEN_HEIGHT / 2;

    // Player direction
    int playerDirection = 0; // 0 = idle, 1 = moving right, -1 = moving left

    // For setting row of sprite sheet
    int playerRow = 0; // Default

    // Flip sprite
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    // Track if player is moving
    bool isMoving = false;

    // Track if player is attacking
    bool isAttacking = false;

    int playerFrame = 0;
    int frameTime = 120;
    Uint32 lastFrameTime = SDL_GetTicks();

    // Game event loop
    while (running) {
        SDL_Event event;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } 
        }

        // Get the current state of the keyboard
        const Uint8 *keyStates = SDL_GetKeyboardState(NULL);

        // Reset movement flag
        isMoving = false;
        
        // Handle movement using keystates
        if (keyStates[SDL_SCANCODE_UP]) {
            if (playerY > playerMinY) {
                playerY -= 5; // Move up
                playerDirection = 2;
                isMoving = true;
            }
        }
        if (keyStates[SDL_SCANCODE_DOWN]) {
            if (playerY < playerMaxY) {
                playerY += 5; // Move down
                playerDirection = 0;
                isMoving = true;
            }
        }
        if (keyStates[SDL_SCANCODE_LEFT]) {
            if (playerX > playerMinX) {
                playerX -= 5; // Move left
                playerDirection = 3;
                isMoving = true;
            }
        }
        if (keyStates[SDL_SCANCODE_RIGHT]) {
            if (playerX < playerMaxX) {
                playerX += 5; // Move right
                playerDirection = 1;
                isMoving = true;
            }
        }
        
        if (keyStates[SDL_SCANCODE_SPACE]) {
            isAttacking = true;
            playerFrame = 0;
        }

        if (isAttacking && playerFrame == 3) {
            isAttacking = false;
            playerFrame = 0;
        }

        // Update player animation and clips based on direction and movement
        playerAnimation(
            playerRow, 
            playerDirection, 
            isMoving, 
            isAttacking, 
            flip,
            playerFrame
        );
        playerClip(playerSpriteClips, playerRow);

        // Clear screen
        SDL_SetRenderDrawColor(gameRenderer, 0, 0, 0, 0);
        SDL_RenderClear(gameRenderer);

        // Update frame when enough time has passed
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastFrameTime >= frameTime) {
            if (isAttacking) {
                playerFrame = (playerFrame + 1) % 4;
            } else {
                playerFrame = (playerFrame + 1) % 6;
            }
            lastFrameTime = currentTime;
        }


        // Render textures to screen
        gameSpriteSheetTexture.render(
            playerX, 
            playerY, 
            &playerSpriteClips[playerFrame],
            playerScale,
            flip
        );

        // Player's bounding box 
        SDL_Rect playerBoundingBox = {playerX, playerY, 48 * playerScale, 48 * playerScale};
        SDL_SetRenderDrawColor(gameRenderer, 255, 0, 0, 255); // Red color
        SDL_RenderDrawRect(gameRenderer, &playerBoundingBox); // Draw outline

        // Update Screen
        SDL_RenderPresent(gameRenderer);

        SDL_Delay(16);
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

void playerClip(std::vector<SDL_Rect> &clips, int spriteRow) {
    int xCordinate = 0;
    int yCordinate = spriteRow * 48; // move to next row

    if (spriteRow < 6) {
        for (int i = 0; i < 6; i++) {
            clips[i].x = xCordinate;
            clips[i].y = yCordinate;
            clips[i].w = 48;
            clips[i].h = 48;
            xCordinate += 48;
        }
    } else if (spriteRow > 5 && spriteRow < 9) {
        for (int i = 0; i < 4; i++) {
            clips[i].x = xCordinate;
            clips[i].y = yCordinate;
            clips[i].w = 48;
            clips[i].h = 48;
            xCordinate += 48;
        }
    } else {
        for (int i = 0; i < 3; i++) {
            clips[i].x = xCordinate;
            clips[i].y = yCordinate;
            clips[i].w = 48;
            clips[i].h = 48;
            xCordinate += 48;
        }
    }
}

void playerAnimation(
    int &playerRow, 
    int &playerDirection,
    bool &isMoving, 
    bool &isAttacking,
    SDL_RendererFlip &flip,
    int &playerFrame
) {
    std::cout << playerFrame << std::endl;
    if (playerDirection == 0) {
        if (isAttacking && playerFrame < 3) {
            playerRow = 6; // Attack animation frame row
        }
        else if (isMoving && !isAttacking) {
            playerRow = 3; // Moving animation row
        } else {
            playerRow = 0; // Idle animation row
        }
    } else if (playerDirection == 1) {
        flip = SDL_FLIP_NONE;
        if (isAttacking && playerFrame < 3) {
            playerRow = 7; // Attack animation frame row
        }
        else if (isMoving && !isAttacking) {
            playerRow = 4; // Moving animation row
        } else {
            playerRow = 1; // Idle animation row
        }
    } else if (playerDirection == 2) {
        if (isAttacking && playerFrame < 3) {
            playerRow = 8; // Attack animation frame row
        }
        else if (isMoving && !isAttacking) {
            playerRow = 5; // Moving animation row
            
        } else {
            playerRow = 2; // Idle animation row
        }
    } else {
        if (isAttacking && playerFrame < 3) {
            playerRow = 7; // Attack animation frame row
        } 
        else if (isMoving && !isAttacking) {
            playerRow = 4; // Moving animation row
        } else {
            playerRow = 1; // Idle animation row
        }
        flip = SDL_FLIP_HORIZONTAL;
    }
}

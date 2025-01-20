#include "init.h"
#include "class.h"
#include <vector>

bool loadMedia(
    LTexture& gameTexture, 
    std::string path
);
void spriteClip(
    std::vector<SDL_Rect> &clips, 
    int spriteRow,
    int frameWidth,
    int frameHeight,
    int offset
); 
void playerAnimation(
    int &playerRow, 
    int &playerDirection,
    bool &playerIsMoving, 
    bool &isAttacking,
    SDL_RendererFlip &flip,
    int &playerFrame
);
void slimeAnimation(
    int &playerRow, 
    int &playerDirection,
    bool &slimeIsMoving, 
    SDL_RendererFlip &flip,
    int &slimeFrame,
    bool slimeAlive
); 
bool playerColliding(
    const SDL_Rect &playerBoundingBox, 
    const SDL_Rect &slimeBoundingBox,
    int screenWidth, 
    int screenHeight
);

int main(int argc, char *argv[]) {
    // Initialze game variables
    int SCREEN_WIDTH = 800;
    int SCREEN_HEIGHT = 600;

    SDL_Window* gameWindow = nullptr;
    SDL_Renderer* renderer = nullptr;

    std::string title = "Slime Boy";

    // Initiaize SDL and create window
    if (!init(
        gameWindow, 
        renderer,
        title, 
        SCREEN_WIDTH, 
        SCREEN_HEIGHT
    )) {
        std::cout << "Failed to intialize SDL!" << std::endl;
        return -1;
    }

    // FIX CLIP AMOUNT
    std::vector<SDL_Rect> playerClips(6);
    spriteClip(
        playerClips, 
        0,
        48,
        48,
        6
    ); 

    std::vector<SDL_Rect> slimeClips(4);
    spriteClip(
        slimeClips,
        0,
        24,
        24,
        2
    );

    LTexture 
        playerSpriteTexture(renderer),
        slimeSpriteTexture(renderer);

    // Load images to textures
    if (!loadMedia(
        playerSpriteTexture, 
        "./sprites/characters/player.png"
    )) return -1;

    if (!loadMedia(
        slimeSpriteTexture, 
        "./sprites/characters/slime.png"
    )) return -1;

    // Flag to control event loop
    bool running = true;

    // Scale for characters
    int playerScale = 3;
    int slimeScale = 2;

    // Boundaries for player movement
    int playerMinX = 0;
    int playerMinY = 0;
    int playerMaxX = SCREEN_WIDTH - 48 * playerScale;
    int playerMaxY = SCREEN_HEIGHT - 48 * playerScale;

    // Player coordinates
    int playerX = SCREEN_WIDTH / 2; 
    int playerY = SCREEN_HEIGHT / 2;

    // Slime movement variables
    int slimeX = 100;
    int slimeY = 200;
    int slimeSpeed = 3;
    int slimeDirection = 1;
    bool slimeIsMoving = false;
    Uint32 slimeLastUpdate = 0;

    // Player direction
    int playerDirection = 0; 
    
    // Sprite sheet rows
    int playerRow = 0; 
    int slimeRow = 0;

    // To Flip sprites
    SDL_RendererFlip 
        playerFlip = SDL_FLIP_NONE, 
        slimeFlip = SDL_FLIP_NONE;

    // Track if player actions
    bool playerIsMoving = false;
    bool isAttacking = false;

    int playerFrame = 0;
    int frameTime = 120;
    Uint32 playerLastFrame = SDL_GetTicks();

    int slimeFrame = 0;
    int slimeFrameTime = 120;
    Uint32 slimeLastFrame = SDL_GetTicks();

    bool slimeAlive = true;

    Uint32 playerLastAttack = 0;
    int attackCooldown = 350;

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
        playerIsMoving = false;
        
        // Handle movement using keystates
        if (keyStates[SDL_SCANCODE_UP]) {
            if (playerY > playerMinY) {
                playerY -= 5; // Move up
                playerDirection = 2;
                playerIsMoving = true;
            }
        }
        if (keyStates[SDL_SCANCODE_DOWN]) {
            if (playerY < playerMaxY) {
                playerY += 5; // Move down
                playerDirection = 0;
                playerIsMoving = true;
            }
        }
        if (keyStates[SDL_SCANCODE_LEFT]) {
            if (playerX > playerMinX) {
                playerX -= 5; // Move left
                playerDirection = 3;
                playerIsMoving = true;
            }
        }
        if (keyStates[SDL_SCANCODE_RIGHT]) {
            if (playerX < playerMaxX) {
                playerX += 5; // Move right
                playerDirection = 1;
                playerIsMoving = true;
            }
        }
        
        if (keyStates[SDL_SCANCODE_SPACE]) {
            Uint32 currentAttackTime = SDL_GetTicks();
            if (currentAttackTime - playerLastAttack >= attackCooldown) {
                isAttacking = true;
                playerFrame = 0;
                playerLastAttack = currentAttackTime;
            }
        }

        if (isAttacking && playerFrame == 3) {
            isAttacking = false;
            playerFrame = 0;
        }

        // Update player animation and clips based on direction and movement
        playerAnimation(
            playerRow, 
            playerDirection, 
            playerIsMoving, 
            isAttacking, 
            playerFlip,
            playerFrame
        );
        spriteClip(
            playerClips, 
            playerRow,
            48,
            48,
            8
        );
        spriteClip(
            slimeClips,
            slimeRow,
            32,
            32,
            2
        );

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        // Update frame sprite frames
        Uint32 currentTime = SDL_GetTicks();

        // Player
        if (currentTime - playerLastFrame >= frameTime) {
            if (isAttacking) {
                playerFrame = (playerFrame + 1) % 4;
            } else {
                playerFrame = (playerFrame + 1) % 6;
            }
            playerLastFrame = currentTime;
        }

        // Slime
        if (currentTime - slimeLastFrame >= frameTime) {
            if (slimeAlive) {
                slimeFrame = (slimeFrame + 1) % 4;
            } else {
                slimeFrame = (slimeFrame + 1) % 5;
            }
            slimeLastFrame = currentTime;
        }

        // Update slime animation based on direction
        slimeAnimation(
            slimeRow,
            slimeDirection,
            slimeIsMoving,
            slimeFlip,
            slimeFrame,
            slimeAlive 
        );

        // Render player to screen
        playerSpriteTexture.render(
            playerX, 
            playerY, 
            &playerClips[playerFrame],
            playerScale,
            playerFlip
        );
        if (slimeAlive) {
            // Render slime to screen
            slimeSpriteTexture.render(
                slimeX,
                slimeY,
                &slimeClips[slimeFrame],
                slimeScale,
                slimeFlip
            );
        } else if (slimeFrame < 4) {
            slimeSpriteTexture.render(
                slimeX,
                slimeY,
                &slimeClips[slimeFrame],
                slimeScale,
                slimeFlip
            );
        } else {
            slimeX = -1000;
        }

        // Player's bounding box 
        SDL_Rect playerBoundingBox = {
            playerX, 
            playerY, 
            48 * playerScale, 
            48 * playerScale
        };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
        SDL_RenderDrawRect(renderer, &playerBoundingBox); 

        // Slimes's bounding box 
        SDL_Rect slimeBoundingBox = {
            slimeX, 
            slimeY, 
            32 * slimeScale, 
            32 * slimeScale
        };
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); 
        SDL_RenderDrawRect(renderer, &slimeBoundingBox); 

        if (slimeAlive && currentTime - slimeLastUpdate > 15) {
            // Calculate center of player bounding box
            const int playerCenterX = playerBoundingBox.x + playerBoundingBox.w / 2;
            const int playerCenterY = playerBoundingBox.y + playerBoundingBox.h / 2;

            // Calculate center of slime bounding box
            const int slimeCenterX = slimeBoundingBox.x + slimeBoundingBox.w / 2;
            const int slimeCenterY = slimeBoundingBox.y + slimeBoundingBox.h / 2;

            // Calculate X-axis movement
            if (slimeCenterX < playerCenterX) {
                // Move slime right
                if (slimeCenterX + slimeSpeed > playerCenterX) {
                    slimeX += playerCenterX - slimeCenterX; // Snap to player center
                } else {
                    slimeX += slimeSpeed;
                }
                slimeDirection = 1; 
            } else if (slimeCenterX > playerCenterX) {
                // Move slime left
                if (slimeCenterX - slimeSpeed < playerCenterX) {
                    slimeX -= slimeCenterX - playerCenterX; 
                } else {
                    slimeX -= slimeSpeed;
                }
                slimeDirection = 3; 
            }

            // Calculate Y-axis movement
            if (slimeCenterY < playerCenterY) {
                // Move slime down
                if (slimeCenterY + slimeSpeed > playerCenterY) {
                    slimeY += playerCenterY - slimeCenterY; 
                } else {
                    slimeY += slimeSpeed;
                }
                slimeDirection = 0; 
            } else if (slimeCenterY > playerCenterY) {
                // Move slime up
                if (slimeCenterY - slimeSpeed < playerCenterY) {
                    slimeY -= slimeCenterY - playerCenterY; 
                } else {
                    slimeY -= slimeSpeed;
                }
                slimeDirection = 2;
            }

            // Update last movement time
            slimeLastUpdate = currentTime;
        }

        // Check for collisions
        if (
            playerColliding(
                playerBoundingBox, 
                slimeBoundingBox,
                SCREEN_WIDTH, 
                SCREEN_HEIGHT
            )
        ) {
            if (isAttacking && slimeAlive) {
                std::cout << "Slime takes a hit" << std::endl;
                slimeAlive = false;
                slimeRow = 12;
                slimeFrame = 0;
            }
        }


        // Update Screen
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    // Destroy window    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(gameWindow);
    gameWindow = nullptr;
    renderer = nullptr;

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

void spriteClip(
    std::vector<SDL_Rect> &clips, 
    int spriteRow,
    int frameWidth,
    int frameHeight,
    int offset
) {
    int xCordinate = 0;
    int yCordinate = offset + spriteRow * frameHeight; 

    for (int i = 0; i < clips.size(); i++) {
        clips[i].x = xCordinate;
        clips[i].y = yCordinate;
        clips[i].w = frameWidth;
        clips[i].h = frameHeight;
        xCordinate += frameWidth;
    }
}


void playerAnimation(
    int &playerRow, 
    int &playerDirection,
    bool &playerIsMoving, 
    bool &isAttacking,
    SDL_RendererFlip &flip,
    int &playerFrame
) {
    if (playerDirection == 0) {
        if (isAttacking && playerFrame < 3) {
            playerRow = 6; // Attack animation frame row
        } else if (playerIsMoving && !isAttacking) {
            playerRow = 3; // Moving animation row
        } else {
            playerRow = 0; // Idle animation row
        }
    } else if (playerDirection == 1) {
        flip = SDL_FLIP_NONE;
        if (isAttacking && playerFrame < 3) {
            playerRow = 7; // Attack animation frame row
        } else if (playerIsMoving && !isAttacking) {
            playerRow = 4; // Moving animation row
        } else {
            playerRow = 1; // Idle animation row
        }
    } else if (playerDirection == 2) {
        if (isAttacking && playerFrame < 3) {
            playerRow = 8; // Attack animation frame row
        } else if (playerIsMoving && !isAttacking) {
            playerRow = 5; // Moving animation row
            
        } else {
            playerRow = 2; // Idle animation row
        }
    } else {
        if (isAttacking && playerFrame < 3) {
            playerRow = 7; // Attack animation frame row
        } else if (playerIsMoving && !isAttacking) {
            playerRow = 4; // Moving animation row
        } else {
            playerRow = 1; // Idle animation row
        }
        flip = SDL_FLIP_HORIZONTAL;
    }
}

void slimeAnimation(
    int &slimeRow, 
    int &slimeDirection,
    bool &slimeIsMoving, 
    SDL_RendererFlip &flip,
    int &slimeFrame,
    bool slimeAlive
) {
    if (slimeAlive) {
        if (slimeDirection == 0) {
            slimeRow = 0;
        } else if (slimeDirection == 1) {
            slimeRow = 1;
            flip = SDL_FLIP_NONE;
        } else if (slimeDirection == 2) {
            slimeRow = 2;
        } else if (slimeDirection == 3) {
            slimeRow = 1;
            flip = SDL_FLIP_HORIZONTAL;
        }
    }
    
}

bool playerColliding(
    const SDL_Rect &playerBoundingBox, 
    const SDL_Rect &slimeBoundingBox,
    int screenWidth, 
    int screenHeight
) {
    bool isColliding = false;

    if (
        playerBoundingBox.x < slimeBoundingBox.x + slimeBoundingBox.w &&
        playerBoundingBox.x + playerBoundingBox.w > slimeBoundingBox.x &&
        playerBoundingBox.y < slimeBoundingBox.y + slimeBoundingBox.h &&
        playerBoundingBox.y + playerBoundingBox.h > slimeBoundingBox.y
    ) {
        isColliding = true;
    }

    return isColliding;
}


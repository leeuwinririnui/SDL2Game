#include "init.h"
#include "character.h"

bool playerColliding(
    const SDL_Rect &playerBoundingBox, 
    const SDL_Rect &slimeBoundingBox
);
bool isFacingSlime(
    const SDL_Rect &playerBoundingBox, 
    const SDL_Rect &slimeBoundingBox, 
    Character *player,
    Character *slime
);
void attackSlime(Character *player, Character *slime); 

int playerPoints = 0;

void renderText(SDL_Renderer* renderer, int points);

int main(int argc, char *argv[]) {
    // Initialze game variables
    int SCREEN_WIDTH = 1000;
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

    LTexture playerTexture(renderer);
    LTexture slimeTexture(renderer);

    std::string pathToPlayer("./sprites/characters/player.png");
    std::string pathToSlime("./sprites/characters/slime.png");

    // Might need multuple for each row
    std::vector<SDL_Rect> playerClips(6);
    std::vector<SDL_Rect> slimeClips(7);

    Character *player = new Character(
        playerTexture, 
        pathToPlayer,
        playerClips,
        48,
        48,
        0,
        6,
        0,
        0,
        false,
        false,
        SDL_FLIP_NONE,
        3,
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2,
        120,
        true
    );

    Character *slime = new Character(
        slimeTexture, 
        pathToSlime,
        slimeClips,
        32,
        32,
        0,
        2,
        0,
        0,
        false,
        false,
        SDL_FLIP_NONE,
        2,
        150,
        150,
        120,
        true
    );

    // Set character animation rows
    player->setIdleRows({ 0, 1, 2, 1 });
    player->setMoveRows({ 3, 4, 5, 4 });
    player->setAttackRows({ 6, 7, 8, 7 });
    slime->setIdleRows({ 0, 1, 2, 1 });
    slime->setMoveRows({ 3, 4, 5, 4 });
    slime->setDeathRow(12);

    // Load images to textures
    player->loadMedia();
    slime->loadMedia();

    // Clip sprites
    player->spriteClip();
    slime->spriteClip();

    // Boundaries for player movement
    int playerMinX = 0;
    int playerMinY = 0;
    int playerMaxX = SCREEN_WIDTH - 48 * player->getScale();
    int playerMaxY = SCREEN_HEIGHT - 48 * player->getScale();

    Uint32 playerLastFrame = SDL_GetTicks();
    Uint32 slimeLastFrame = SDL_GetTicks();

    Uint32 playerLastAttack = 0;
    int attackCooldown = 350;

    Uint32 slimeRespawnTime = 0;
    const Uint32 slimeRespawnDelay = 3000; 
    int slimeSpawnX = 100; // Default spawn position
    int slimeSpawnY = 100;


    // Flag to start game
    bool start = false;

    // Flag to control event loop
    bool running = true;

    // Game event loop
    while (running) {
        SDL_Event event;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } 
        }

        Uint32 currentTime = SDL_GetTicks();
        const Uint8 *keyStates = SDL_GetKeyboardState(NULL);
        player->setMoving(false);
        
        // Handle movement
        if (keyStates[SDL_SCANCODE_UP] && player->getPosY() > playerMinY) {
            player->setPosY(player->getPosY() - 5);
            player->setDirection(2);
            player->setMoving(true);
        }
        if (keyStates[SDL_SCANCODE_DOWN] && player->getPosY() < playerMaxY) {
            player->setPosY(player->getPosY() + 5);
            player->setDirection(0);
            player->setMoving(true);
        }
        if (keyStates[SDL_SCANCODE_LEFT] && player->getPosX() > playerMinX) {
            player->setPosX(player->getPosX() - 5);
            player->setDirection(3);
            player->setMoving(true);
        }
        if (keyStates[SDL_SCANCODE_RIGHT] && player->getPosX() < playerMaxX) {
            player->setPosX(player->getPosX() + 5);
            player->setDirection(1);
            player->setMoving(true);
        }

        // Handle player attack
        if (keyStates[SDL_SCANCODE_SPACE]) {
            Uint32 currentAttackTime = SDL_GetTicks();
            if (currentAttackTime - playerLastAttack >= attackCooldown) {
                player->setAttacking(true);
                player->setCurrentFrame(0);
                playerLastAttack = currentAttackTime;
            }
        }

        if (player->getAttacking() && player->getCurrentFrame() == 3) {
            player->setAttacking(false);
            player->setCurrentFrame(0);
        }

        // Update animations
        if (currentTime - playerLastFrame >= player->getFrameTime()) {
            if (player->getAttacking()) {
                player->setCurrentFrame((player->getCurrentFrame() + 1) % 4);
            } else {
                player->setCurrentFrame((player->getCurrentFrame() + 1) % 6);
            }
            playerLastFrame = currentTime;
        }

        // Slime
        if (currentTime - slimeLastFrame >= slime->getFrameTime()) {
            if (slime->getAlive()) {
                slime->setCurrentFrame((slime->getCurrentFrame() + 1) % 4);
            } else {
                slime->setCurrentFrame((slime->getCurrentFrame() + 1) % 5);
            }
            slimeLastFrame = currentTime;
        }

        // Player's bounding box 
        SDL_Rect playerBoundingBox = {
            player->getPosX(), 
            player->getPosY(), 
            48 * player->getScale(), 
            48 * player->getScale()
        };

        // Slimes's bounding box 
        SDL_Rect slimeBoundingBox = {
            slime->getPosX(), 
            slime->getPosY(), 
            32 * slime->getScale(), 
            32 * slime->getScale()
        };

        int playerCenterX = playerBoundingBox.x + playerBoundingBox.w / 2;
        int playerCenterY = playerBoundingBox.y + playerBoundingBox.h / 2;

        int slimeCenterX = slimeBoundingBox.x + slimeBoundingBox.w / 2;
        int slimeCenterY = slimeBoundingBox.y + slimeBoundingBox.h / 2;

        // Handle slime movement
        if (slime->getAlive()) {
            // Adjust vertical position
            if (slimeCenterY < playerCenterY - 25) {
                slime->setPosY(slime->getPosY() + 2);
                slime->setDirection(0); // Down
            }
            if (slimeCenterY > playerCenterY + 25) {
                slime->setPosY(slime->getPosY() - 2);
                slime->setDirection(2); // Up
            }
            if (slimeCenterX < playerCenterX - 25) {
                slime->setPosX(slime->getPosX() + 2);
                slime->setDirection(1); // Right
            }
            if (slimeCenterX > playerCenterX + 25) {
                slime->setPosX(slime->getPosX() - 2);
                slime->setDirection(3); // Left
            }
        } else if (slime->getCurrentFrame() == 3) {
            slime->setPosX(-1000);
        }

        // Check for collisions
        if (playerColliding(playerBoundingBox, slimeBoundingBox)) {
            std::cout << "Colliding" << std::endl;
            if (isFacingSlime(playerBoundingBox, slimeBoundingBox, player, slime)) {
                std::cout << "Facing Slime" << std::endl;
                attackSlime(player, slime);
            }
        } else {
            std::cout << "Not Colliding" << std::endl;
        }

        // Update animations
        player->updateAnimation();
        slime->updateAnimation();
        
        player->spriteClip();
        slime->spriteClip();

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        // Render characters to screen
        player->texture.render(
            player->getPosX(), 
            player->getPosY(), 
            &playerClips[player->getCurrentFrame()],
            player->getScale(),
            player->getFlip()
        );

        slime->texture.render(
            slime->getPosX(), 
            slime->getPosY(), 
            &slimeClips[slime->getCurrentFrame()],
            slime->getScale(),
            slime->getFlip()
        );

        // Update Screen
        renderText(renderer, playerPoints);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // Destroy window    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(gameWindow);
    gameWindow = nullptr;
    renderer = nullptr;
    player = nullptr;
    slime = nullptr;

    // Quit SDL 
    IMG_Quit();
    SDL_Quit();

    // Farewell message
    std::cout << "Goodbye! :)" << std::endl;

    return 0;
}

bool playerColliding(
    const SDL_Rect &playerBoundingBox, 
    const SDL_Rect &slimeBoundingBox
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

bool isFacingSlime(
    const SDL_Rect &playerBoundingBox, 
    const SDL_Rect &slimeBoundingBox, 
    Character *player,
    Character *slime
) {
    if (player->getDirection() == 0 && slime->getDirection() == 2 && playerBoundingBox.y + playerBoundingBox.h > (slimeBoundingBox.y + slimeBoundingBox.h)) {
        return true;
    }
    if (player->getDirection() == 1 && slime->getDirection() == 3 && playerBoundingBox.x + playerBoundingBox.w >= slimeBoundingBox.x + slimeBoundingBox.w) {
        return true;
    }
    if (player->getDirection() == 2 && slime->getDirection() == 0 && playerBoundingBox.y < slimeBoundingBox.y) {
        return true;
    }
    if (player->getDirection() == 3 && slime->getDirection() == 1 && playerBoundingBox.x < slimeBoundingBox.x) {
        return true;
    }

    return false;
}


void attackSlime(Character *player, Character *slime) {
    if (player->getAttacking() && slime->getAlive()) {
            std::cout << "Slime takes a hit" << std::endl;
            slime->setAlive(false);
            slime->setRow(12);
            slime->setCurrentFrame(0);
            playerPoints += 10;
        }
}

void renderText(SDL_Renderer* renderer, int points) {
    TTF_Font* font = TTF_OpenFont("./ThaleahFat.ttf", 28);
    if (!font) {
        std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    // Convert score to text
    std::string scoreText = "Score: " + std::to_string(points);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), { 255, 255, 255 }); 
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Get text dimensions
    SDL_Rect textRect = { 10, 10, textSurface->w, textSurface->h }; // Position at the top left corner

    // Render the text
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Free resources
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}


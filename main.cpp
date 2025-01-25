#include "init.h"
#include "character.h"

#define MAXSLIMES 100
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#define TITLE "Slime Boy"
#define PATHTOPLAYER "./sprites/characters/player.png"
#define PATHTOSLIME "./sprites/characters/slime.png"
#define PLAYERSCALE 3
#define SLIMESCALE 2
#define ATTACKCOOLDOWN 400

enum GameState {
    GameStart,
    Playing,
    GameOver
};

void startScreen(SDL_Renderer* renderer, bool &running);
void handleEvents(SDL_Event &event, bool &running);
bool playerColliding(Character *player,Character *slime);
void attackSlime(Character *player, Character *slime, int &playerPoints);
void renderPoints(SDL_Renderer* renderer, int &playerPoints);
void playerMovement(const Uint8 *keyStates, Character *player);
void handlePlayerActions(const Uint8 *keyStates, Character *player, Uint32 currentTime, Uint32 &playerLastAttac);
void playerAttack(const Uint8 *keyStates, Character *player, Uint32 currentTime,Uint32 &playerLastAttack);
void handleSlimeActions(std::vector<Character *> &slimes, Character *player);
void slimeMovement(Character *slime, Character *player);
void slimeDeath(Character *slime);
void handleCollisions(Character *player, std::vector<Character *> &slimes, int &playerPoints); 
void updateFrames(Character *player, std::vector<Character *> &slimes, Uint32 currentTime); 
void renderCharacter(SDL_Renderer *renderer, Character *character, std::vector<SDL_Rect> clips);
void renderBoundingBox(SDL_Renderer *renderer, Character *character, SDL_Color color); 
void endScreen(SDL_Renderer *renderer, bool &running, int playerPoints);
void resetPlayer(Character *player, int &points);
void resetSlime(Character *slime);

int main(int argc, char *argv[]) {
    SDL_Window* gameWindow = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Initiaize SDL and create window
    if (!init(gameWindow, renderer, TITLE, SCREEN_WIDTH,SCREEN_HEIGHT)) {
        std::cout << "Failed to intialize SDL!" << std::endl;
        return -1;
    }

    // Texture loading and character setup
    LTexture playerTexture(renderer);
    LTexture slimeTexture(renderer);

    std::vector<SDL_Rect> playerClips(6);
    Character *player = new Character(
        100, playerTexture, PATHTOPLAYER, playerClips, 48, 48, 0, 8, 0, 0, false,
        false, SDL_FLIP_NONE, PLAYERSCALE, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 
        80, true, 15, 5
    );

    int playerPoints = 0;

    // Player boundaries
    player->setMinX(0);
    player->setMinY(0);
    player->setMaxX(SCREEN_WIDTH - 48 * player->getScale());
    player->setMaxY(SCREEN_HEIGHT - 48 * player->getScale());
    
     // Slime setup
    std::vector<Character*> slimes;
    std::vector<std::vector<SDL_Rect>> slimeClips(MAXSLIMES, std::vector<SDL_Rect>(7));
    srand(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < MAXSLIMES; i++) {
        int randomSpeed = (std::rand() % 3) + 1;
        Character *slime = new Character(
            20, slimeTexture, PATHTOSLIME, slimeClips[i], 32, 32, 0, 2, 0, 0,
            false, false, SDL_FLIP_NONE, SLIMESCALE, 0, 0, 120, false, 5, randomSpeed
        );
        slimes.push_back(slime);
    }

    // Character animations and media loading
    player->setIdleRows({ 0, 1, 2, 1 });
    player->setMoveRows({ 3, 4, 5, 4 });
    player->setAttackRows({ 6, 7, 8, 7 });
    player->setDeathRow(9);
    player->loadMedia();
    player->spriteClip();

    for (auto &slime : slimes) {
        slime->setIdleRows({ 0, 1, 2, 1 });
        slime->setMoveRows({ 3, 4, 5, 4 });
        slime->setDeathRow(12);
        slime->setRandomSpawnPoints();
        slime->loadMedia();
        slime->spriteClip();
    }

    Uint32 playerLastAttack = 0;    
    GameState gameState = GameState::GameStart;
    bool running = true;

    // Game event loop
    while (running) {
        // Handle Game Start
        if (gameState == GameState::GameStart) {
            startScreen(renderer, running);
            if (!running) {
                break; // Exit if user quits
            } else {
                gameState = GameState::Playing; // Start the game if Enter is pressed
            }
        }
        
        // Handle Game Over
        else if (gameState == GameState::GameOver) {
            endScreen(renderer, running, playerPoints);
            if (!running) {
                break;  // Exit the loop if user quits
            }

            resetPlayer(player, playerPoints);
            for (auto &slime : slimes) resetSlime(slime);

            gameState = GameState::Playing;  // Go back to GameStart after GameOver
        }
        
        // Handle Playing
        else if (gameState == GameState::Playing) {
            SDL_Event event;
            handleEvents(event, running);

            Uint32 currentTime = SDL_GetTicks();
            const Uint8 *keyStates = SDL_GetKeyboardState(NULL);
            player->setMoving(false);

            for (auto &slime : slimes) {
                slime->respawnTimer();
                slime->checkRespawn();
            }

            handlePlayerActions(keyStates, player, currentTime, playerLastAttack);
            handleSlimeActions(slimes, player);
            handleCollisions(player, slimes, playerPoints);

            updateFrames(player, slimes, currentTime);

            player->updateAnimation();
            player->spriteClip();
            for (auto &slime : slimes) {
                slime->updateAnimation();
                slime->spriteClip();
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            for (auto const &slime : slimes) {
                renderCharacter(renderer, slime, slime->getClips());
                renderBoundingBox(renderer, slime, SDL_Color{ 0, 0, 255, 255 });
            }

            renderCharacter(renderer, player, playerClips);
            renderBoundingBox(renderer, player, SDL_Color{ 255, 0, 0, 255 });

            renderPoints(renderer, playerPoints);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);

            if (!player->getAlive() && player->getCurrentFrame() == 2) {
                std::cout << "Game Over!" << std::endl;
                gameState = GameState::GameOver;  // Transition to Game Over
            }
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(gameWindow);
    IMG_Quit();
    SDL_Quit();

    std::cout << "Goodbye! :)" << std::endl;
    return 0;
}

// --------------------------- FUNCTIONS ---------------------------
void handleEvents(SDL_Event &event, bool &running) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }
}

void handlePlayerActions(const Uint8 *keyStates, Character *player, Uint32 currentTime, Uint32 &playerLastAttack) {
    if (!player->getAlive()) return;

    playerMovement(keyStates, player);
    playerAttack(keyStates, player, currentTime, playerLastAttack);
}

void playerMovement(const Uint8 *keyStates, Character *player) {
    int minX = player->getMinX();
    int maxX = player->getMaxX();
    int minY = player->getMinY();
    int maxY = player->getMaxY();
    int posX = player->getPosX();
    int posY = player->getPosY();
    int speed = player->getSpeed();

    bool isMoving = false;

    if (keyStates[SDL_SCANCODE_UP] && posY > minY) {
        player->setPosY(posY - speed);
        player->setDirection(2); // Up
        isMoving = true;
    }

    if (keyStates[SDL_SCANCODE_DOWN] && posY < maxY) {
        player->setPosY(posY + speed);
        player->setDirection(0); // Down
        isMoving = true;
    }

    if (keyStates[SDL_SCANCODE_LEFT] && posX > minX) {
        player->setPosX(posX - speed);
        player->setDirection(3); // Left
        isMoving = true;
    }

    if (keyStates[SDL_SCANCODE_RIGHT] && posX < maxX) {
        player->setPosX(posX + speed);
        player->setDirection(1); // Right
        isMoving = true;
    }

    player->setMoving(isMoving);
}

void playerAttack(const Uint8 *keyStates, Character *player, Uint32 currentTime, Uint32 &playerLastAttack) {
    if (keyStates[SDL_SCANCODE_SPACE]) {
        if (currentTime - playerLastAttack >= ATTACKCOOLDOWN) {
            player->setAttacking(true);
            player->setCurrentFrame(0);
            playerLastAttack = currentTime;
        }
    }

    if (player->getAttacking() && player->getCurrentFrame() == 3) {
        player->setAttacking(false);
        player->setCurrentFrame(0);
    }
}

void handleSlimeActions(std::vector<Character *> &slimes, Character *player) {
    for (auto &slime : slimes) {
        if (slime->getAlive()) {
            slimeMovement(slime, player); // Handles alive slimes
        } else {
            slimeDeath(slime); // Handles dead slimes
        }
    }
}

void slimeMovement(Character *slime, Character *player) {
    if (!player->getAlive()) return;

    slime->setMoving(true);

    SDL_Point playerCenter = player->getBoundingBoxCenter();
    SDL_Point slimeCenter = slime->getBoundingBoxCenter();

    int slimeSpeed = slime->getSpeed();
    int slimePosX = slime->getPosX();
    int slimePosY = slime->getPosY();

    if (slimeCenter.y < playerCenter.y) {
        slime->setPosY(slimePosY + slimeSpeed);
        slime->setDirection(0); // Down
    }

    if (slimeCenter.y > playerCenter.y) {
        slime->setPosY(slimePosY - slimeSpeed);
        slime->setDirection(2); // Up
    }

    if (slimeCenter.x < playerCenter.x) {
        slime->setPosX(slimePosX + slimeSpeed);
        slime->setDirection(1); // Right
    }

    if (slimeCenter.x > playerCenter.x) {
        slime->setPosX(slimePosX - slimeSpeed);
        slime->setDirection(3); // Left
    }

    if (slimeCenter.x == playerCenter.x && slimeCenter.y == playerCenter.y) {
        player->setHealth(player->getHealth() - 20);
        if (player->getHealth() <= 0) {
            player->setAlive(false);
            player->setCurrentFrame(0);
        }
    }
}

void slimeDeath(Character *slime) {
    slime->setMoving(false);

    // Move the slime off-screen when the death animation is finished
    if (slime->getCurrentFrame() == 4) {
        slime->setPosX(-1000);
    }
}

void handleCollisions(Character *player, std::vector<Character *> &slimes, int &playerPoints) {
    for (auto &slime : slimes) {
        if (playerColliding(player, slime)) {
            attackSlime(player, slime, playerPoints);
        }
    }
}

bool playerColliding(Character *player, Character *slime) {
    int playerDirection = player->getDirection();
    SDL_Rect playerBox = player->getBoundingBox();
    SDL_Rect slimeBox = slime->getBoundingBox();

    if (playerDirection == 0 && slimeBox.y + slimeBox.h <= playerBox.y + playerBox.h / 2) return false;
    if (playerDirection == 1 && slimeBox.x + slimeBox.w <= playerBox.x + playerBox.w / 2) return false;
    if (playerDirection == 2 && slimeBox.y >= playerBox.y + playerBox.h / 2) return false;
    if (playerDirection == 3 && slimeBox.x >= playerBox.x + playerBox.w / 2) return false;

    return playerBox.x < slimeBox.x + slimeBox.w &&
           playerBox.x + playerBox.w > slimeBox.x &&
           playerBox.y < slimeBox.y + slimeBox.h &&
           playerBox.y + playerBox.h > slimeBox.y;
}

void attackSlime(Character *player, Character *slime, int &playerPoints) {
    if (player->getAttacking() && slime->getAlive() && player->getCurrentFrame() < 2) {
        std::cout << "Slime takes a hit" << std::endl;
        slime->setHealth(0); // One shot
        slime->setAlive(false);
        slime->setCurrentFrame(0);
        playerPoints += 10;
    }
}

void updateFrames(Character *player, std::vector<Character *> &slimes, Uint32 currentTime) {
    // Update player animation
    if (currentTime - player->getLastFrame() >= player->getFrameTime()) {
        if (player->getAttacking()) {
            player->setCurrentFrame((player->getCurrentFrame() + 1) % 4);
        } else {
            player->setCurrentFrame((player->getCurrentFrame() + 1) % 6);
        }
        player->updateLastFrame();
    }

    // Update slime animations
    for (auto &slime : slimes) {
        if (currentTime - slime->getLastFrame() >= slime->getFrameTime()) {
            if (slime->getAlive()) {
                slime->setCurrentFrame((slime->getCurrentFrame() + 1) % 4);
            } else {
                slime->setCurrentFrame((slime->getCurrentFrame() + 1) % 5);
            }
            slime->updateLastFrame();
        }
    }
}

void renderCharacter(SDL_Renderer *renderer, Character *character, std::vector<SDL_Rect> clips) {
    character->texture.render(
        character->getPosX(),
        character->getPosY(),
        &clips[character->getCurrentFrame()],
        character->getScale(),
        character->getFlip()
    );
}

void renderBoundingBox(SDL_Renderer *renderer, Character *character, SDL_Color color) {
    SDL_Rect boundingBox = character->getBoundingBox();
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &boundingBox);
}

void renderPoints(SDL_Renderer* renderer, int &playerPoints) {
    TTF_Font* font = TTF_OpenFont("./ThaleahFat.ttf", 28);
    if (!font) {
        std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    // Convert score to text
    std::string scoreText = "Score: " + std::to_string(playerPoints);
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

void startScreen(SDL_Renderer *renderer, bool &running) {
    // Load font
    TTF_Font* font = TTF_OpenFont("./ThaleahFat.ttf", 42);
    if (!font) {
        std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }
    
    // Create the message texture
    std::string message = "Press Enter to start";
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, message.c_str(), { 255, 255, 255 });
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    
    // Get text dimensions and position
    SDL_Rect textRect = { (SCREEN_WIDTH - textSurface->w) / 2, (SCREEN_HEIGHT - textSurface->h) / 2, textSurface->w, textSurface->h };

    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render the text
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_RenderPresent(renderer);

    // Handle events
    SDL_Event event;
    bool waiting = true;

    while (waiting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                waiting = false;
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_ESCAPE) {
                    waiting = false;
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                }
            }
        }
    }

    // Clean up resources
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

void endScreen(SDL_Renderer *renderer, bool &running, int playerPoints) {
    // Load font
    TTF_Font* font = TTF_OpenFont("./ThaleahFat.ttf", 42);

    // Create the message texture
    std::string message = "Game Over! Press Enter to Play Again";
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, message.c_str(), { 255, 255, 255 });
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Get text dimensions and position
    SDL_Rect textRect = { (SCREEN_WIDTH - textSurface->w) / 2, (SCREEN_HEIGHT - textSurface->h) / 2, textSurface->w, textSurface->h };

    renderPoints(renderer, playerPoints);

    // Render the text
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_RenderPresent(renderer);

    // Handle events
    SDL_Event event;
    bool waiting = true;

    while (waiting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                waiting = false;
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN) {
                    waiting = false;  // Exit the game
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    waiting = false;
                    running = false;  // Quit the game
                }
            }
        }
    }

    // Clean up resources
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

void resetPlayer(Character *player, int &playerPoints) {
    player->setHealth(100);
    player->setRow(0);
    player->setOffset(8);
    player->setDirection(0);
    player->setCurrentFrame(0);
    player->setMoving(false);
    player->setAttacking(false);
    player->setFlip(SDL_FLIP_NONE);
    player->setAlive(true);
    playerPoints = 0;
}

void resetSlime(Character *slime) {
    slime->setHealth(20);
    slime->setRow(0);
    slime->setOffset(2);
    slime->setDirection(0);
    slime->setCurrentFrame(0);
    slime->setMoving(false);
    slime->setAttacking(false);
    slime->setFlip(SDL_FLIP_NONE);
    slime->setAlive(true);
    slime->setRandomSpawnPoints();
};

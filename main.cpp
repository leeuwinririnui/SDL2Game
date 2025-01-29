#include "init.h"
#include "player.h"
#include "texture.h"
#include "slime.h"

#define MAXSLIMES 20
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#define TITLE "Slime Boy"
#define PATHTOPLAYER "./sprites/characters/player.png"
#define PATHTOSLIME "./sprites/characters/slime.png"
#define PLAYERSCALE 3
#define SLIMESCALE 2

// Enumarator to track different game states
enum GameState {
    GameStart,
    Playing,
    GameOver
};

// Enumarator to track directions
enum Direction {
    Down,
    Right, 
    Up, 
    Left
};

// --------------------------- FUNCTION DECLARATIONS ---------------------------
void handleEvents(SDL_Event &event, bool &running);
bool playerColliding(Player *player,Slime *slime);
bool slimeColliding(Player *player, Slime *slime);
void attackSlime(Player *player, Slime *slime, int &playerPoints);
void playerMovement(const Uint8 *keyStates, Player *player);
void handlePlayerActions(const Uint8 *keyStates, Player *player, Uint32 currentTime);
void playerAttack(const Uint8 *keyStates, Player *player, Uint32 currentTime);
void handleSlimeActions(std::vector<Slime*> &slimes, Player *player);
bool isBlocked(int nextX, int nextY, Slime *currentSlime, const std::vector<Slime*> &slimes); 
void slimeMovement(Slime *slime, Player *player, const std::vector<Slime*> &slimes);
void slimeDeath(Slime *slime);
void handleCollisions(Player *player, std::vector<Slime*> &slimes, int &playerPoints, Uint32 currentTime);
void slimeAttacks(Player *player, Slime *slime, Uint32 currentTime);
void updateFrames(Player *player, std::vector<Slime*> &slimes, Uint32 currentTime); 
void renderCharacter(SDL_Renderer *renderer, Character *character, std::vector<SDL_Rect> clips);
void renderPoints(SDL_Renderer* renderer, int &playerPoints);
void renderHealth(SDL_Renderer* renderer, int playerHealth);
void renderBoundingBox(SDL_Renderer *renderer, Character *character, SDL_Color color); 
void startScreen(SDL_Renderer* renderer, bool &running);
void endScreen(SDL_Renderer *renderer, bool &running, int playerPoints);

int main(int argc, char *argv[]) {
    SDL_Window* gameWindow = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Initiaize SDL and create window
    if (!init(gameWindow, renderer, TITLE, SCREEN_WIDTH,SCREEN_HEIGHT)) {
        std::cout << "Failed to intialize SDL!" << std::endl;
        return -1;
    }

    // Texture loading and character setup
    LTexture *map = new LTexture(renderer);
    LTexture playerTexture(renderer);
    LTexture slimeTexture(renderer);

    // Map setup
    std::string mapPath = "./alphamap.png";
    if (!map->loadFromFile(mapPath)) {
        std::cout << "Failed to load texture image " << mapPath << std::endl;
        return -1;
    }

    std::vector<SDL_Rect> playerClips(6);
    Player *player = new Player(
        100, playerTexture, PATHTOPLAYER, playerClips, 48, 48, 0, 8, 0, 0, false,
        false, SDL_FLIP_NONE, PLAYERSCALE, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 
        80, true, 24, 400, 4
    );

    int playerPoints = 0;

    // Player boundaries
    player->setMinX(0);
    player->setMinY(0);
    player->setMaxX(SCREEN_WIDTH - 48 * player->getScale());
    player->setMaxY(SCREEN_HEIGHT - 48 * player->getScale());
    
     // Slime setup
    std::vector<Slime*> slimes;
    std::vector<std::vector<SDL_Rect>> slimeClips(MAXSLIMES, std::vector<SDL_Rect>(7));
    srand(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < MAXSLIMES; i++) {
        int randomSpeed = (std::rand() % 3) + 1;
        Slime *slime = new Slime(
            20, slimeTexture, PATHTOSLIME, slimeClips[i], 32, 32, 0, 2, 0, 0,
            false, false, SDL_FLIP_NONE, SLIMESCALE, 0, 0, 140, false, 5, 400, randomSpeed
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
        slime->setAttackRows({ 6, 7, 8, 7 });
        slime->setDeathRow(12);
        slime->setRandomSpawnPoints();
        slime->loadMedia();
        slime->spriteClip();
    }
    
    Uint32 playerLastAttack = 0;    
    GameState gameState = GameState::GameStart;
    bool running = true;
    Uint32 currentTime;

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

            player->resetStates();
            for (auto &slime : slimes) slime->resetStates();
            
            playerPoints = 0;

            gameState = GameState::Playing;  // Go back to GameStart after GameOver
        }
        
        // Handle Playing
        else if (gameState == GameState::Playing) {
            SDL_Event event;
            handleEvents(event, running);

            currentTime = SDL_GetTicks();
            const Uint8 *keyStates = SDL_GetKeyboardState(NULL);
            player->setMoving(false);

            for (auto &slime : slimes) {
                slime->respawnTimer();
                slime->checkRespawn();
            }

            // Handle movements and collisions
            handlePlayerActions(keyStates, player, currentTime);
            handleSlimeActions(slimes, player);
            handleCollisions(player, slimes, playerPoints, currentTime);
            
            updateFrames(player, slimes, currentTime);
            
            player->updateAnimation();
            player->spriteClip();
            for (auto &slime : slimes) {
                slime->updateAnimation();
                slime->spriteClip();
            }

            // Clear screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            map->render(0, 0, nullptr, 1, SDL_FLIP_NONE);

            // Player will flash red if attacked by slime
            player->updateTint(currentTime);

            // Render characters
            renderCharacter(renderer, player, playerClips);
            renderBoundingBox(renderer, player, SDL_Color{ 255, 0, 0, 255 });

            for (auto const &slime : slimes) {
                renderCharacter(renderer, slime, slime->getClips());
                renderBoundingBox(renderer, slime, SDL_Color{ 0, 0, 255, 255 });
            }

            // Render points and health
            renderPoints(renderer, playerPoints);
            renderHealth(renderer, player->getHealth());

            SDL_RenderPresent(renderer);
            SDL_Delay(16);

            // End game if player death animation has completed
            if (!player->getAlive() && player->getCurrentFrame() == 2) {
                player->setFlashing(false);
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

// --------------------------- FUNCTION DEFINITIONS ---------------------------
void handleEvents(SDL_Event &event, bool &running) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }
}

void handlePlayerActions(const Uint8 *keyStates, Player *player, Uint32 currentTime) {
    if (!player->getAlive()) return;

    playerMovement(keyStates, player);
    playerAttack(keyStates, player, currentTime);
}

void playerMovement(const Uint8 *keyStates, Player *player) {
    int minX = player->getMinX();
    int maxX = player->getMaxX();
    int minY = player->getMinY();
    int maxY = player->getMaxY();
    int posX = player->getPosX();
    int posY = player->getPosY();
    int speed = player->getSpeed();

    bool isMoving = false;

    if (!player->getAttacking()) {
        if (keyStates[SDL_SCANCODE_UP] && !keyStates[SDL_SCANCODE_DOWN]) {
            player->setDirection(Up); 
            if (posY > minY) {
                player->setPosY(posY - speed);
                isMoving = true;
            }
        }

        if (keyStates[SDL_SCANCODE_DOWN] && !keyStates[SDL_SCANCODE_UP]) {
            player->setDirection(Down); 
            if (posY < maxY) {
                player->setPosY(posY + speed);
                isMoving = true;
            }
        }

        if (keyStates[SDL_SCANCODE_LEFT] && !keyStates[SDL_SCANCODE_RIGHT]) {
            player->setDirection(Left); 
            if (posX > minX) {
                player->setPosX(posX - speed);
                isMoving = true;
            }
        }

        if (keyStates[SDL_SCANCODE_RIGHT] && !keyStates[SDL_SCANCODE_LEFT]) {
            player->setDirection(Right); 
            if (posX < maxX) {
                player->setPosX(posX + speed);
                isMoving = true;
            }
        }
    }

    player->setMoving(isMoving);
}

void playerAttack(const Uint8 *keyStates, Player *player, Uint32 currentTime) {
    if (!player->getAttacking() && keyStates[SDL_SCANCODE_SPACE] && currentTime - player->getLastAttack() >= player->getAttackCooldown()) {
        int playerDirection = player->getDirection();
        int attackDistance = 20;
        int posX = player->getPosX();
        int posY = player->getPosY();
        int minX = player->getMinX();
        int maxX = player->getMaxX();
        int minY = player->getMinY();
        int maxY = player->getMaxY();

        // Update position based on direction (if attack involves movement)
        switch (playerDirection) {
            case Down: 
                if (posY + attackDistance <= maxY) {
                    player->setPosY(posY + attackDistance);
                }
                break;
            case Right: 
                if (posX + attackDistance <= maxX) {
                    player->setPosX(posX + attackDistance);
                }
                break;
            case Up: 
                if (posY - attackDistance >= minY) {
                    player->setPosY(posY - attackDistance);
                }
                break;
            case Left: 
                if (posX - attackDistance >= minX) {
                    player->setPosX(posX - attackDistance);
                }
                break;
        }

        // Set attack state
        player->setAttacking(true);
        player->setCurrentFrame(0);
        player->setLastAttack(currentTime);
    }

    // End attack when animation completes
    if (player->getAttacking() && player->getCurrentFrame() == 3) {
        player->setAttacking(false);
        player->setCurrentFrame(0);
    }
}

void handleSlimeActions(std::vector<Slime*> &slimes, Player *player) {
    for (auto &slime : slimes) {
        if (slime->getAlive()) {
            slimeMovement(slime, player, slimes); // Handles alive slimes
        } else {
            slimeDeath(slime); // Handles dead slimes
        }
    }
}

bool isBlocked(int nextX, int nextY, Slime *currentSlime, const std::vector<Slime*> &slimes) {
    for (Character *otherSlime : slimes) {
        if (otherSlime == currentSlime) continue;

        SDL_Rect otherBox = otherSlime->getBoundingBox();
        SDL_Rect nextBox = { nextX, nextY, currentSlime->getFrameWidth(), currentSlime->getFrameHeight() };

        // Check if the next position intersect with another another slime's bounding box
        if (SDL_HasIntersection(&nextBox, &otherBox)) {
            return true;
        }
    }
    return false;
}

void slimeMovement(Slime *slime, Player *player, const std::vector<Slime*> &slimes) {
    if (!player->getAlive()) return;

    slime->setMoving(true);

    SDL_Point playerCenter = player->getBoundingBoxCenter();
    SDL_Point slimeCenter = slime->getBoundingBoxCenter();

    int slimeSpeed = slime->getSpeed();
    int slimePosX = slime->getPosX();
    int slimePosY = slime->getPosY();

    // Calculate the difference between the slime and player positions
    int deltaX = playerCenter.x - slimeCenter.x;
    int deltaY = playerCenter.y - slimeCenter.y;
    
    if (slimeCenter.y < playerCenter.y) {
        int moveY = std::min(slimeSpeed, deltaY); // Move only as much as needed
        if (!isBlocked(slimePosX, slimePosY + moveY, slime, slimes)) {
            slime->setPosY(slimePosY + moveY);
            slime->setDirection(Down); 
        }
    } else if (slimeCenter.y > playerCenter.y) {
        int moveY = std::min(slimeSpeed, -deltaY); 
        if (!isBlocked(slimePosX, slimePosY - moveY, slime, slimes)) {
            slime->setPosY(slimePosY - moveY);
            slime->setDirection(Up); 
        }
    }

    if (slimeCenter.x < playerCenter.x) {
        int moveX = std::min(slimeSpeed, deltaX);
        if (!isBlocked(slimePosX + moveX, slimePosY, slime, slimes)) {
            slime->setPosX(slimePosX + slimeSpeed);
            slime->setDirection(Right); 
        }
    } else if (slimeCenter.x > playerCenter.x) {
        int moveX = std::min(slimeSpeed, -deltaX);
        if (!isBlocked(slimePosX - moveX, slimePosY, slime, slimes)) {
            slime->setPosX(slimePosX - slimeSpeed);
            slime->setDirection(Left); 
        }
    }
}

void slimeDeath(Slime *slime) {
    slime->setMoving(false);

    // Move the slime off-screen when the death animation is finished
    if (slime->getCurrentFrame() == 4) {
        slime->setPosX(-1000); 
    }
}

void handleCollisions(Player *player, std::vector<Slime*> &slimes, int &playerPoints, Uint32 currentTime) {
    for (auto &slime : slimes) {
        if (playerColliding(player, slime)) {
            std::cout << "Player Colliding" << std::endl;
            attackSlime(player, slime, playerPoints);
        } 

        if (slime->getAlive() && slimeColliding(player, slime)) {
            std::cout << "Slime Colliding" << std::endl;
            slimeAttacks(player, slime, currentTime);
        } else if (slime->getAttacking()) {
            slime->setAttacking(false);
            slime->setSpeed(2);
            slime->setFrameTime(120);
        }
    }
}

void slimeAttacks(Player *player, Slime *slime, Uint32 currentTime) {
    if (!slime->getAttacking()) {
        slime->setAttacking(true);
        slime->setCurrentFrame(0);
        slime->setSpeed(4);
    } else if (slime->getAttacking() && slime->getCurrentFrame() == 0) {
        slime->setAttacking(false);
        slime->setSpeed(2);
    }

    SDL_Point playerCenter = player->getBoundingBoxCenter();
    SDL_Point slimeCenter = slime->getBoundingBoxCenter();

    if (currentTime - slime->getLastAttack() >= slime->getAttackCooldown()  && player->getAlive()) {
        if (std::abs(slimeCenter.x - playerCenter.x) <= 10 && std::abs(slimeCenter.y - playerCenter.y) <= 10) {
            player->setFlashing(true);
            player->setHealth(player->getHealth() - 20);
            slime->setLastAttack(currentTime);

            if (player->getHealth() <= 0) {
                player->setAlive(false);
                player->setCurrentFrame(0);
            }
        }
    }
}

bool playerColliding(Player *player, Slime *slime) {
    int playerDirection = player->getDirection();
    SDL_Rect playerBox = player->getBoundingBox();
    SDL_Rect slimeBox = slime->getBoundingBox();

    if (playerDirection == Down && slimeBox.y + slimeBox.h <= playerBox.y + playerBox.h / 2) return false;
    if (playerDirection == Right && slimeBox.x + slimeBox.w <= playerBox.x + playerBox.w / 2) return false;
    if (playerDirection == Up && slimeBox.y >= playerBox.y + playerBox.h / 2) return false;
    if (playerDirection == Left && slimeBox.x >= playerBox.x + playerBox.w / 2) return false;

    return playerBox.x < slimeBox.x + slimeBox.w &&
           playerBox.x + playerBox.w > slimeBox.x &&
           playerBox.y < slimeBox.y + slimeBox.h &&
           playerBox.y + playerBox.h > slimeBox.y;
}

bool slimeColliding(Player *player, Slime *slime) {
    SDL_Rect playerBox = player->getBoundingBox();
    SDL_Rect slimeBox = slime->getBoundingBox();

    return playerBox.x < slimeBox.x + slimeBox.w &&
           playerBox.x + playerBox.w > slimeBox.x &&
           playerBox.y < slimeBox.y + slimeBox.h &&
           playerBox.y + playerBox.h > slimeBox.y;
}

void attackSlime(Player *player, Slime *slime, int &playerPoints) {
    if (player->getAttacking() && slime->getAlive() && player->getCurrentFrame() < 2) {
        std::cout << "Slime takes a hit" << std::endl;
        std::cout << "Slime current frame: " << slime->getCurrentFrame() << std::endl;
        slime->setHealth(0); // One shot
        slime->setCurrentFrame(0);
        slime->setAlive(false);
        playerPoints += 10;
    }
}

void updateFrames(Player *player, std::vector<Slime*> &slimes, Uint32 currentTime) {
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
    TTF_Font* font = TTF_OpenFont("./ThaleahFat.ttf", 32);
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

void renderHealth(SDL_Renderer* renderer, int playerHealth) {
    TTF_Font* font = TTF_OpenFont("./ThaleahFat.ttf", 32);
    if (!font) {
        std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    // Convert score to text
    std::string scoreText = "Health: " + std::to_string(playerHealth);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), { 255, 255, 255 });
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Get text dimensions
    SDL_Rect textRect = { 1000 - textSurface->w - 10, 10, textSurface->w, textSurface->h }; // Position at the top right corner

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
    int posX = (SCREEN_WIDTH - textSurface->w) / 2;
    int posY = (SCREEN_HEIGHT - textSurface->h) / 2;
    SDL_Rect textRect = { posX, posY, textSurface->w, textSurface->h };

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

    int posX = (SCREEN_WIDTH - textSurface->w) / 2;
    int posY = (SCREEN_HEIGHT - textSurface->h) / 2;
    SDL_Rect textRect = { posX, posY, textSurface->w, textSurface->h };

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

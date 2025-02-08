#include "init.h"
#include "slime.h"
#include "player.h"
#include "health.h"
#include "dust.h"
#include "ui.h"

// Constants for game configuration
#define MAXSLIMES 15
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#define TITLE "Slime Boy"
#define PATHTOPLAYER "./sprites/characters/player.png"
#define PATHTOSLIME "./sprites/characters/slime.png"
#define PATHTODUST "./sprites/particles/dust_particles_01.png"
#define PATHTOFONT "./ThaleahFat.ttf"
#define PLAYERSCALE 3
#define SLIMESCALE 2

// Enumarator to track different game states
enum GameState {
    GameStart,
    Playing,
    GameOver
};

// --------------------------- FUNCTION DECLARATIONS ---------------------------
void processGameEvents(SDL_Event &event, bool &running);
bool isPlayerCollidingWithSlime(Player *player,Slime *slime);
bool isSlimeCollidingWithPlayer(Player *player, Slime *slime);
void handlePlayerActions(const Uint8 *keyStates, Player *player, Uint32 currentTime, Dust *dust);
void handleSlimeActions(std::vector<Slime*> &slimes, Player *player);
void handleCollisions(Player *player, std::vector<Slime*> &slimes, int &playerPoints, Uint32 currentTime);
void updateFrames(Dust *dust, Player *player, std::vector<Slime*> &slimes, Uint32 currentTime); 
void renderBoundingBox(SDL_Renderer *renderer, Character *character, SDL_Color color); 

int main(int argc, char *argv[]) {
    SDL_Window* gameWindow = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Set random seed 
    srand(static_cast<unsigned int>(time(0)));

    // Initiaize SDL and create window
    if (!init(gameWindow, renderer, TITLE, SCREEN_WIDTH,SCREEN_HEIGHT)) {
        std::cout << "Failed to intialize SDL!" << std::endl;
        return -1;
    }

    // Textures 
    LTexture *map = new LTexture(renderer);
    LTexture dustTexture(renderer);
    LTexture playerTexture(renderer);
    LTexture slimeTexture(renderer);
    LTexture healthTexture(renderer);

    // UI setup
    UI *uI = new UI(PATHTOFONT, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Potions setup
    std::string healthPath = "./sprites/potions/Potion 3.png";
    HealthPotion *healthPotion = new HealthPotion(healthTexture, healthPath, "Health Potion", 20);
    healthPotion->setSize(32, 32);
    healthPotion->loadMedia();

    // Map setup
    std::string mapPath = "./map.png";
    if (!map->loadFromFile(mapPath)) {
        std::cout << "Failed to load texture image " << mapPath << std::endl;
        return -1;
    }

    // Dust setup
    std::vector<SDL_Rect> dustClips(4);
    Dust *dust = new Dust(dustTexture, 100, 200, 12, 12, 2, PATHTODUST);
    dust->loadMedia();
    dust->clipDust(dustClips);

    // Player setup
    std::vector<SDL_Rect> playerClips(6);
    Player *player = new Player(
        100, playerTexture, PATHTOPLAYER, playerClips, 48, 48, 0, 8, 0, 0, false,
        false, SDL_FLIP_NONE, PLAYERSCALE, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 
        80, true, 24, 300, 6
    );

    int playerPoints = 0;

    // Set player boundaries
    player->setMinX(-player->getBoundingBox().w / 2);
    player->setMinY(-player->getBoundingBox().w / 2);
    player->setMaxX(SCREEN_WIDTH - player->getBoundingBox().w);
    player->setMaxY(SCREEN_HEIGHT - player->getBoundingBox().h);
    
     // Slime setup
    std::vector<Slime*> slimes;
    std::vector<std::vector<SDL_Rect>> slimeClips(MAXSLIMES, std::vector<SDL_Rect>(7));

    for (int i = 0; i < MAXSLIMES; i++) {
        int randomSpeed = (std::rand() % 3) + 2;
        Slime *slime = new Slime(
            20, slimeTexture, PATHTOSLIME, slimeClips[i], 32, 32, 0, 2, 0, 4,
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
     
    GameState gameState = GameState::GameStart;
    bool running = true;
    Uint32 currentTime;

    // Index of health potion holder
    int healthPotionHolder = -1;

    // Game event loop
    while (running) {
        // Handle Game Start
        if (gameState == GameState::GameStart) {
            uI->renderStartScreen(renderer, running);
            if (!running) {
                break; // Exit if user quits
            } else {
                gameState = GameState::Playing; // Start the game if Enter is pressed
            }
        }
        
        // Handle Game Over
        else if (gameState == GameState::GameOver) {
            uI->renderEndScreen(renderer, running, playerPoints);
            if (!running) {
                break;  // Exit the loop if user quits
            }

            // Reset states
            player->resetStates();
            for (auto &slime : slimes) slime->resetStates();
            healthPotion->resetStates();
            
            healthPotionHolder = -1;
            playerPoints = 0;

            gameState = GameState::Playing;  // Go back to GameStart after GameOver
        }
        
        // Handle Playing
        else if (gameState == GameState::Playing) {
            SDL_Event event;
            processGameEvents(event, running);

            currentTime = SDL_GetTicks();
            const Uint8 *keyStates = SDL_GetKeyboardState(NULL);
            player->setMoving(false);
            
            // Handle slime respawn
            for (auto &slime : slimes) {
                slime->respawnTimer();
                slime->checkRespawn();
            }

            // Handle movements and collisions
            if (player->getAlive()) handlePlayerActions(keyStates, player, currentTime, dust);
            handleSlimeActions(slimes, player);
            handleCollisions(player, slimes, playerPoints, currentTime);

            // Handle potions
            healthPotion->spawnPotion(player, slimes, healthPotionHolder);
            healthPotion->playerUsesPotion(healthPotion, player, healthPotionHolder);
            
            updateFrames(dust, player, slimes, currentTime);
            
            player->updateAnimation();
            player->spriteClip();
            for (auto &slime : slimes) {
                slime->updateAnimation();
                slime->spriteClip();
            }

            // Clear screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            // Render map
            map->render(0, 0, nullptr, 1, SDL_FLIP_NONE);

            // Set health potion position and render
            healthPotion->setPosition(healthPotion, slimes, healthPotionHolder);
            if (healthPotion->getIsVisible() ) {
                healthPotion->texture.render(healthPotion->getX(), healthPotion->getY(), nullptr, 2, SDL_FLIP_NONE);
            }

            // Player will flash red if attacked by slime or blue if recovering health 
            player->updateTint(currentTime);

            // Render player and slimes
            if (player->getMoving()) {
                dust->render(renderer, dustClips);
            }
            player->render(renderer, playerClips);
            // renderBoundingBox(renderer, player, SDL_Color{ 255, 0, 0, 255 });

            for (auto const &slime : slimes) {
                slime->render(renderer, slime->getClips());
                // renderBoundingBox(renderer, slime, SDL_Color{ 0, 0, 255, 255 });
            }

            // Render points and health
            uI->renderText(renderer, "Health: " + std::to_string(player->getHealth()), 15, 5, 32, false, running);
            uI->renderText(renderer, "Score: " + std::to_string(playerPoints), 15, 30, 32, false, running);

            SDL_RenderPresent(renderer);
            SDL_Delay(16);

            // End game if player death animation has completed
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

// --------------------------- FUNCTION DEFINITIONS ---------------------------

// Handle SDL events
void processGameEvents(SDL_Event &event, bool &running) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false; // Stop game loop if the user closes the window
        }
    }
}

// Handle all player actions, including movement and attacks
void handlePlayerActions(const Uint8 *keyStates, Player *player, Uint32 currentTime, Dust *dust) {
    player->movement(keyStates, player, dust);
    player->attack(keyStates, player, currentTime);
}

// Handle all slime actions 
void handleSlimeActions(std::vector<Slime*> &slimes, Player *player) {
    for (auto &slime : slimes) {
        if (slime->getAlive()) {
            slime->movement(player, slimes); // Handle movement for alive slimes
        } else {
            slime->handleDeath(); // Handle death for dead slimes
        }
    }
}

// handle collisions between the player and slimes
void handleCollisions(Player *player, std::vector<Slime*> &slimes, int &playerPoints, Uint32 currentTime) {
    for (auto &slime : slimes) {
        if (isPlayerCollidingWithSlime(player, slime)) {
            std::cout << "Player Colliding" << std::endl;
            player->handleDamage(slime, playerPoints); // Handle player attacking slime
        } 

        if (slime->getAlive() && isSlimeCollidingWithPlayer(player, slime)) {
            std::cout << "Slime Colliding" << std::endl;
            slime->attack(player, currentTime); // Handle slime attacking player
        } else if (slime->getAttacking()) {
            slime->setAttacking(false);
            slime->setSpeed(2);
            slime->setFrameTime(100);
        }
    }
}

// Check if player is colliding with slime
bool isPlayerCollidingWithSlime(Player *player, Slime *slime) {
    SDL_Rect playerBox = player->getBoundingBox();
    SDL_Rect slimeBox = slime->getBoundingBox();

    // Calculate the center points of the player and slime
    SDL_Point playerCenter = player->getBoundingBoxCenter();
    SDL_Point slimeCenter = slime->getBoundingBoxCenter();

    // Calculate the distance between the player and slime centers
    int dx = playerCenter.x - slimeCenter.x;
    int dy = playerCenter.y - slimeCenter.y;
    int distanceSquared = dx * dx + dy * dy;

    //  Range of 60
    if (distanceSquared > 3600) {
        return false; // Slime is too far away
    }

    // Check bounding box collision
    return playerBox.x < slimeBox.x + slimeBox.w &&
           playerBox.x + playerBox.w > slimeBox.x &&
           playerBox.y < slimeBox.y + slimeBox.h &&
           playerBox.y + playerBox.h > slimeBox.y;
}

// Check if slime is colliding with player
bool isSlimeCollidingWithPlayer(Player *player, Slime *slime) {
    SDL_Rect playerBox = player->getBoundingBox();
    SDL_Rect slimeBox = slime->getBoundingBox();

    return playerBox.x < slimeBox.x + slimeBox.w &&
           playerBox.x + playerBox.w > slimeBox.x &&
           playerBox.y < slimeBox.y + slimeBox.h &&
           playerBox.y + playerBox.h > slimeBox.y;
}

// Update animation frames for player and slimes
void updateFrames(Dust *dust, Player *player, std::vector<Slime*> &slimes, Uint32 currentTime) {
    // Update dust animation
    if (currentTime - dust->getLastFrame() >= dust->getFrameTime()) {
        dust->setCurrentFrame((dust->getCurrentFrame() + 1) % 3);
        dust->setLastFrame(currentTime);
    }

    // Update player animation
    if (currentTime - player->getLastFrame() >= player->getFrameTime()) {
        int frameCount = player->getAttacking() ? 4 : 6;
        player->setCurrentFrame((player->getCurrentFrame() + 1) % frameCount);
        player->updateLastFrame();
    }

    // Update slime animations
    for (auto &slime : slimes) {
        if (currentTime - slime->getLastFrame() >= slime->getFrameTime()) {
            int frameCount = slime->getAlive() ? 4 : 5;
            slime->setCurrentFrame((slime->getCurrentFrame() + 1) % frameCount);
            slime->updateLastFrame();
        }
    }
}

// Render bounding box of character
void renderBoundingBox(SDL_Renderer *renderer, Character *character, SDL_Color color) {
    SDL_Rect boundingBox = character->getBoundingBox();
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &boundingBox);
}
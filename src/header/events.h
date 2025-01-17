#include "includes.h"

// Handle key down events
void movePlayer(
    SDL_Event &event, 
    int &x, 
    int &y, 
    int maxWidth, 
    int maxHeight,
    int &playerDirection,
    bool &isMoving
) {
    int moveDistance = 12;
    
    switch(event.key.keysym.sym) {
        case SDLK_w:
            std::cout << "W key was pressed" << std::endl;
            isMoving = true;
            if (y > 0) {
                playerDirection = 2;
                y -= moveDistance;
            }
            break;
        case SDLK_a:
            std::cout << "A key was pressed" << std::endl;
            isMoving = true;
            if (x > 0) {
                playerDirection = 3;
                x -= moveDistance;
            }
            break;
        case SDLK_s:
            std::cout << "S key was pressed" << std::endl;
            isMoving = true;
            if (y < 600) {
                playerDirection = 0;
                y += moveDistance;
            }
            break;
        case SDLK_d:
            std::cout << "D key was pressed" << std::endl;
            isMoving = true;
            if (x < 600) {
                playerDirection = 1;
                x += moveDistance;
            }
            break;
        default:
           std::cout << "Some key was pressed" << std::endl;
    }
}

void keyUp(SDL_Event &event, bool &isMoving) {
    switch(event.key.keysym.sym) {
        case SDLK_w:
            std::cout << "W key was released" << std::endl;
            isMoving = false;
            break;
        case SDLK_a:
            std::cout << "A key was released" << std::endl;
            isMoving = false;
            break;
        case SDLK_s:
            std::cout << "S key was released" << std::endl;
            isMoving = false;
            break;
        case SDLK_d:
            std::cout << "D key was released" << std::endl;
            isMoving = false;
            break;
        default:
           std::cout << "Some key was released" << std::endl;
    }
}
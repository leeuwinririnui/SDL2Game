#include "includes.h"

// Handle key down events
void movePlayer(
    SDL_Event& event, 
    int& x, 
    int& y, 
    int maxWidth, 
    int maxHeight
) {
    int moveDistance = 12;
    switch(event.key.keysym.sym) {
        case SDLK_w:
            std::cout << "W key was pressed" << std::endl;
            if (y > 0)
                y -= moveDistance;
            break;
        case SDLK_a:
            std::cout << "A key was pressed" << std::endl;
            if (x > 0)
                x -= moveDistance;
            break;
        case SDLK_s:
            std::cout << "S key was pressed" << std::endl;
            if (y < maxHeight)
                y += moveDistance;
            break;
        case SDLK_d:
            std::cout << "D key was pressed" << std::endl;
            if (x < maxWidth)
                x += moveDistance;
            break;
        default:
           std::cout << "Some key was pressed" << std::endl;
    }
}

void keyUp(SDL_Event &event) {
    switch(event.key.keysym.sym) {
        case SDLK_w:
            std::cout << "W key was released" << std::endl;
            break;
        case SDLK_a:
            std::cout << "A key was released" << std::endl;
            break;
        case SDLK_s:
            std::cout << "S key was released" << std::endl;
            break;
        case SDLK_d:
            std::cout << "D key was released" << std::endl;
            break;
        default:
           std::cout << "Some key was released" << std::endl;
    }
}
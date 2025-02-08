#ifndef UI_H
#define UI_H

#include "includes.h"

class UI {
private:
    // Path to font
    std::string fontPath;
    int screenWidth, screenHeight;

public:
    UI(std::string fontPath, int screenWidth, int screenHeight) 
    : fontPath(fontPath), screenWidth(screenWidth), screenHeight(screenHeight) {}
    
    void renderStartScreen(SDL_Renderer *renderer, bool &running);
    void renderEndScreen(SDL_Renderer *renderer, bool &running, int &points);

    // Helper functions
    TTF_Font *loadFont(int fontSize);
    void renderText(SDL_Renderer *renderer, const std::string &text, int x, int y, int fontSize, bool inputNeeded, bool &running);
    void waitForUserInput(bool &waiting, bool &running);
};

#endif
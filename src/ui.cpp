#include "ui.h"

// Display the start screen
void UI::renderStartScreen(SDL_Renderer *renderer, bool &running) {
    TTF_Font *font = loadFont(48);
    if (!font) return;

    std::string message = "Press Enter to Start!";

    // Calcute text dimensions 
    int textWidth, textHeight;
    TTF_SizeText(font, message.c_str(), &textWidth, &textHeight);

    // Calculate position
    int x = (screenWidth - textWidth) / 2;
    int y = (screenHeight - textHeight) / 2;

    // Render text
    renderText(renderer, message.c_str(), x, y, 48, true, running);
}

// Display the game over screen
void UI::renderEndScreen(SDL_Renderer *renderer, bool &running, int &points) {
    TTF_Font *font = loadFont(48);
    if (!font) return;

    std::string message = "Game Over! Press Enter to Play Again";

    // Calcute text dimensions 
    int textWidth, textHeight;
    TTF_SizeText(font, message.c_str(), &textWidth, &textHeight);

    // Calculate position
    int x = (screenWidth - textWidth) / 2;
    int y = (screenHeight - textHeight) / 2;

    // Render text
    renderText(renderer, message.c_str(), x, y, 48, true, running);
}

// Function to load font
TTF_Font* UI::loadFont(int fontSize) {
    TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) {
        std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }
    return font;
}

// Render text to the screen
void UI::renderText(SDL_Renderer *renderer, const std::string &text, int x, int y, int fontSize, bool inputNeeded, bool &running) {
    TTF_Font *font = loadFont(fontSize);
    if (!font) return;

    SDL_Color color = { 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    
    // Render text to x and y positions
    SDL_Rect textRect = { x, y, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    if (inputNeeded) {
        SDL_RenderPresent(renderer);
        bool waiting = true;

        while (waiting) {
            waitForUserInput(waiting, running);
        }
    }

    // Free resources
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

// Function to wait for user input
void UI::waitForUserInput(bool &waiting, bool &running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            waiting = false;
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_RETURN) {
                waiting = false;  
            } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                waiting = false;
                running = false;  // Quit the game
            }
        }
    }
}

#include "dust.h"

// Load image from file
bool Dust::loadMedia() {
    if (!texture.loadFromFile(path.c_str())) {
        std::cout << "Failed to load texture image " << path.c_str() << std::endl;
        return false;
    }
    return true;
}

// Handle clipping for dust animation
void Dust::clipDust(std::vector<SDL_Rect> &clips) {
    int x = 0;
    int y = 0;

    for (int i = 0; i < clips.size(); i++) {
        clips[i].x = x;
        clips[i].y = y;
        clips[i].w = 24;
        clips[i].h = 12;
        x += 12;
    }
}

// Render texture
void Dust::render(SDL_Renderer *renderer, std::vector<SDL_Rect> clips) {
    texture.render(
        x,
        y,
        &clips[currentFrame],
        scale,
        flip, 
        isVertical
    );
}
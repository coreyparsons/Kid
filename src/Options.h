#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

struct SDL_Window;
struct SDL_Renderer;
struct GameInput;
struct GameState;
struct Assets;
struct Vec2;

void optionsUpdateAndRender(SDL_Window* window, SDL_Renderer* renderer, GameInput gameInput, float &dt, GameState &gameState, bool &vsync);
void percentOption(SDL_Renderer* renderer, Assets assets, GameInput gameInput, Vec2 &textPos, float tableGap, std::string text, float &value);

#endif
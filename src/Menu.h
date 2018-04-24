#ifndef MENU_H
#define MENU_H

#include "main_game.h"
//#include "platform_layer.h"

enum BoardType
{
	BT_TUTORIAL,
	BT_JUMP,
	BT_CHANGE_BLOCK,
	BT_TELEPORT,
	BT_POWERUPS_UNLOCKED
};

struct MenuState
{
	bool init = false;
	bool isPausingGame = false;
	float amountParticles = 0.0f;
	float particleListSize = 1000.0f;
	Particle* particleList = new Particle[1000]();

	float soundVolume = 1.0f;
	float musicVolume = 0.2f;
	float snowIntensity = 0.4f;

	float dt = 1.0f / 60.0f;
	bool vsync = true;
	bool startVsync = true;

	bool dieClicked = false;
	bool quitClicked = false;

	BoardType boardType;
	bool tutorial = true;
};

struct SDL_Window;
struct SDL_Renderer;
struct GameInput;
struct GameState;

void menuUpdateAndRender(SDL_Window* window, SDL_Renderer* renderer, GameInput gameInput, float dt, GameState &gameState);
void renderMenuBackground(SDL_Renderer* renderer, Assets assets, MenuState &menuState, float dt);

#endif
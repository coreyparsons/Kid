#ifndef PLATFORM_LAYER_H
#define PLATFORM_LAYER_H

//all of the SDL things

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#include <stdlib.h> //for random
#include <time.h> //seed for random

#include "maths.h"
#include "main_game.h"
#include "upgrades.h"
#include "menu.h"

struct MouseButton
{
	bool left;
	bool middle;
	bool right;
	bool leftClicked;
	bool rightClicked;
};

//game specific
struct GameInput
{
	bool up;
	bool upPressed;
	bool down;
	bool left;
	bool right;
	bool action;
	bool actionPressed;
	bool back;
	bool backPressed;
	bool anyKeyDown;
	bool lostFocus;
	MouseButton mouseButton = {};
	Vec2 mousePos = {};
	Vec2 axisPos;
};

enum GameScreen
{
	GS_MAIN_GAME,
	GS_MENU,
	GS_UPGRADES,
	GS_OPTIONS,
	GS_BOARD,
};

//should not have assets (if i need to save gameState, i should not save assets, 
//although i would not need to save any of the mainState either (as i would only be able to save between games))
//maybe i need a struct called "saveableState" that allows me to save and load that particular struct

struct GameState 
{
	Assets assets;

	UpgradesState upgradesState;
	MainState mainState;
	MenuState menuState;

	bool gameRunning = true;
	GameScreen screen = GS_MAIN_GAME;
	GameScreen prevScreen = GS_MAIN_GAME;
};

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Rect;
struct SDL_Texture;
struct SDL_Color;

void drawRect(SDL_Renderer* renderer, SDL_Rect& rect, SDL_Color& colour);
void drawRect(SDL_Renderer* renderer, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b);
void drawOutlineRect(SDL_Renderer* renderer, SDL_Rect &rect, SDL_Color &colour);
void drawOutlineRect(SDL_Renderer* renderer, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b);
SDL_Texture* createTexture(char* filename, SDL_Renderer* renderer);

#endif
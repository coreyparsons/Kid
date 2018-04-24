#ifndef UPGRADES_H
#define UPGRADES_H

#include <string>
#include "sdl_ttf.h"

struct UpgradeBoxData
{
	float upgradeLevel = -1.0f;
	char* title = " ";
	char* description = " ";
};

struct UpgradesState
{
	bool firstLoop = true;

	float savedPoints = 0.0f;
	float savedPlayerMaxJumps = 0.0f;
	float savedMaxBlockChanges = 0.0f;
	float savedMaxTeleports = 0.0f;
	float savedExtraJumpIntensity = 0.0f;
	float savedCoinIntensity = 0.0f;
	float savedPickupIntensity = 0.0f;

	float highScore = 0.0f;
	float points = 0.0f;

	float playerMaxJumps = 0.0f;
	float maxBlockChanges = 0.0f;
	float maxTeleports = 0.0f;
	float extraJumpIntensity = 0.0f;
	float coinIntensity = 0.0f;
	float pickupIntensity = 0.0f;

	UpgradeBoxData upgradeBoxData = {};

	bool jumpTutorial = true;
	bool changeBlockTutorial = true;
	bool teleportTutorial = true;
	bool powerupsUnlock = true;
};

struct SDL_Window;
struct SDL_Renderer;
struct GameInput;
struct GameState;
struct Vec2;
struct Colour;
struct Assets;

void upgradesUpdateAndRender(SDL_Window* window, SDL_Renderer* renderer, GameInput gameInput, float dt, GameState &gameState);
bool button(SDL_Renderer* renderer, Assets assets, GameInput gameInput, Vec2 pos, Vec2 size, Colour colour, std::string text, TTF_Font* font);
bool imgButton(SDL_Renderer* renderer, Assets assets, GameInput gameInput, SDL_Texture* texture, Vec2 pos, Vec2 size, std::string text, TTF_Font* font);
bool imgButton(SDL_Renderer* renderer, Assets assets, GameInput gameInput, SDL_Texture* texture, Vec2 pos, Vec2 size);
bool textButton(SDL_Renderer* renderer, Assets assets, GameInput gameInput, Vec2 pos, Vec2 size, std::string text, TTF_Font* font);
SDL_Rect rectFromPosAndSize(Vec2 pos, Vec2 size);
void upgrade(SDL_Renderer* renderer, Assets assets, GameInput gameInput, SDL_Rect rect, float &upgradeLevel, float &points, SDL_Texture* iconTexture);
Vec2 getTextSize(std::string text, TTF_Font* font);
float costOf(float upgradeLevel);
float getCostDiff(float oldUpgradeLevel, float newUpgradeLevel);
void upgradeBox(SDL_Renderer* renderer, Assets assets, float upgradeLevel, std::string title, std::string description);
void upgradeBox(SDL_Renderer* renderer, Assets assets, UpgradeBoxData upgradeBoxData);

#endif
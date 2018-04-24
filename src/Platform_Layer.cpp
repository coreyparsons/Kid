//sdl things 
#include "SDL.h"
#include "SDL_Image.h"
#include "SDL_Mixer.h"
#include "SDL_ttf.h"

#include "platform_layer.h"
#include "main_game.h"
#include "menu.h"
#include "options.h"

//for debugging
#include <iostream>
using namespace std;

#define GAME_SHOW_FPS 0
#define EMBEDED_APPLICATION 0

/*
TODO (Platform layer):

- do error handling for SDL things
- look into render size 0, 0 (makes the render size the same as the window)
- for some reason button pressed doesn't work for the first time on bradey's laptop

*/

void handleEvents(SDL_Event &event, SDL_Window* window, GameInput &gameInput, bool &gameRunning, SDL_GameController* controller);
void updateAndRender(SDL_Window* window, SDL_Renderer* renderer, GameInput &gameInput, float &dt, GameState &gameState, bool &vsync, BoardType boardType);
void limitFps(float dt, unsigned long long &timerStart, bool vsync, SDL_Window* window);

void toggleFullscreen(SDL_Window* window);

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	GameState gameState;

#if EMBEDED_APPLICATION
	SDL_RWops* saveFile = SDL_RWFromFile("save_data.bin", "rb");
#else
	std::string pathString = SDL_GetPrefPath("", "Kid");
	SDL_RWops* saveFile = SDL_RWFromFile(pathString.append("save_data.bin").c_str(), "rb");
#endif
	if (saveFile == NULL)
	{
#if EMBEDED_APPLICATION
		saveFile = SDL_RWFromFile("save_data.bin", "wb");
#else
		saveFile = SDL_RWFromFile(pathString.c_str(), "wb");
#endif
	}
	else
	{
		SDL_RWread(saveFile, &gameState.menuState, sizeof(MenuState), 1);
		SDL_RWread(saveFile, &gameState.upgradesState, sizeof(UpgradesState), 1);
	}

	SDL_RWclose(saveFile);

	//heap things
	gameState.menuState.init = false;
	gameState.upgradesState.firstLoop = true;
	gameState.menuState.amountParticles = 0.0f;
	gameState.menuState.particleList = new Particle[(int)gameState.menuState.particleListSize]();

	//window info
	float &dt = gameState.menuState.dt;
	bool &vsync = gameState.menuState.vsync;
	gameState.menuState.startVsync = vsync;
	GameInput gameInput = {};
	unsigned long long timerStart = SDL_GetPerformanceCounter();
	SDL_GameController* controller = {};

	//init screen
	SDL_Window* window = SDL_CreateWindow("Kid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 960, 540, SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP); //960, 540
	SDL_Renderer* renderer;
	if (vsync)
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
		SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
		SDL_GetCurrentDisplayMode(0, &mode);
		dt = 1.0f / (float)mode.refresh_rate;
	}
	else
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	SDL_RenderSetLogicalSize(renderer, 1920, 1080);
	SDL_SetWindowIcon(window, IMG_Load("resources/icon.png"));

	Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024); //lower chunk size = less delay
	Mix_Volume(-1, 100);
	TTF_Init();

	//windows specific things (i think)
	srand(time(0));

	SDL_Event event;
	gameState.screen = GS_MENU;

	Mix_Volume(-1, MIX_MAX_VOLUME * gameState.menuState.soundVolume);
	Mix_VolumeMusic(MIX_MAX_VOLUME * gameState.menuState.musicVolume);

	while (gameState.gameRunning)
	{
		handleEvents(event, window, gameInput, gameState.gameRunning, controller);
		
		//temp, quick exit
#if 0
		if (gameInput.backPressed)
			gameState.gameRunning = false;
#endif
#if 0
		if (gameInput.mouseButton.leftClicked)
		{
			if (fps == 60.0f)
				fps = 30.0f;
			else
				fps = 60.0f;
			dt = 1.0f / fps;
		}
#endif

		updateAndRender(window, renderer, gameInput, dt, gameState, vsync, gameState.menuState.boardType);

		limitFps(dt, timerStart, gameState.menuState.startVsync, window);
	}

	if (gameState.mainState.player.init)
	{
		gameState.upgradesState.points += gameState.mainState.currentScore;
	}
#if EMBEDED_APPLICATION
	saveFile = SDL_RWFromFile("save_data.bin", "wb");
#else
	saveFile = SDL_RWFromFile(pathString.c_str(), "wb");
#endif

	SDL_RWwrite(saveFile, &gameState.menuState, sizeof(MenuState), 1);
	SDL_RWwrite(saveFile, &gameState.upgradesState, sizeof(UpgradesState), 1);
	SDL_RWclose(saveFile);

	SDL_Quit();
	return 0;
}

void updateAndRender(SDL_Window* window, SDL_Renderer* renderer, GameInput &gameInput, float &dt, GameState &gameState, bool &vsync, BoardType boardType)
{
	switch (gameState.screen)
	{
	case GS_MAIN_GAME:
	{
		mainUpdateAndRender(window, renderer, gameInput, dt, gameState);
		break;
	}
	case GS_UPGRADES:
	{
		upgradesUpdateAndRender(window, renderer, gameInput, dt, gameState);
		break;
	}
	case GS_MENU:
	{
		menuUpdateAndRender(window, renderer, gameInput, dt, gameState);
		break;
	}
	case GS_OPTIONS:
	{
		optionsUpdateAndRender(window, renderer, gameInput, dt, gameState, vsync);
		break;
	}
	case GS_BOARD:
	{
		boardUpdateAndRender(window, renderer, gameInput, dt, gameState, boardType);
		break;
	}
	default:
	{
		drawRect(renderer, 0, 0, 1920, 1080, 255, 0, 0);
		SDL_RenderPresent(renderer);
		break;
	}
	}
}

void handleEvents(SDL_Event &event, SDL_Window* window, GameInput &gameInput, bool &gameRunning, SDL_GameController* controller)
{
	bool prevLeft = gameInput.mouseButton.left;
	bool prevRight = gameInput.mouseButton.right;
	bool prevAction = gameInput.action;
	bool prevBack = gameInput.back;
	bool prevUp = gameInput.up;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			gameRunning = false;
		else if (event.type == SDL_KEYDOWN)
		{
			gameInput.anyKeyDown = true;
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			{
				gameInput.back = true;
				break;
			}
			case SDLK_w:
			{
				gameInput.up = true;
				break;
			}
			case SDLK_s:
			{
				gameInput.down = true;
				break;
			}
			case SDLK_a:
			{
				gameInput.left = true;
				break;
			}
			case SDLK_d:
			{
				gameInput.right = true;
				break;
			}
			case SDLK_SPACE:
			{
				gameInput.action = true;
				break;
			}
			case SDLK_F11:
			{
				toggleFullscreen(window);
				break;
			}
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			gameInput.anyKeyDown = false;
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
			{
				gameInput.up = false;
				break;
			}
			case SDLK_s:
			{
				gameInput.down = false;
				break;
			}
			case SDLK_a:
			{
				gameInput.left = false;
				break;
			}
			case SDLK_d:
			{
				gameInput.right = false;
				break;
			}
			case SDLK_SPACE:
			{
				gameInput.action = false;
				break;
			}
			case SDLK_ESCAPE:
			{
				gameInput.back = false;
				break;
			}
			}
		}
		else if (event.type == SDL_CONTROLLERDEVICEADDED)
		{
			for (int i = 0; i < SDL_NumJoysticks(); i++)
			{
				if (SDL_IsGameController(i))
				{
					controller = SDL_GameControllerOpen(i);
				}
			}
		}
		else if (event.type == SDL_CONTROLLERBUTTONDOWN)
		{
			switch (event.cbutton.button)
			{
			case SDL_CONTROLLER_BUTTON_Y:
			{
				toggleFullscreen(window);
				break;
			}
			case SDL_CONTROLLER_BUTTON_A:
			{
				gameInput.action = true;
				break;
			}
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
			{
				gameInput.down = true;
				break;
			}
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			{
				gameInput.left = true;
				break;
			}
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			{
				gameInput.right = true;
				break;
			}
			case SDL_CONTROLLER_BUTTON_DPAD_UP:
			{
				gameInput.up = true;
				break;
			}
			case SDL_CONTROLLER_BUTTON_START:
			{
				gameInput.back = true;
				break;
			}
			}
		}
		else if (event.type == SDL_CONTROLLERBUTTONUP)
		{
			switch (event.cbutton.button)
			{
			case SDL_CONTROLLER_BUTTON_A:
			{
				gameInput.action = false;
				break;
			}
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
			{
				gameInput.down = false;
				break;
			}
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			{
				gameInput.left = false;
				break;
			}
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			{
				gameInput.right = false;
				break;
			}
			case SDL_CONTROLLER_BUTTON_DPAD_UP:
			{
				gameInput.up = false;
				break;
			}
			case SDL_CONTROLLER_BUTTON_START:
			{
				gameInput.back = false;
				break;
			}
			}
		}
		else if (event.type == SDL_CONTROLLERAXISMOTION)
		{
			switch (event.caxis.axis)
			{
			case SDL_CONTROLLER_AXIS_LEFTX:
			{
				if (event.caxis.value >= 0)
					gameInput.axisPos.x = event.caxis.value / 32767.0f;
				else
					gameInput.axisPos.x = event.caxis.value / 32768.0f;
				break;
			}
			case SDL_CONTROLLER_AXIS_LEFTY:
			{
				if (event.caxis.value >= 0)
					gameInput.axisPos.y = event.caxis.value / 32767.0f;
				else
					gameInput.axisPos.y = event.caxis.value / 32768.0f;
				break;
			}
			}
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
			{
				gameInput.mouseButton.left = true;
				break;
			}
			case SDL_BUTTON_MIDDLE:
			{
				gameInput.mouseButton.middle = true;
				break;
			}
			case SDL_BUTTON_RIGHT:
			{
				gameInput.mouseButton.right = true;
				break;
			}
			}
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
			{
				gameInput.mouseButton.left = false;
				break;
			}
			case SDL_BUTTON_MIDDLE:
			{
				gameInput.mouseButton.middle = false;
				break;
			}
			case SDL_BUTTON_RIGHT:
			{
				gameInput.mouseButton.right = false;
				break;
			}
			}
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			gameInput.mousePos = { (float)event.motion.x, (float)event.motion.y };
		}
		else if (event.type == SDL_WINDOWEVENT)
		{
			if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
			{
				gameInput.lostFocus = true;
			}
			else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
			{
				gameInput.lostFocus = false;
			}
		}
	}

	((!prevLeft) && gameInput.mouseButton.left) ? gameInput.mouseButton.leftClicked = true : gameInput.mouseButton.leftClicked = false;
	((!prevRight) && gameInput.mouseButton.right) ? gameInput.mouseButton.rightClicked = true : gameInput.mouseButton.rightClicked = false;
	((!prevAction) && gameInput.action) ? gameInput.actionPressed = true : gameInput.actionPressed = false;
	((!prevBack) && gameInput.back) ? gameInput.backPressed = true : gameInput.backPressed = false;
	((!prevUp) && gameInput.up) ? gameInput.upPressed = true : gameInput.upPressed = false;
}

void limitFps(float dt, unsigned long long &timerStart, bool vsync, SDL_Window* window)
{
	if (!vsync || ((SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED))
	{
		float delaySeconds = dt - ((float)(SDL_GetPerformanceCounter() - timerStart) / (float)(SDL_GetPerformanceFrequency()));

		if (delaySeconds > 0)
		{
			SDL_Delay((int)(delaySeconds * 1000.0f));
		}
		while (dt > ((float)(SDL_GetPerformanceCounter() - (timerStart)) / (float)(SDL_GetPerformanceFrequency()))){}
	}

#if GAME_SHOW_FPS
	cout << "FPS: " << roundToI(1.0f / ((float)(SDL_GetPerformanceCounter() - timerStart) / (float)SDL_GetPerformanceFrequency())) << endl;
#endif

	timerStart = SDL_GetPerformanceCounter();
}

void toggleFullscreen(SDL_Window* window)
{
	if ((SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP) == SDL_WINDOW_FULLSCREEN_DESKTOP)
	{
		SDL_SetWindowFullscreen(window, 0);
	}
	else
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
}


void drawRect(SDL_Renderer* renderer, SDL_Rect &rect, SDL_Color &colour)
{
	SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
	SDL_RenderFillRect(renderer, &rect);
}

void drawRect(SDL_Renderer* renderer, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b)
{
	SDL_Rect rect = { x, y, w, h };
	SDL_Color colour = { r, g, b };
	drawRect(renderer, rect, colour);
}

void drawOutlineRect(SDL_Renderer* renderer, SDL_Rect &rect, SDL_Color &colour)
{
	SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
	SDL_RenderDrawRect(renderer, &rect);
	SDL_Rect rect2 = { rect.x + 1, rect.y + 1, rect.w - 2, rect.h - 2 };
	SDL_RenderDrawRect(renderer, &rect2);
	rect2 = { rect2.x + 1, rect2.y + 1, rect2.w - 2, rect2.h - 2 };
	SDL_RenderDrawRect(renderer, &rect2);
}

void drawOutlineRect(SDL_Renderer* renderer, int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b)
{
	SDL_Rect rect = { x, y, w, h };
	SDL_Color colour = { r, g, b };
	drawOutlineRect(renderer, rect, colour);
}

//seems to have memory leak (not a huge problem)
SDL_Texture* createTexture(char* filename, SDL_Renderer* renderer)
{
	SDL_Surface* image = IMG_Load(filename);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_FreeSurface(image);
	return texture;
}

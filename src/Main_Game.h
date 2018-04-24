#ifndef MAIN_GAME_H
#define MAIN_GAME_H

#include "sdl_ttf.h"
#include "string"
#include "sdl_mixer.h"
#include "maths.h"

struct Colour
{
	unsigned char r, g, b;
};

enum EntityType
{
	ET_PLAYER,
	ET_BOX,
	ET_FLOOR,
	ET_STATIC_BOX,
	ET_WALKTHROUGH_BOX,
	ET_COIN,
	ET_JUMP_PICKUP,
	ET_CHANGE_BLOCK_PICKUP,
	ET_TELEPORT_PICKUP,
};

struct Entity //MAKE THE ENTITY SYSTEM BETTER
{
	EntityType type;
	Vec2 pos;
	Vec2 size;
	Vec2 vel;
	Vec2 accel;
	Colour colour;
	bool init = false;
};

struct Particle
{
	bool init;

	Vec2 pos;
	Vec2 vel;

	Vec2 startSize;
	Vec2 endSize;

	float startAngle;
	float endAngle;

	float lifeTime;
	float maxLifeTime;
	SDL_Texture* texture;
};

struct Camera
{
	Vec2 pos;
	Vec2 size;
};

enum Wall
{
	W_FLOOR,
	W_RIGHT,
	W_LEFT,
};

struct Assets
{
	bool init = false;
	TTF_Font* goodTimesFont;
	TTF_Font* goodTimesFontLarge;

	SDL_Texture* upgradesBackground;
	SDL_Texture* playerTexture;
	SDL_Texture* playerEyeTexture;
	SDL_Texture* playerBlinkTexture;
	SDL_Texture* jumpIconTexture;
	SDL_Texture* changeBlockIconTexture;
	SDL_Texture* teleportIconTexture;
	SDL_Texture* plusTexture;
	SDL_Texture* minusTexture;
	SDL_Texture* coinTexture;
	SDL_Texture* jumpIntensityIconTexture;
	SDL_Texture* jumpPickupTexture;
	SDL_Texture* changeBlockPickupTexture;
	SDL_Texture* teleportPickupTexture;
	SDL_Texture* coinIconTexture;
	SDL_Texture* pickupsIconTexture;
	SDL_Texture* coinBackgroundTexture;
	SDL_Texture* pickupBackgroundTexture;
	SDL_Texture* snowflakeTexture;
	SDL_Texture* dustTexture;
	SDL_Texture* sparkleTexture;
	SDL_Texture* spiralCloudTexture;
	SDL_Texture* blueSwirlTexture;
	SDL_Texture* lavaSpewTexture;
	SDL_Texture* bloodTexture;

	//things to implement
	SDL_Texture* tutorialBoard;
	SDL_Texture* jumpBoard;
	SDL_Texture* changeBlockBoard;
	SDL_Texture* teleportBoard;
	SDL_Texture* powerupsUnlockedBoard;

	Mix_Chunk* buttonSelectSound;
	Mix_Chunk* coinPickupSound;
	Mix_Chunk* powerupPickupSound;
	Mix_Chunk* teleportSound;
	Mix_Chunk* extraJumpSound;
	Mix_Chunk* changeBlockSound;
	Mix_Chunk* invalidSelectionSound;
	Mix_Chunk* squashSound;
	Mix_Chunk* lavaDeathSound;
	Mix_Music* gameMusic;
};

struct MainState
{
	//data
	bool init = false;

	Entity player = {};
	float playerJumpTimer = 0.0f;
	Wall playerJumpWall = W_FLOOR;
	Vec2 playerDrawSize = {};
	Vec2 playerEyePos = {};
	Vec2 playerEyeSize = {};
	float playerBlinkTimer = 0.0f;
	float playerBlinkTimerMax = 0.0f;

	Vec2 playerPrevEyeLooking = {};

	float playerJumpsLeft = 0.0f;
	float blockChanges = 0.0f;
	float teleports = 0.0f;

	float extraJumpTextEffectTime = 0.0f;
	float changeBlockTextEffectTime = 0.0f;
	float teleportTextEffectTime = 0.0f;

	float squashTimer = 0.0f;

	Entity floor = {};

	float entityListSize = 1000.0f;
	Entity* entityList = new Entity[1000]();

	float particleListSize = 2000.0f;
	Particle* particleList = new Particle[2000]();

	float fallingBoxTimer = 0.0f;
	float fallingBoxTimerMax = 1.0f;
	Camera camera = {};

	float amountCoins = 0.0f;
	float amountPickups = 0.0f;
	float pickupBackgroundSize = 0.0f;

	float lavaLevel = 0.0f;
	float lavaVel = 60.0f;

	float snowParticles = 0.0f;
	float lavaParticles = 0.0f;

	float deathTimer = 0.0f;

	float playerHighest = 0.0f;
	float currentScore = 0.0f;
	float currentTime = 0.0f;

	float tutorialTime = 0.0f;
};

struct SDL_Window;
struct SDL_Renderer;
struct GameInput;
struct GameState;
enum BoardType;
enum GameScreen;

void mainUpdateAndRender(SDL_Window* window, SDL_Renderer* renderer, GameInput gameInput, float dt, GameState &gameState);

bool testWall(float &collisionTime, float boxX, float boxY, float boxSizeY, float playerX, float playerY, float playerMovDiffX, float playerMovDiffY);
Entity &getEmptyValue(Entity *entityList, float entityListSize);
void addEntityToList(Entity entity, Entity *entityList, float entityListSize);
void moveWithinX(float &posX);
void renderText(SDL_Renderer* renderer, std::string string, TTF_Font* font, Vec2 pos, Vec2 size, Colour colour);
void renderTextWrapped(SDL_Renderer* renderer, std::string string, TTF_Font* font, Vec2 pos, Vec2 size, Colour colour, int wrapLength);
void renderText(SDL_Renderer* renderer, std::string string, TTF_Font* font, Vec2 pos, Colour colour);
bool collided(Entity entityA, Entity entityB);
bool pointInsideRect(Vec2 point, Vec2 rectPos, Vec2 rectSize);
SDL_Texture* loadTexture(SDL_Renderer* renderer, char* file);
void renderEntity(SDL_Renderer* renderer, Entity entity, SDL_Texture* texture, Camera camera);
bool entityOnScreen(Camera camera, Entity entity);
void renderEntity(SDL_Renderer* renderer, Entity entity, SDL_Texture* texture, SDL_Texture* backgroundTexture, float pickupBackgroundSize, Camera camera);
void makeParticle(Particle* &particleList, float particleListSize, Vec2 centerPoint, Vec2 startSize, Vec2 endSize, Vec2 vel, float maxLifeTime, SDL_Texture* texture);
void makeParticle(Particle* &particleList, float particleListSize, Vec2 centerPoint, Vec2 startSize, Vec2 endSize, float startAngle, float endAngle, Vec2 vel, float maxLifeTime, SDL_Texture* texture);
void renderParticles(Particle particleList[], float particleListSize, Camera camera, SDL_Renderer* renderer, float dt);
void boardUpdateAndRender(SDL_Window* window, SDL_Renderer* renderer, GameInput gameInput, float dt, GameState &gameState, BoardType boardType);
void doBoard(SDL_Renderer* renderer, GameInput gameInput, float dt, GameState &gameState, SDL_Texture* board, GameScreen gotoScreen, bool &tutorial);

#endif
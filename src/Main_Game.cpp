#include "platform_layer.h"
#include "sdl.h" 
#include "sdl_image.h"

#include "main_game.h"

#include <iostream>

/*

BUGS:

- wide objects will not collide correctly because of the way i have done the screen scrolling
  - the way that it is done now is to take everything and make it relative to the player at the center of the screen
  - i think a better way to do it is to make it relative to the box, and have it at the start of the screen (the box that is)
    - this way, boxes will be allowed to be the width of the screen but not any more

- when running into a box for a while and then jumping, you go through the floor (sort of fixed, the epsilon was too small)
  - i think this is something to do with the epsilon and movement, but i have no real idea

- if you get high enough, the collisions stop working (sort of fixed, the epsilon was too small)
  - i'm pretty sure this is because of the epsilon and the fact that floats lose precision when they scale
  - can be solved by having entities with local space and world space

- sometimes there will be a 1 pixel gap between the player and the box that it is colliding against
  - this is to do with the resolution of the window vs the 1920 x 1080 that the game is supposed to use
  - i don't think i will fix this, although if i had to then i would change the renderer resolution to NULL, NULL, as it resizes to the window, then i would change the camera code to render better

- the wall sliding is not relative to the closest box
  - probably not going to fix this issue as a solution is too long winded

TODO:

- if the player dies very early in the game and doesn't get very high i should show the tutorial pannel again

- maybe i should have a "help" button in the options menu
  - it would cycle through all of the help boards that are unlocked

- look at what is causing the menu to use so much cpu (not a huge priority)
*/

#define PRINT_PARTICLES 0

void mainUpdateAndRender(SDL_Window* window, SDL_Renderer* renderer, GameInput gameInput, float dt, GameState &gameState)
{
	MainState &mainState = gameState.mainState;
	UpgradesState &upgradesState = gameState.upgradesState;
	Assets &assets = gameState.assets;

	float &highScore = upgradesState.highScore;

	float &playerMaxJumps = upgradesState.playerMaxJumps;
	float &maxBlockChanges = upgradesState.maxBlockChanges;
	float &points = upgradesState.points;
	float &extraJumpIntensity = upgradesState.extraJumpIntensity;
	float &coinIntensity = upgradesState.coinIntensity;
	float &maxTeleports = upgradesState.maxTeleports;
	float &pickupIntensity = upgradesState.pickupIntensity;

	float &entityListSize = mainState.entityListSize;
	Entity* entityList = mainState.entityList;

	float &particleListSize = mainState.particleListSize;
	Particle* particleList = mainState.particleList;

	Entity &player = entityList[(int)entityListSize - 1];
	Entity &floor = entityList[0];

	float &lavaLevel = mainState.lavaLevel;
	float &lavaVel = mainState.lavaVel;

	float &playerJumpsLeft = mainState.playerJumpsLeft;
	float &playerJumpTimer = mainState.playerJumpTimer;
	Wall &playerJumpWall = mainState.playerJumpWall;
	Vec2 &playerDrawSize = mainState.playerDrawSize;
	Vec2 &playerEyePos = mainState.playerEyePos;
	Vec2 &playerEyeSize = mainState.playerEyeSize;
	float &playerBlinkTimer = mainState.playerBlinkTimer;
	float &playerBlinkTimerMax = mainState.playerBlinkTimerMax;

	Vec2 &playerPrevEyeLooking = mainState.playerPrevEyeLooking;

	float &blockChanges = mainState.blockChanges;
	float &teleports = mainState.teleports;

	float &amountCoins = mainState.amountCoins;
	float &amountPickups = mainState.amountPickups;
	float &pickupBackgroundSize = mainState.pickupBackgroundSize;

	float &fallingBoxTimer = mainState.fallingBoxTimer;
	float &fallingBoxTimerMax = mainState.fallingBoxTimerMax;
	Camera &camera = mainState.camera;

	float &snowParticles = mainState.snowParticles;
	float &lavaParticles = mainState.lavaParticles;

	float &playerHighest = mainState.playerHighest;
	float &currentScore = mainState.currentScore;
	float &currentTime = mainState.currentTime;

	float &squashTimer = mainState.squashTimer;
	float &deathTimer = mainState.deathTimer;

	//assets
	SDL_Texture* &playerTexture = assets.playerTexture;
	SDL_Texture* &playerEyeTexture = assets.playerEyeTexture;
	SDL_Texture* &playerBlinkTexture = assets.playerBlinkTexture;
	SDL_Texture* &coinTexture = assets.coinTexture;
	SDL_Texture* &jumpPickupTexture = assets.jumpPickupTexture;
	SDL_Texture* &changeBlockPickupTexture = assets.changeBlockPickupTexture;
	SDL_Texture* &teleportPickupTexture = assets.teleportPickupTexture;
	TTF_Font* &goodTimesFont = assets.goodTimesFont;
	Mix_Chunk* &coinPickupSound = assets.coinPickupSound;
	Mix_Chunk* &powerupPickupSound = assets.powerupPickupSound;
	Mix_Chunk* &teleportSound = assets.teleportSound;
	Mix_Chunk* &extraJumpSound = assets.extraJumpSound;
	Mix_Chunk* &changeBlockSound = assets.changeBlockSound;
	Mix_Chunk* &invalidSelectionSound = assets.invalidSelectionSound;
	Mix_Chunk* &squashSound = assets.squashSound;
	Mix_Chunk* &lavaDeathSound = assets.lavaDeathSound;

	if (gameState.menuState.tutorial)
	{
		gameState.screen = GS_BOARD;
		gameState.menuState.boardType = BT_TUTORIAL;
		return;
	}

	if (mainState.init == false)
	{
		//init player
		player.type = ET_PLAYER;
		player.pos = { 600, 800 };
		player.size = { 70, 100 };
		player.colour = { 0, 0, 0 };
		player.init = true;

		playerDrawSize = { 70, 100 };
		playerEyePos = { 20, 35 };
		playerEyeSize = { 30, 30 };
		playerBlinkTimerMax = 0.5f + ((rand() % 450) / 100.0f);

		//init floor
		floor.type = ET_FLOOR;
		floor.pos = { 0, 900 };
		floor.size = { WINDOW_WIDTH, 180 };
		floor.colour = { 40, 40, 40 };
		floor.init = true;

		//init upgrade things
		playerJumpsLeft = playerMaxJumps;
		blockChanges = maxBlockChanges;
		teleports = maxTeleports;

		//init lava
		lavaLevel = 1500.0f;
		lavaVel = 45.0f;

		//init falling box timer
		fallingBoxTimer = 0.0f;
		fallingBoxTimerMax = 1.0f;

		pickupBackgroundSize = 150.0f;

		entityListSize = 1000.0f;

		gameState.menuState.isPausingGame = true;

		//init camera
		camera.size = { WINDOW_WIDTH, WINDOW_HEIGHT };

		Mix_PlayMusic(assets.gameMusic, -1);

		gameState.mainState.init = true;
	}

	Mix_ResumeMusic();

	//update game things
	if (gameInput.backPressed || gameInput.lostFocus)
	{
		gameState.prevScreen = gameState.screen;
		gameState.screen = GS_MENU;
	}

	currentTime += dt;
	float prevPlayerHighest = playerHighest;
	playerHighest = max(playerHighest, (800.0f - player.pos.y));
	if (prevPlayerHighest != playerHighest)
	{
		currentScore += (playerHighest - prevPlayerHighest) / 100.0f; //change this
	}
	highScore = max(currentScore, highScore);

	if (player.size.y < 100.0f)
	{
		gameInput.up = false;
		gameInput.actionPressed = false;
		if (player.size.y < 80.0f)
		{
			gameInput.left = false;
			gameInput.right = false;
		}
	}

	//change block
	if (gameInput.mouseButton.leftClicked && blockChanges > 0.0f)
	{
		bool boxFound = false;
		for (int i = 0; i < entityListSize; i++)
		{
			Entity &entity = entityList[i];

			if (entity.init && entity.type == ET_STATIC_BOX && pointInsideRect(gameInput.mousePos, entity.pos - camera.pos, entity.size))
			{
				Mix_PlayChannel(-1, changeBlockSound, 0);
				blockChanges--;
				entity.type = ET_WALKTHROUGH_BOX;
				boxFound = true;

				for (int i = 0; i < 30; i++)
				{
					makeParticle(particleList, particleListSize, entity.pos + (entity.size / 2.0f), entity.size, Vec2{}, Vec2{ randInt(-500, 500), randInt(-500, 500) }, randInt(200, 400) / 100.0f, assets.dustTexture);
				}

				break;
			}
		}
		if (!boxFound)
		{
			Mix_PlayChannel(-1, invalidSelectionSound, 0);
		}
	}

	//teleport
	if (gameInput.mouseButton.rightClicked && teleports > 0.0f)
	{
		bool canTeleport = true;

		if (canTeleport)
		{
			for (int i = 0; i < entityListSize; i++)
			{
				Entity entity = entityList[i];

				if (entity.init && (entity.type == ET_BOX || entity.type == ET_STATIC_BOX || entity.type == ET_FLOOR))
				{
					if (pointInsideRect(gameInput.mousePos, entity.pos - camera.pos, entity.size))
					{
						canTeleport = false;
						break;
					}
				}
			}
		}

		if (canTeleport)
		{
			Mix_PlayChannel(-1, teleportSound, 0);

			for (int i = 0; i < 30; i++)
			{
				Vec2 centerPoint = { player.pos.x + (player.size.x / 2.0f) + randInt(-35, 35), player.pos.y + (player.size.y / 2.0f) + randInt(-50, 50) };
				makeParticle(particleList, particleListSize, centerPoint, Vec2{ 40, 40 }, Vec2{}, randInt(0, 180), randInt(0, 180), Vec2{ randInt(-10, 10), randInt(-10, 10) }, randInt(100, 200) / 100.0f, assets.sparkleTexture);
			}

			player.pos = gameInput.mousePos + camera.pos - (player.size / 2.0f);

			for (int i = 0; i < 30; i++)
			{
				Vec2 centerPoint = { player.pos.x + (player.size.x / 2.0f) + randInt(-35, 35), player.pos.y + (player.size.y / 2.0f) + randInt(-50, 50) };
				makeParticle(particleList, particleListSize, centerPoint, Vec2{ 40, 40 }, Vec2{}, randInt(0, 180), randInt(0, 180), Vec2{ randInt(-10, 10), randInt(-10, 10) }, randInt(100, 200) / 100.0f, assets.sparkleTexture);
			}

			player.vel = {};
			teleports--;
		}
		else
		{
			Mix_PlayChannel(-1, invalidSelectionSound, 0);
		}
	}

	//make falling boxes
	fallingBoxTimer += dt;
	if (fallingBoxTimer >= fallingBoxTimerMax)
	{
		Entity box = {};
		box.type = ET_BOX;

		box.size = { (float)roundToI(100.0f + (clamp((float)(rand() % 250 + rand() % 250), 100.0f, 300.0f))),
					 (float)roundToI(100.0f + (clamp((float)(rand() % 200 + rand() % 200), 100.0f, 300.0f))) };

		//find highest entity
		Entity highestEntity = {};
		highestEntity.pos = floor.pos;
		for (int i = 0; i < entityListSize; i++)
		{
			Entity entity = entityList[i];
			if (entity.init && entity.pos.y < highestEntity.pos.y && (entity.type == ET_PLAYER || entity.type == ET_STATIC_BOX))
			{
				highestEntity = entity;
			}
		}
		if (camera.pos.y < highestEntity.pos.y)
		{
			highestEntity.pos.y = camera.pos.y;
		}

		float randModifier = (((rand() % 300 + rand() % 300) * 2.0f) - 600.0f);

		box.pos = { (float)roundToI(((highestEntity.pos.x + highestEntity.size.x / 2.0f) - (box.size.x / 2.0f)) + randModifier), (float)roundToI(highestEntity.pos.y - (400.0f + (rand() % 200))) };
		moveWithinX(box.pos.x);

		//make sure box doesn't spawn inside another
		bool boxHasMoved;
		for (int j = 0; j < 4; j++)
		{
			boxHasMoved = false;
			for (int i = 0; i < entityListSize; i++)
			{
				Entity entity = entityList[i];
				if (entity.init && (entity.type == ET_BOX))
				{
					for (int j = 0; j < 100; j++)
					{
						if (collided(box, entity))
						{
							box.pos.y -= 10.0f;
							boxHasMoved = true;
						}
					}
				}
			}
			if (!boxHasMoved)
			{
				break;
			}
		}

		float colourMin = 0.0f;
		float colourMax = 80.0f;

		box.colour = { (unsigned char)clamp((float)((unsigned char)(130 + (rand() % 130 + rand() % 130))), colourMin, colourMax),
						(unsigned char)clamp((float)((unsigned char)(130 + (rand() % 130 + rand() % 130))), colourMin, colourMax),
						(unsigned char)clamp((float)((unsigned char)(130 + (rand() % 130 + rand() % 130))), colourMin, colourMax) };

		box.vel = { 0.0f, 230.0f + (rand() % 400) }; //{ (rand() % 100) - 50.0f, 100.0f + (rand() % 500) }; //moving the x may be good
		box.init = true;

		addEntityToList(box, entityList, entityListSize);

		fallingBoxTimer = 0.0f;

		fallingBoxTimerMax = 1.0f + ((rand() % 100) / 100.0f);
	}

	//make coins
	float maxCoins = 30.0f;
	while (amountCoins < maxCoins)
	{
		float highestCoinY = 0.0f;
		for (int i = 0; i < entityListSize; i++)
		{
			Entity entity = entityList[i];
			if (entity.type == ET_COIN && entity.init)
			{
				if (entity.pos.y < highestCoinY)
				{
					highestCoinY = entity.pos.y;
				}
			}
		}
		Entity coin = {};

		coin.pos = { (float)(rand() % WINDOW_WIDTH), highestCoinY + ((rand() % 600) - 300.0f) - (2000.0f - (200.0f * coinIntensity)) };
		coin.size = { 40, 40 };
		coin.type = ET_COIN;
		coin.init = true;

		addEntityToList(coin, entityList, entityListSize);
		amountCoins++;
	}

	bool pickupsUnlocked = (playerMaxJumps > 0.0f && maxBlockChanges > 0.0f && maxTeleports > 0.0f);

	//make other pickups
	if (pickupsUnlocked)
	{
		float maxPickups = 20.0f;
		while (amountPickups < maxPickups)
		{
			float highestPickupY = 800.0f;
			for (int i = 0; i < entityListSize; i++)
			{
				Entity entity = entityList[i];
				if ((entity.type == ET_CHANGE_BLOCK_PICKUP || entity.type == ET_JUMP_PICKUP || entity.type == ET_TELEPORT_PICKUP) && entity.init)
				{
					if (entity.pos.y < highestPickupY)
					{
						highestPickupY = entity.pos.y;
					}
				}
			}

			Entity pickup = {};

			pickup.pos = { (float)(rand() % WINDOW_WIDTH), highestPickupY + ((rand() % 400) - 200.0f) - (5000 - (300.0f * pickupIntensity)) };
			pickup.size = { 40, 40 };

			float randPickupNum = rand() % 3;
			if (randPickupNum == 0)
			{
				pickup.type = ET_JUMP_PICKUP;
			}
			else if (randPickupNum == 1)
			{
				pickup.type = ET_CHANGE_BLOCK_PICKUP;
			}
			if (randPickupNum == 2)
			{
				pickup.type = ET_TELEPORT_PICKUP;
			}
			pickup.init = true;

			addEntityToList(pickup, entityList, entityListSize);
			amountPickups++;
		}
	}

	//make particles
	while (snowParticles < (1400 * ((int)(gameState.menuState.snowIntensity * 10)) / 10))
	{
		Vec2 centerPoint = { randInt(0, WINDOW_WIDTH), randInt(camera.pos.y - (WINDOW_HEIGHT / 2.0f), camera.pos.y + camera.size.y + (WINDOW_HEIGHT / 2.0f)) };
		float size = randInt(2, 25);
		makeParticle(particleList, particleListSize, centerPoint, Vec2{ size, size }, Vec2{}, randInt(0, 365), randInt(0, 365), Vec2{ randInt(-80, 80), randInt(-80, 80) }, randInt(1000, 5000) / 1000.0f, assets.snowflakeTexture);
		snowParticles++;
	}

#if PRINT_PARTICLES //print particles
	int particles = 0;
	for (int i = 0; i < particleListSize; i++)
	{
		if (particleList[i].init)
		{
			particles++;
		}
		else
		{
			break;
		}
	}
	std::cout << "PARTICLES: " << particles << std::endl;
#endif

	//set accel for the player based on input
	float playerAccelChange = 7000.0f;
	float playerJumpVel = 3200.0f;
	float playerWallJumpVel = 1000.0f;
	float playerWallSlideVel = 150.0f;

	bool playerOnGround = false;
	bool playerOnLeftWall = false;
	bool playerOnRightWall = false;
	bool boxOnPlayer = false;
	bool playerFellHard = false;

	if (gameInput.left)
	{
		player.accel.x = -playerAccelChange;
	}
	if (gameInput.right)
	{
		player.accel.x = playerAccelChange;
	}
	if (gameInput.left == gameInput.right)
	{
		player.accel.x = 0.0f;
	}
	player.accel.y = 5000.0f;

	//do collision and movement for player and all boxes
	for (int j = 0; j < entityListSize; j++)
	{
		Entity &entity = entityList[j];
		if (entity.init && (entity.type == ET_PLAYER || entity.type == ET_BOX))
		{
			Entity closestOther;
			Vec2 entityMovDiff = (entity.vel * dt) + (entity.accel * (dt * dt));
			float collisionTime = 1.0f;
			Vec2 entityMidPos = { entity.pos.x + (entity.size.x / 2.0f), entity.pos.y + (entity.size.y / 2.0f) }; //remove this? (used in loop below making this redundant)
			Vec2 wallNormal = { 0, 0 };

			for (int collisionNum = 0; collisionNum < 4; collisionNum++)
			{
				for (int i = 0; i < entityListSize; i++)
				{
					Entity other = entityList[i];
					if ((other.init) && ((other.type == ET_BOX || other.type == ET_STATIC_BOX || other.type == ET_PLAYER) ||
						(entity.type == ET_BOX && other.type == ET_WALKTHROUGH_BOX)) && (j != i))
					{

						entityMidPos = { entity.pos.x + (entity.size.x / 2.0f), entity.pos.y + (entity.size.y / 2.0f) };

						other.pos -= (entity.size / 2.0f);
						other.size += (entity.size);

						//this is to try to account for sceen scrolling, it doesn't work for objects more than half of the screen's width (thoughts in the bugs section)
						float screenBound = entityMidPos.x + (WINDOW_WIDTH / 2.0f);

						if (screenBound > WINDOW_WIDTH)
						{
							screenBound -= WINDOW_WIDTH;
						}

						if (other.pos.x < screenBound)
						{
							other.pos.x += (WINDOW_WIDTH);
						}
						if (entityMidPos.x < screenBound)
						{
							entityMidPos.x += (WINDOW_WIDTH);
						}

						other.pos.x -= screenBound;
						entityMidPos.x -= screenBound;
						//--------------------------------------------------------

						if (testWall(collisionTime, other.pos.x, other.pos.y, other.size.y, entityMidPos.x, entityMidPos.y, entityMovDiff.x, entityMovDiff.y))
						{
							closestOther = other;
							wallNormal = Vec2{ -1, 0 };
						}
						if (testWall(collisionTime, other.pos.x + other.size.x, other.pos.y, other.size.y, entityMidPos.x, entityMidPos.y, entityMovDiff.x, entityMovDiff.y))
						{
							closestOther = other;
							wallNormal = Vec2{ 1, 0 };
						}
						if (testWall(collisionTime, other.pos.y, other.pos.x, other.size.x, entityMidPos.y, entityMidPos.x, entityMovDiff.y, entityMovDiff.x))
						{
							closestOther = other;
							wallNormal = Vec2{ 0, -1 };
						}
						if (testWall(collisionTime, other.pos.y + other.size.y, other.pos.x, other.size.x, entityMidPos.y, entityMidPos.x, entityMovDiff.y, entityMovDiff.x))
						{
							closestOther = other;
							wallNormal = Vec2{ 0, 1 };
						}
					}
				}

				//this is to test collision with the floor (has to be done seperately becuse of the way i have done collisions)
				if (testWall(collisionTime, floor.pos.y, floor.pos.x, floor.size.x, entity.pos.y + entity.size.y, WINDOW_WIDTH / 2.0f, entityMovDiff.y, entityMovDiff.x))
				{
					closestOther = floor;
					wallNormal = Vec2{ 0, -1 };
				}

				if (collisionTime < 1.0f)
				{
					if (entity.type == ET_PLAYER)
					{
						if (wallNormal.x == -1 && wallNormal.y == 0)
						{
							if (entity.vel.y > 0)
							{
								playerOnRightWall = true;
								player.accel.y = max(player.accel.y - (8.0f * player.vel.y), 0.0f); //needs ODE
							}
						}
						else if (wallNormal.x == 1 && wallNormal.y == 0)
						{
							if (entity.vel.y > 0)
							{
								playerOnLeftWall = true;
								player.accel.y = max(player.accel.y - (8.0f * player.vel.y), 0.0f); //needs ODE
							}
						}

						if (wallNormal.x == 0 && wallNormal.y == -1)
						{
							if (player.vel.y > 3000.0f)
							{
								playerFellHard = true;
							}
							playerOnGround = true;
							entity.vel.y = closestOther.vel.y;
						}
						else
						{
							entity.vel = (entity.vel * (1.0f - absolute(wallNormal)));
						}
					}

					if (wallNormal.x == 0 && wallNormal.y == -1 && entity.type == ET_BOX && (closestOther.type == ET_STATIC_BOX || closestOther.type == ET_FLOOR || closestOther.type == ET_WALKTHROUGH_BOX))
					{
						entity.vel = {};
						entity.accel = {};
						entity.type = ET_STATIC_BOX;
					}
					if (wallNormal.x == 0 && wallNormal.y == -1 && entity.type == ET_BOX && (closestOther.type == ET_PLAYER) && playerOnGround)
					{
						collisionTime = 1.0f;
					}

					if (closestOther.type == ET_PLAYER && wallNormal.x == 0 && wallNormal.y == -1)
					{
						boxOnPlayer = true; //may not need this now, as the squash is calculated another way
					}
				}

				entity.pos = entity.pos + (collisionTime * entityMovDiff);
				moveWithinX(entity.pos.x);
				//entityMidPos = { entity.pos.x + (entity.size.x / 2.0f), entity.pos.y + (entity.size.y / 2.0f) }; //dont know if this line is needed either (see above)

				if (collisionTime == 1.0f) { break; }

				float collisionTimeRemaining = 1.0f - collisionTime;

				Vec2 amountOver = entityMovDiff * collisionTimeRemaining;
				entityMovDiff = amountOver + (wallNormal * absdot(amountOver, wallNormal));
				collisionTime = 1.0f;
			}

			if (entity.type == ET_PLAYER)
			{
				entity.accel.x -= 8.0f * entity.vel.x; //casey says there is a better way to do this (ODE)
			}
			else if ((entity.pos.y > lavaLevel) && (entity.type == ET_BOX))
			{
				if (entity.type == ET_COIN)
				{
					amountCoins--;
				}
				else if (entity.type == ET_JUMP_PICKUP || entity.type == ET_CHANGE_BLOCK_PICKUP || entity.type == ET_TELEPORT_PICKUP)
				{
					amountPickups--;
				}
				entity = {};
			}
			entity.vel += (entity.accel * 2.0f) * dt;
		}
	}

	//update particles
	for (int i = 0; i < particleListSize; i++)
	{
		Particle &particle = particleList[i];
		if (particle.init)
		{
			particle.pos += particle.vel * dt;
			particle.lifeTime += dt;
			if (particle.lifeTime >= particle.maxLifeTime)
			{
				if (particle.texture == assets.snowflakeTexture)
				{
					snowParticles--;
				}
				else if (particle.texture == assets.lavaSpewTexture)
				{
					lavaParticles--;
				}
				particle = {};
			}
		}
	}

	//some of the gameplay features that involve the player

	//making dust if the player fell hard enough
	if (playerFellHard)
	{
		for (int i = 0; i < 10; i++)
		{
			float spread = 30.0f;
			float size = 50.0f + (rand() % 100);
			makeParticle(particleList, particleListSize, Vec2{ (player.pos.x + (player.size.x / 2.0f)) + ((rand() % (int)(spread * 2)) - spread), (player.pos.y + player.size.y) + ((rand() % (int)(spread * 2)) - spread) },
				Vec2{ size, size }, Vec2{ 0.0f, 0.0f }, Vec2{ (rand() % 200) - 100.0f, (rand() % 100) - 100.0f }, 2.0f + (((rand() % 100) / 100.0f) - 0.5f), assets.dustTexture);
		}
	}

	//making sure the player doesn't get stuck inside a block
	Vec2 playerDir = { 0.0f, -1.0f };
	float playerStuckTimes = 0.0f;
	while (!(playerDir.x == 0 && playerDir.y == 0))
	{
		playerDir = {};
		for (int i = 0; i < entityListSize; i++)
		{
			Entity other = entityList[i];
			if (other.init && (other.type == ET_BOX || other.type == ET_STATIC_BOX) && collided(player, other))
			{
				Vec2 otherMidPos = other.pos + (other.size / 2.0f);
				Vec2 playerMidPos = player.pos + (player.size / 2.0f);
				playerDir = unitLen(playerDir + unitLen(playerMidPos - otherMidPos));
			}
			if (other.init && other.type == ET_FLOOR && collided(player, other))
			{
				playerDir = { 0.0f, -1.0f };
			}
		}

		player.pos += (playerDir * 5.0f);
		moveWithinX(player.pos.x);
		playerStuckTimes++;
		if (playerStuckTimes > 500.0f)
		{
			player.pos.y -= 100.0f;
			break;
		}
	}

	//box stuck prevention
	for (int i = 0; i < entityListSize; i++)
	{
		Entity &entity = entityList[i];
		if (entity.init && (entity.type == ET_BOX))
		{
			for (int j = 0; j < entityListSize; j++)
			{
				Entity other = entityList[j];
				if (other.init && (other.type == ET_STATIC_BOX || other.type == ET_BOX))
				{
					float collidedTimes = 0.0f;
					while (collided(entity, other) && (j != i))
					{
						entity.pos.y -= 10.0f;
						entity.pos.x += 1.0f;
						collidedTimes++;
						if (collidedTimes > 100.0f)
						{
							entity.pos.x += 50.0f;
							moveWithinX(entity.pos.x);
							break;
						}
					}
				}
			}
		}
	}

	//player squashing
	float squashValue = 200.0f;

	if (playerOnGround && boxOnPlayer)
	{
		player.size.y -= squashValue * dt;
		playerDrawSize.y -= squashValue * dt;
		player.pos.y += squashValue * dt;
	}
	else
	{
		if (player.size.y < 100.0f && player.init)
		{
			player.size.y += squashValue * dt;
			playerDrawSize.y += squashValue * dt;
			player.pos.y -= squashValue * dt;
		}
	}

	//death
	if (deathTimer > 0.0f)
	{
		deathTimer += dt;
	}

	if (player.size.y <= 50.0f && player.init)
	{

		for (int i = 0; i < 50; i++)
		{
			Vec2 centerPoint = { player.pos.x + (player.size.x / 2.0f) + randInt(-30, 30), player.pos.y + (player.size.y / 2.0f) + randInt(-30, 30) };
			makeParticle(particleList, particleListSize, centerPoint, Vec2{ 40, 40 }, Vec2{}, randInt(0, 180), randInt(0, 180), Vec2{ randInt(-100, 100), randInt(-100, 100) }, randInt(30, 100) / 100.0f, assets.bloodTexture);
		}

		player.init = false;
		player.size = {};
		playerDrawSize = {};

		Mix_PlayChannel(-1, squashSound, 0);
		deathTimer += dt;
	}
	else if (player.pos.y + (player.size.y * 0.9f) > lavaLevel && player.init)
	{
		Mix_PlayChannel(-1, lavaDeathSound, 0);
		deathTimer += dt;

		for (int i = 0; i < 50; i++)
		{
			Vec2 centerPoint = { player.pos.x + (player.size.x / 2.0f) + randInt(-30, 30), player.pos.y + (player.size.y / 2.0f) + randInt(-30, 30) };
			makeParticle(particleList, particleListSize, centerPoint, Vec2{ 200, 200 }, Vec2{}, randInt(0, 180), randInt(0, 180), Vec2{ randInt(-200, 200), randInt(-200, 200) }, randInt(200, 300) / 100.0f, assets.lavaSpewTexture);
		}
		player.init = false;
	}

	if (deathTimer > 2.0f)
	{
		gameState.screen = GS_UPGRADES;
		points += (float)((int)currentScore);
		return;
	}

	//coin pickup
	for (int i = 0; i < entityListSize; i++)
	{
		Entity &entity = entityList[i];
		if (entity.type == ET_COIN && entity.init)
		{
			if (collided(player, entity))
			{
				for (int i = 0; i < 10; i++)
				{
					Vec2 centerPoint = { entity.pos.x + randInt(-30, 30), entity.pos.y + randInt(-30, 30) };
					makeParticle(particleList, particleListSize, centerPoint, Vec2{ 40, 40 }, Vec2{}, randInt(0, 180), randInt(0, 180), Vec2{ randInt(-10, 10), randInt(-10, 10) }, randInt(30, 100) / 100.0f, assets.sparkleTexture);
				}
				currentScore += 20.0f;
				Mix_PlayChannel(-1, coinPickupSound, 0);
				entity = {};
				amountCoins--;
			}
		}
	}

	//getting pickups
	for (int i = 0; i < entityListSize; i++)
	{
		Entity &entity = entityList[i];
		if (entity.type == ET_JUMP_PICKUP && entity.init)
		{
			if (collided(player, entity))
			{
				for (int i = 0; i < 10; i++)
				{
					Vec2 centerPoint = { entity.pos.x + randInt(-30, 30), entity.pos.y + randInt(-30, 30) };
					makeParticle(particleList, particleListSize, centerPoint, Vec2{ 40, 40 }, Vec2{}, randInt(0, 180), randInt(0, 180), Vec2{ randInt(-10, 10), randInt(-10, 10) }, randInt(30, 100) / 100.0f, assets.blueSwirlTexture);
				}
				playerJumpsLeft += 1.0f;
				mainState.extraJumpTextEffectTime += dt;
				Mix_PlayChannel(-1, powerupPickupSound, 0);
				entity = {};
				amountPickups--;
			}
		}
		else if (entity.type == ET_CHANGE_BLOCK_PICKUP && entity.init)
		{
			if (collided(player, entity))
			{
				for (int i = 0; i < 10; i++)
				{
					Vec2 centerPoint = { entity.pos.x + randInt(-30, 30), entity.pos.y + randInt(-30, 30) };
					makeParticle(particleList, particleListSize, centerPoint, Vec2{ 40, 40 }, Vec2{}, randInt(0, 180), randInt(0, 180), Vec2{ randInt(-10, 10), randInt(-10, 10) }, randInt(30, 100) / 100.0f, assets.blueSwirlTexture);
				}
				blockChanges += 1.0f;
				mainState.changeBlockTextEffectTime += dt;
				Mix_PlayChannel(-1, powerupPickupSound, 0);
				entity = {};
				amountPickups--;
			}
		}
		else if (entity.type == ET_TELEPORT_PICKUP && entity.init)
		{
			if (collided(player, entity))
			{
				for (int i = 0; i < 10; i++)
				{
					Vec2 centerPoint = { entity.pos.x + randInt(-30, 30), entity.pos.y + randInt(-30, 30) };
					makeParticle(particleList, particleListSize, centerPoint, Vec2{ 40, 40 }, Vec2{}, randInt(0, 180), randInt(0, 180), Vec2{ randInt(-10, 10), randInt(-10, 10) }, randInt(30, 100) / 100.0f, assets.blueSwirlTexture);
				}
				teleports += 1.0f;
				mainState.teleportTextEffectTime += dt;
				Mix_PlayChannel(-1, powerupPickupSound, 0);
				entity = {};
				amountPickups--;
			}
		}
	}

	//calculate jump
	float playerTimerMax = 0.15f;
	if (playerOnGround)
	{
		playerJumpTimer = 0.0f;
		playerJumpWall = W_FLOOR;
	}
	else if (playerOnLeftWall)
	{
		playerJumpTimer = 0.0f;
		playerJumpWall = W_LEFT;
	}
	else if (playerOnRightWall)
	{
		playerJumpTimer = 0.0f;
		playerJumpWall = W_RIGHT;
	}

	if (gameInput.actionPressed)
	{
		if (playerJumpTimer < playerTimerMax)
		{
			if (playerJumpWall == W_FLOOR)
			{
				player.vel.y = -playerJumpVel;
				playerJumpTimer = playerTimerMax;

			}
			else if (playerJumpWall == W_LEFT)
			{
				player.vel.y = -playerJumpVel;
				player.vel.x = playerWallJumpVel;
				playerJumpTimer = playerTimerMax;
			}
			else if (playerJumpWall == W_RIGHT)
			{
				player.vel.y = -playerJumpVel;
				player.vel.x = -playerWallJumpVel;
				playerJumpTimer = playerTimerMax;
			}
			else
			{
				player.vel.y = -playerJumpVel;
				playerJumpTimer = playerTimerMax;
			}
		}
	}

	playerJumpTimer += dt;

	if (gameInput.upPressed && playerJumpsLeft > 0.0f)
	{
		for (int i = 0; i < 10; i++)
		{
			Vec2 centerPoint = { player.pos.x + randInt(-50, 50), player.pos.y + player.size.y + randInt(-10, 10) };
			float size = randInt(60, 120);
			makeParticle(particleList, particleListSize, centerPoint, Vec2{size, size}, Vec2{ size + 20, size + 20 }, randInt(0, 180), randInt(360, 720), Vec2{ randInt(-200, 200), randInt(600, 650) }, randInt(30, 100) / 100.0f, assets.spiralCloudTexture);
		}

		Mix_PlayChannel(-1, extraJumpSound, 0);
		player.vel.y = -(playerJumpVel * (1.0f + (extraJumpIntensity / 10.0f)));
		playerJumpsLeft -= 1.0f;
	}

	//update lava
	if (currentTime < 240.0f)
	{
		lavaVel += (0.4 * dt);
	}
	while (lavaParticles < 250)
	{
		for (int i = 0; i < 4; i++)
		{
			Vec2 centerPoint = { rand() % WINDOW_WIDTH, lavaLevel };
			float angle = randInt(0, 360);

			makeParticle(particleList, particleListSize, centerPoint, Vec2{ 100, 100 }, Vec2{ 0, 0 }, angle, angle + randInt(-50, 50), Vec2{ 0.0f, -(lavaVel * (randInt(300, 400) / 100.0f)) }, randInt(30, 100) / 100.0f, assets.lavaSpewTexture);
			lavaParticles++;
		}
	}
	lavaLevel -= lavaVel * dt;


	//update camera
	camera.pos.y = player.pos.y + (0.5 * player.size.y) - (0.5 * camera.size.y);

	if (camera.pos.y + camera.size.y > WINDOW_HEIGHT)
	{
		camera.pos.y = WINDOW_HEIGHT - camera.size.y;
	}

	//update pickup background
	pickupBackgroundSize = 130.0f + (sin(currentTime * 4.0f) * 20.0f);

	//render
	drawRect(renderer, 0, 0, 1920, 1080, 100, 100, 100);

	for (int i = 0; i < entityListSize; i++)
	{
		Entity entity = entityList[i];
		if (entity.init)
		{
			if (entity.type == ET_WALKTHROUGH_BOX)
			{
				if ((entity.pos.x + entity.size.x > WINDOW_WIDTH) && (entity.pos.x <= WINDOW_WIDTH))
				{
					drawOutlineRect(renderer, 0 - camera.pos.x, roundToI(entity.pos.y - camera.pos.y), roundToI(entity.size.x - (WINDOW_WIDTH - entity.pos.x)), roundToI(entity.size.y), entity.colour.r, entity.colour.g, entity.colour.b);
					drawOutlineRect(renderer, roundToI(entity.pos.x - camera.pos.x), roundToI(entity.pos.y - camera.pos.y), roundToI(WINDOW_WIDTH - entity.pos.x) + 1, roundToI(entity.size.y), entity.colour.r, entity.colour.g, entity.colour.b);
				}
				else
				{
					drawOutlineRect(renderer, roundToI(entity.pos.x - camera.pos.x), roundToI(entity.pos.y - camera.pos.y), roundToI(entity.size.x), roundToI(entity.size.y), entity.colour.r, entity.colour.g, entity.colour.b);
				}
			}
			else if (entity.type == ET_COIN)
			{
				renderEntity(renderer, entity, coinTexture, assets.coinBackgroundTexture, pickupBackgroundSize, camera);
			}
			else if (entity.type == ET_JUMP_PICKUP)
			{
				renderEntity(renderer, entity, jumpPickupTexture, assets.pickupBackgroundTexture, pickupBackgroundSize, camera);
			}
			else if (entity.type == ET_CHANGE_BLOCK_PICKUP)
			{
				renderEntity(renderer, entity, changeBlockPickupTexture, assets.pickupBackgroundTexture, pickupBackgroundSize, camera);
			}
			else if (entity.type == ET_TELEPORT_PICKUP)
			{
				renderEntity(renderer, entity, teleportPickupTexture, assets.pickupBackgroundTexture, pickupBackgroundSize, camera);
			}
			else if (entity.type != ET_PLAYER)
			{
				if ((entity.pos.x + entity.size.x > WINDOW_WIDTH) && (entity.pos.x <= WINDOW_WIDTH))
				{
					drawRect(renderer, 0 - camera.pos.x, roundToI(entity.pos.y - camera.pos.y), roundToI(entity.size.x - (WINDOW_WIDTH - entity.pos.x)), roundToI(entity.size.y), entity.colour.r, entity.colour.g, entity.colour.b);
					drawRect(renderer, roundToI(entity.pos.x - camera.pos.x), roundToI(entity.pos.y - camera.pos.y), roundToI(WINDOW_WIDTH - entity.pos.x) + 1, roundToI(entity.size.y), entity.colour.r, entity.colour.g, entity.colour.b);
				}
				else
				{
					drawRect(renderer, roundToI(entity.pos.x - camera.pos.x), roundToI(entity.pos.y - camera.pos.y), roundToI(entity.size.x), roundToI(entity.size.y), entity.colour.r, entity.colour.g, entity.colour.b);
				}
			}
		}
	}

	//draw player
	SDL_Rect playerDrawRect;
	SDL_Rect playerEyeDrawRect;

	Vec2 eyeLookingTo = {}; //make the player look at the nearest moving box, that he is not touching, on the screen (and possibly only above him)

	Entity closestBox = {};
	for (int i = 0; i < entityListSize; i++)
	{
		Entity entity = entityList[i];
		if (entity.type == ET_BOX && (entity.pos.y + entity.size.y) < (player.pos.y + player.size.y) && entityOnScreen(camera, entity) && entity.init)
		{
			if (!closestBox.init)
			{
				closestBox = entity;
			}
			else
			{
				Vec2 boxCentre = entity.pos + (entity.size / 2.0f);
				Vec2 otherBoxCentre = closestBox.pos + (closestBox.size / 2.0f);
				Vec2 playerCentre = player.pos + (player.size / 2.0f);

				float boxDistance = mag(boxCentre - playerCentre);
				float otherDistance = mag(otherBoxCentre - playerCentre);

				if (boxDistance < otherDistance)
				{
					closestBox = entity;
				}
			}
		}
	}

	float maxEyeRadius = 12.0f;

	if (closestBox.init)
	{
		Vec2 closestBoxCentre = closestBox.pos + (closestBox.size / 2.0f);
		Vec2 playerCentre = player.pos + (player.size / 2.0f);

		eyeLookingTo = (unitLen(closestBoxCentre - playerCentre) * maxEyeRadius);
	}
	else
	{

		if (absolute(player.vel.x / 100.0f) < maxEyeRadius)
		{
			eyeLookingTo.x = player.vel.x / 100.0f;
		}
		else
		{
			eyeLookingTo.x = unitLen(player.vel).x * maxEyeRadius;
		}
		if (absolute(player.vel.y / 100.0f) < maxEyeRadius)
		{
			eyeLookingTo.y = player.vel.y / 100.0f;
		}
		else
		{
			eyeLookingTo.y = unitLen(player.vel).y * maxEyeRadius;
		}
	}

	Vec2 eyeLooking = playerPrevEyeLooking + ((eyeLookingTo - playerPrevEyeLooking) * 0.5f);

	playerPrevEyeLooking = eyeLooking;

	if (!player.init)
	{
		eyeLooking = {};
	}

	float playerBlinkDuration = 0.06f;
	if (playerBlinkTimer >= playerBlinkTimerMax)
	{
		playerBlinkTimer = 0.0f;
		playerBlinkTimerMax = 0.5f + ((rand() % 650) / 100.0f);
	}
	playerBlinkTimer += dt;

	if ((player.pos.x + playerDrawSize.x > WINDOW_WIDTH) && (player.pos.x <= WINDOW_WIDTH))
	{
		if (!(playerBlinkTimer <= playerBlinkDuration || player.size.y < 70.0f))
		{
			playerDrawRect = { roundToI(player.pos.x - camera.pos.x), roundToI(player.pos.y - camera.pos.y), roundToI(playerDrawSize.x), roundToI(playerDrawSize.y) };
			SDL_RenderCopy(renderer, playerTexture, NULL, &playerDrawRect);

			playerEyeDrawRect = { roundToI(playerDrawRect.x + playerEyePos.x + eyeLooking.x), roundToI(playerDrawRect.y + playerEyePos.y + eyeLooking.y), roundToI(playerEyeSize.x), roundToI(playerEyeSize.y) };
			SDL_RenderCopy(renderer, playerEyeTexture, NULL, &playerEyeDrawRect);
		}
		else
		{
			playerDrawRect = { roundToI(player.pos.x - camera.pos.x), roundToI(player.pos.y - camera.pos.y), roundToI(playerDrawSize.x), roundToI(playerDrawSize.y) };
			SDL_RenderCopy(renderer, playerBlinkTexture, NULL, &playerDrawRect);
		}

		if (!(playerBlinkTimer <= playerBlinkDuration || player.size.y < 70.0f))
		{
			playerDrawRect = { roundToI(player.pos.x - camera.pos.x - WINDOW_WIDTH), roundToI(player.pos.y - camera.pos.y), roundToI(playerDrawSize.x), roundToI(playerDrawSize.y) };
			SDL_RenderCopy(renderer, playerTexture, NULL, &playerDrawRect);

			playerEyeDrawRect = { roundToI(playerDrawRect.x + playerEyePos.x + eyeLooking.x), roundToI(playerDrawRect.y + playerEyePos.y + eyeLooking.y), roundToI(playerEyeSize.x), roundToI(playerEyeSize.y) };
			SDL_RenderCopy(renderer, playerEyeTexture, NULL, &playerEyeDrawRect);
		}
		else
		{
			playerDrawRect = { roundToI(player.pos.x - camera.pos.x - WINDOW_WIDTH), roundToI(player.pos.y - camera.pos.y), roundToI(playerDrawSize.x), roundToI(playerDrawSize.y) };
			SDL_RenderCopy(renderer, playerBlinkTexture, NULL, &playerDrawRect);
		}
	}
	else
	{
		if (!(playerBlinkTimer <= playerBlinkDuration || player.size.y < 70.0f))
		{
			playerDrawRect = { roundToI(player.pos.x - camera.pos.x), roundToI(player.pos.y - camera.pos.y), roundToI(playerDrawSize.x), roundToI(playerDrawSize.y) };
			SDL_RenderCopy(renderer, playerTexture, NULL, &playerDrawRect);

			playerEyeDrawRect = { roundToI(playerDrawRect.x + playerEyePos.x + eyeLooking.x), roundToI(playerDrawRect.y + playerEyePos.y + eyeLooking.y), roundToI(playerEyeSize.x), roundToI(playerEyeSize.y) };
			SDL_RenderCopy(renderer, playerEyeTexture, NULL, &playerEyeDrawRect);
		}
		else
		{
			playerDrawRect = { roundToI(player.pos.x - camera.pos.x), roundToI(player.pos.y - camera.pos.y), roundToI(playerDrawSize.x), roundToI(playerDrawSize.y) };
			SDL_RenderCopy(renderer, playerBlinkTexture, NULL, &playerDrawRect);
		}
	}

	//drawRect(window, renderer, 0, lavaLevel - camera.pos.y, WINDOW_WIDTH, WINDOW_HEIGHT * 2.0f, 170 + (sin(lavaLevel / 100.0f) * 20), 70 + (sin(lavaLevel / 100.0f) * 10), 10 + (sin(lavaLevel / 100.0f) * 10));
	drawRect(renderer, 0, lavaLevel - camera.pos.y, WINDOW_WIDTH, WINDOW_HEIGHT * 2.0f, 150, 60, 0);

	//render particles 
#if 0
	for (int i = 0; i < particleListSize; i++)
	{
		Particle &particle = particleList[i];
		if (particle.init)
		{
			float particleTime = particle.lifeTime / particle.maxLifeTime;
			//Vec2 particlePos = lerp(particle.startPos, particle.endPos, particleTime);
			Vec2 particleSize = lerp(particle.startSize, particle.endSize, particleTime);
			float particleAngle = lerp(particle.startAngle, particle.endAngle, particleTime);

			SDL_Rect particleRect = { roundToI(particle.pos.x - camera.pos.x), roundToI(particle.pos.y - camera.pos.y), roundToI(particleSize.x), roundToI(particleSize.y) };
			SDL_Point particleAngleCentre = { roundToI((particleSize.x / 2.0f)), roundToI((particleSize.y / 2.0f)) };

			SDL_RenderCopyEx(renderer, particle.texture, NULL, &particleRect, particleAngle, &particleAngleCentre, SDL_RendererFlip{ SDL_FLIP_NONE });
		}
	}
#endif
	renderParticles(particleList, particleListSize, camera, renderer, dt);

	//gui things

	std::string highScoreString = "High Score: " + std::to_string((int)highScore);
	renderText(renderer, highScoreString, goodTimesFont, Vec2{ 10, 5 }, Colour{ 0, 0, 0 });

	std::string currentScoreString = "Score: " + std::to_string((int)currentScore);
	renderText(renderer, currentScoreString, goodTimesFont, Vec2{ 10, 65 }, Colour{ 0, 0, 0 });

#if 0
	std::string heightString = "Height: " + std::to_string((int)((800.0f - player.pos.y) / 100.0f));
	renderText(renderer, heightString, goodTimesFont, Vec2{ 10, 125 }, Colour{ 0, 0, 0 });
#endif

	if (playerMaxJumps > 0.0f)
	{
		std::string jumpsLeftString = "Jumps: " + std::to_string((int)playerJumpsLeft);
		if (mainState.extraJumpTextEffectTime > 0.0f)
		{
			Vec2 textSize = getTextSize(jumpsLeftString, goodTimesFont);
			Vec2 originalTextSize = getTextSize(jumpsLeftString, goodTimesFont);
			textSize.x *= (sinf(mainState.extraJumpTextEffectTime * 7) + 2) * 0.5f;
			textSize.y *= (sinf(mainState.extraJumpTextEffectTime * 7) + 2) * 0.5f;
			renderText(renderer, jumpsLeftString, goodTimesFont, Vec2{ 10 - ((textSize.x - originalTextSize.x) / 2.0f), (WINDOW_HEIGHT - 60) - ((textSize.y - originalTextSize.y) / 2.0f) }, textSize, Colour{ 0, 0, 0 });
			mainState.extraJumpTextEffectTime += dt;
			if (mainState.extraJumpTextEffectTime > (PI / 7.0f))
			{
				mainState.extraJumpTextEffectTime = 0.0f;
			}
		}
		else
		{
			renderText(renderer, jumpsLeftString, goodTimesFont, Vec2{ 10, WINDOW_HEIGHT - 60 }, Colour{ 0, 0, 0 });
		}
	}
	if (maxBlockChanges > 0.0f)
	{
		std::string blockChangesString = "block Changes: " + std::to_string((int)blockChanges);
		if (mainState.changeBlockTextEffectTime > 0.0f)
		{
			Vec2 textSize = getTextSize(blockChangesString, goodTimesFont);
			Vec2 originalTextSize = getTextSize(blockChangesString, goodTimesFont);
			textSize.x *= (sinf(mainState.changeBlockTextEffectTime * 7) + 2) * 0.5f;
			textSize.y *= (sinf(mainState.changeBlockTextEffectTime * 7) + 2) * 0.5f;
			renderText(renderer, blockChangesString, goodTimesFont, Vec2{ 400 - ((textSize.x - originalTextSize.x) / 2.0f), (WINDOW_HEIGHT - 60) - ((textSize.y - originalTextSize.y) / 2.0f) }, textSize, Colour{ 0, 0, 0 });
			mainState.changeBlockTextEffectTime += dt;
			if (mainState.changeBlockTextEffectTime > (PI / 7.0f))
			{
				mainState.changeBlockTextEffectTime = 0.0f;
			}
		}
		else
		{
			renderText(renderer, blockChangesString, goodTimesFont, Vec2{ 400, WINDOW_HEIGHT - 60 }, Colour{ 0, 0, 0 });
		}
	}
	if (maxTeleports > 0.0f)
	{
		std::string teleportsString = "teleports: " + std::to_string((int)teleports);
		if (mainState.teleportTextEffectTime > 0.0f)
		{
			Vec2 textSize = getTextSize(teleportsString, goodTimesFont);
			Vec2 originalTextSize = getTextSize(teleportsString, goodTimesFont);
			textSize.x *= (sinf(mainState.teleportTextEffectTime * 7) + 2) * 0.5f;
			textSize.y *= (sinf(mainState.teleportTextEffectTime * 7) + 2) * 0.5f;
			renderText(renderer, teleportsString, goodTimesFont, Vec2{ 1100 - ((textSize.x - originalTextSize.x) / 2.0f), (WINDOW_HEIGHT - 60) - ((textSize.y - originalTextSize.y) / 2.0f) }, textSize, Colour{ 0, 0, 0 });
			mainState.teleportTextEffectTime += dt;
			if (mainState.teleportTextEffectTime > (PI / 7.0f))
			{
				mainState.teleportTextEffectTime = 0.0f;
			}
		}
		else
		{
			renderText(renderer, teleportsString, goodTimesFont, Vec2{ 1100, WINDOW_HEIGHT - 60 }, Colour{ 0, 0, 0 });
		}
	}

	SDL_RenderPresent(renderer);
}

//sometimes when jumping the player falls into the box, this is becuase of float precision at high values of y
//maybe should change this function so it converts to relative pos to solve the screen wrapping issue, i have done it outside the function but may need always when using the funciton
bool testWall(float &collisionTime, float boxX, float boxY, float boxSizeY, float playerX, float playerY, float playerMovDiffX, float playerMovDiffY)
{
	bool collided = false;
	float newCollisionTime = (boxX - playerX) / playerMovDiffX;
	float newPlayerPosOnWallX = playerX + (newCollisionTime * playerMovDiffX);
	float newPlayerPosOnWallY = playerY + (newCollisionTime * playerMovDiffY);

	if (collisionTime > newCollisionTime && (newCollisionTime >= 0)
		&& (newPlayerPosOnWallY > boxY && newPlayerPosOnWallY < (boxY + boxSizeY)))
	{
		collided = true;
		collisionTime = max((newCollisionTime - 0.01f), 0); //see below
	}
	
	return collided;
}
//the epsion is there because, when the collision time is perfect, the player almost (due to float precision) always hits the wall perfectly
//and therefore is "inside" the wall and cannot get out, the epsilon should be swapped by a calculation that checks if it is over the wall and
//puts it on the wall, and then if it is on the wall, it can only go away from the wall


Entity &getEmptyValue(Entity *entityList, float entityListSize)
{
	for (int i = 0; i < entityListSize; i++)
	{
		if (entityList[i].init == false)
		{
			return entityList[i];
		}
	}
}

void addEntityToList(Entity entity, Entity *entityList, float entityListSize)
{
	for (int i = 0; i < entityListSize; i++)
	{
		if (entityList[i].init == false)
		{
			entityList[i] = entity;
			return;
		}
	}
}

void moveWithinX(float &posX) //don't know if i should do everything this way, seems like there should be a better way to do this as it is bug prone
{
	if (posX > WINDOW_WIDTH)
	{
		posX -= WINDOW_WIDTH;
	}
	else if (posX < 0)
	{
		posX += WINDOW_WIDTH;
	}
}

void renderText(SDL_Renderer* renderer, std::string string, TTF_Font* font, Vec2 pos, Vec2 size, Colour colour)
{
	SDL_Color textColour = { colour.r, colour.g, colour.b, 255 };
	SDL_Surface* img = TTF_RenderText_Blended(font, string.c_str(), textColour);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, img);
	SDL_FreeSurface(img);

	SDL_Rect rect = { roundToI(pos.x), roundToI(pos.y), roundToI(size.x), roundToI(size.y) };
	if (size.x == 0.0f && size.y == 0.0f)
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_DestroyTexture(texture);
}

void renderTextWrapped(SDL_Renderer* renderer, std::string string, TTF_Font* font, Vec2 pos, Vec2 sizeMod, Colour colour, int wrapLength)
{
	SDL_Color textColour = { colour.r, colour.g, colour.b, 255 };
	SDL_Surface* img = TTF_RenderText_Blended_Wrapped(font, string.c_str(), textColour, wrapLength / sizeMod.x);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, img);
	SDL_FreeSurface(img);

	SDL_Rect rect = { roundToI(pos.x), roundToI(pos.y), 0, 0 };
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	rect.w *= sizeMod.x;
	rect.h *= sizeMod.y;

	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_DestroyTexture(texture);
}

void renderText(SDL_Renderer* renderer, std::string string, TTF_Font* font, Vec2 pos, Colour colour)
{
	SDL_Color textColour = { colour.r, colour.g, colour.b, 255 };
	SDL_Surface* img = TTF_RenderText_Blended(font, string.c_str(), textColour);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, img);
	SDL_FreeSurface(img);

	SDL_Rect rect = { roundToI(pos.x), roundToI(pos.y), 0, 0 };
	SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);

	SDL_RenderCopy(renderer, texture, NULL, &rect);
	SDL_DestroyTexture(texture);
}

bool collided(Entity entityA, Entity entityB)
{
	Vec2 aMin = entityA.pos;
	Vec2 aMax = entityA.pos + entityA.size;

	Vec2 bMin = entityB.pos;
	Vec2 bMax = entityB.pos + entityB.size;

	if (aMax.x >= bMin.x &&
		aMin.x <= bMax.x &&
		aMax.y >= bMin.y &&
		aMin.y <= bMax.y)
	{
		return true;
	}
	else
	{
		(aMin.x > bMin.x) ? aMin.x -= WINDOW_WIDTH : bMin.x -= WINDOW_WIDTH;
		Vec2 aMax = aMin + entityA.size;
		Vec2 bMax = bMin + entityB.size;

		if (aMax.x >= bMin.x &&
			aMin.x <= bMax.x &&
			aMax.y >= bMin.y &&
			aMin.y <= bMax.y)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool pointInsideRect(Vec2 point, Vec2 rectPos, Vec2 rectSize)
{
	Vec2 rectMin = rectPos;
	Vec2 rectMax = rectPos + rectSize;

	if (point.x > rectMin.x &&
		point.x < rectMax.x &&
		point.y > rectMin.y &&
		point.y < rectMax.y)
	{
		return true;
	}
	else
	{
		rectMin.x -= WINDOW_WIDTH;
		rectMax.x -= WINDOW_WIDTH;
		if (point.x > rectMin.x &&
			point.x < rectMax.x &&
			point.y > rectMin.y &&
			point.y < rectMax.y)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool entityOnScreen(Camera camera, Entity entity)
{
	Entity other;
	other.init = true;
	other.pos = camera.pos;
	other.size = camera.size;
	if (collided(entity, other))
	{
		return true;
	}
	else
	{
		return false;
	}
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, char* file)
{
	SDL_Surface* img = IMG_Load(file);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, img);
	SDL_FreeSurface(img);
	return texture;
}

void renderEntity(SDL_Renderer* renderer, Entity entity, SDL_Texture* texture, Camera camera)
{
	SDL_Rect entityDrawRect;
	if ((entity.pos.x + entity.size.x > WINDOW_WIDTH) && (entity.pos.x <= WINDOW_WIDTH))
	{
		entityDrawRect = { roundToI(entity.pos.x - camera.pos.x), roundToI(entity.pos.y - camera.pos.y), roundToI(entity.size.x), roundToI(entity.size.y) };
		SDL_RenderCopy(renderer, texture, NULL, &entityDrawRect);

		entityDrawRect = { roundToI(entity.pos.x - camera.pos.x - WINDOW_WIDTH), roundToI(entity.pos.y - camera.pos.y), roundToI(entity.size.x), roundToI(entity.size.y) };
		SDL_RenderCopy(renderer, texture, NULL, &entityDrawRect);
	}
	else
	{
		entityDrawRect = { roundToI(entity.pos.x - camera.pos.x), roundToI(entity.pos.y - camera.pos.y), roundToI(entity.size.x), roundToI(entity.size.y) };
		SDL_RenderCopy(renderer, texture, NULL, &entityDrawRect);
	}
}

void renderEntity(SDL_Renderer* renderer, Entity entity, SDL_Texture* texture, SDL_Texture* backgroundTexture, float pickupBackgroundSize, Camera camera)
{
	SDL_Rect entityDrawRect;
	SDL_Rect backgroundRect;
	if ((entity.pos.x + entity.size.x > WINDOW_WIDTH) && (entity.pos.x <= WINDOW_WIDTH))
	{
		entityDrawRect = { roundToI(entity.pos.x - camera.pos.x), roundToI(entity.pos.y - camera.pos.y), roundToI(entity.size.x), roundToI(entity.size.y) };
		backgroundRect = { roundToI((entityDrawRect.x + (entity.size.x / 2.0f)) - pickupBackgroundSize / 2.0f), roundToI((entityDrawRect.y + (entity.size.y / 2.0f)) - pickupBackgroundSize / 2.0f), roundToI(pickupBackgroundSize), roundToI(pickupBackgroundSize) };
		SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
		SDL_RenderCopy(renderer, texture, NULL, &entityDrawRect);

		entityDrawRect = { roundToI(entity.pos.x - camera.pos.x - WINDOW_WIDTH), roundToI(entity.pos.y - camera.pos.y), roundToI(entity.size.x), roundToI(entity.size.y) };
		backgroundRect = { roundToI((entityDrawRect.x + (entity.size.x / 2.0f)) - pickupBackgroundSize / 2.0f), roundToI((entityDrawRect.y + (entity.size.y / 2.0f)) - pickupBackgroundSize / 2.0f), roundToI(pickupBackgroundSize), roundToI(pickupBackgroundSize) };
		SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
		SDL_RenderCopy(renderer, texture, NULL, &entityDrawRect);
	}
	else
	{
		entityDrawRect = { roundToI(entity.pos.x - camera.pos.x), roundToI(entity.pos.y - camera.pos.y), roundToI(entity.size.x), roundToI(entity.size.y) };
		backgroundRect = { roundToI((entityDrawRect.x + (entity.size.x / 2.0f)) - pickupBackgroundSize / 2.0f), roundToI((entityDrawRect.y + (entity.size.y / 2.0f)) - pickupBackgroundSize / 2.0f), roundToI(pickupBackgroundSize), roundToI(pickupBackgroundSize) };
		SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
		SDL_RenderCopy(renderer, texture, NULL, &entityDrawRect);
	}
}

void makeParticle(Particle* &particleList, float particleListSize, Vec2 centerPoint, Vec2 startSize, Vec2 endSize, Vec2 vel, float maxLifeTime, SDL_Texture* texture)
{
	Particle particle = {};
	particle.init = true;
	particle.pos = { centerPoint.x - (startSize.x / 2.0f), centerPoint.y - (startSize.y / 2.0f) };
	particle.vel = vel;
	particle.startSize = startSize;
	particle.endSize = endSize;
	particle.maxLifeTime = maxLifeTime;
	particle.texture = texture;

	//add particle to list
	for (int i = 0; i < particleListSize; i++)
	{
		if (!particleList[i].init)
		{
			particleList[i] = particle;
			break;
		}
	}
}

void makeParticle(Particle* &particleList, float particleListSize, Vec2 centerPoint, Vec2 startSize, Vec2 endSize, float startAngle, float endAngle, Vec2 vel, float maxLifeTime, SDL_Texture* texture)
{
	Particle particle = {};
	particle.init = true;
	particle.pos = { centerPoint.x - (startSize.x / 2.0f), centerPoint.y - (startSize.y / 2.0f) };
	particle.vel = vel;
	particle.startSize = startSize;
	particle.endSize = endSize;
	particle.maxLifeTime = maxLifeTime;
	particle.texture = texture;
	particle.startAngle = startAngle;
	particle.endAngle = endAngle;

	//add particle to list
	for (int i = 0; i < particleListSize; i++)
	{
		if (!particleList[i].init)
		{
			particleList[i] = particle;
			break;
		}
	}
}

#if 0 //make a new function called make interval and then just put the particle code in the implementation
void makeRegularParticle(Particle* &particleList, float particleListSize, float dt, float currentTime, float timeInterval, Vec2 centerPoint, Vec2 startSize, Vec2 endSize, float startAngle, float endAngle, Vec2 vel, float maxLifeTime, SDL_Texture* texture)
{
	if ((currentTime - floor(currentTime / timeInterval)) <= dt)
	{
		makeParticle(particleList, particleListSize, centerPoint, startSize, endSize, startAngle, endAngle, vel, maxLifeTime, texture);
	}
}
#endif

#if 0
bool timeInterval(float currentTime, float dt, float interval)
{
	if ((currentTime - floor(currentTime / interval)) <= dt)
	{
		return true;
	}
	else
	{
		return false;
	}
}
#endif


void renderParticles(Particle particleList[], float particleListSize, Camera camera, SDL_Renderer* renderer, float dt)
{
	for (int i = 0; i < particleListSize; i++)
	{
		Particle &particle = particleList[i];
		if (particle.init)
		{
			float particleTime = particle.lifeTime / particle.maxLifeTime;
			Vec2 particleSize = lerp(particle.startSize, particle.endSize, particleTime);

			float particleAngle = lerp(particle.startAngle, particle.endAngle, particleTime);

			SDL_Rect particleRect = { roundToI(particle.pos.x - camera.pos.x), roundToI(particle.pos.y - camera.pos.y), roundToI(particleSize.x), roundToI(particleSize.y) };
			SDL_Point particleAngleCentre = { roundToI((particleSize.x / 2.0f)), roundToI((particleSize.y / 2.0f)) };

			SDL_RenderCopyEx(renderer, particle.texture, NULL, &particleRect, particleAngle, &particleAngleCentre, SDL_RendererFlip{ SDL_FLIP_NONE });
		}
	}
}

void boardUpdateAndRender(SDL_Window* window, SDL_Renderer* renderer, GameInput gameInput, float dt, GameState &gameState, BoardType boardType)
{
	MainState &mainState = gameState.mainState;
	Assets assets = gameState.assets;
	switch (boardType)
	{
	case BT_TUTORIAL:
	{
		doBoard(renderer, gameInput, dt, gameState, assets.tutorialBoard, GS_MAIN_GAME, gameState.menuState.tutorial);
		break;
	}
	case BT_JUMP:
	{
		doBoard(renderer, gameInput, dt, gameState, assets.jumpBoard, GS_UPGRADES, gameState.upgradesState.jumpTutorial);
		break;
	}
	case BT_CHANGE_BLOCK:
	{
		doBoard(renderer, gameInput, dt, gameState, assets.changeBlockBoard, GS_UPGRADES, gameState.upgradesState.changeBlockTutorial);
		break;
	}
	case BT_TELEPORT:
	{
		doBoard(renderer, gameInput, dt, gameState, assets.teleportBoard, GS_UPGRADES, gameState.upgradesState.teleportTutorial);
		break;
	}
	case BT_POWERUPS_UNLOCKED:
	{
		doBoard(renderer, gameInput, dt, gameState, assets.powerupsUnlockedBoard, GS_UPGRADES, gameState.upgradesState.powerupsUnlock);
		break;
	}
	}
}

void doBoard(SDL_Renderer* renderer, GameInput gameInput, float dt, GameState &gameState, SDL_Texture* board, GameScreen gotoScreen, bool &tutorial)
{
	gameState.mainState.tutorialTime += dt;
	if (gameState.mainState.tutorialTime > 0.5f && gameInput.anyKeyDown)
	{
		gameState.mainState.tutorialTime = 0.0f;
		gameState.screen = gotoScreen;
		tutorial = false;
	}
	else
	{
		SDL_RenderCopy(renderer, board, NULL, NULL);
		SDL_RenderPresent(renderer);
	}
}
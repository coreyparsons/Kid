#include "menu.h"
#include "platform_layer.h"

#include <iostream>

void menuUpdateAndRender(SDL_Window* window, SDL_Renderer* renderer, GameInput gameInput, float dt, GameState &gameState)
{
	Assets &assets = gameState.assets;
	MainState &mainState = gameState.mainState;
	Entity &player = mainState.entityList[(int)(mainState.entityListSize - 1)];
	MenuState &menuState = gameState.menuState;

	float &amountParticles = menuState.amountParticles;
	float &particleListSize = menuState.particleListSize;
	Particle* &particleList = menuState.particleList;
	bool &dieClicked = menuState.dieClicked;
	bool &quitClicked = menuState.quitClicked;

	if (!menuState.init)
	{
		dt = 2.0f;
		quitClicked = false;
		dieClicked = false;
		menuState.isPausingGame = false;
		menuState.init = true;
	}

	if (!assets.init)
	{
		//init assets (should put these in the file they need to be in, not too sure how to do it and might not bother)
		assets.upgradesBackground = loadTexture(renderer, "resources/images/upgrades_background.png");
		assets.playerTexture = loadTexture(renderer, "resources/images/player/player_body.png");
		assets.playerEyeTexture = loadTexture(renderer, "resources/images/player/player_eye.png");
		assets.playerBlinkTexture = loadTexture(renderer, "resources/images/player/player_blinking.png");
		assets.jumpIconTexture = loadTexture(renderer, "resources/images/upgrades/jump.png");
		assets.changeBlockIconTexture = loadTexture(renderer, "resources/images/upgrades/change_block.png");
		assets.teleportIconTexture = loadTexture(renderer, "resources/images/upgrades/teleport.png");
		assets.jumpIntensityIconTexture = loadTexture(renderer, "resources/images/upgrades/jump_intensity.png");
		assets.plusTexture = loadTexture(renderer, "resources/images/buttons/plus.png");
		assets.minusTexture = loadTexture(renderer, "resources/images/buttons/minus.png");
		assets.coinTexture = loadTexture(renderer, "resources/images/upgrades/coin.png");
		assets.jumpPickupTexture = loadTexture(renderer, "resources/images/upgrades/jump_pickup.png");
		assets.changeBlockPickupTexture = loadTexture(renderer, "resources/images/upgrades/block_changer_pickup.png");
		assets.teleportPickupTexture = loadTexture(renderer, "resources/images/upgrades/teleport_pickup.png");
		assets.coinIconTexture = loadTexture(renderer, "resources/images/upgrades/coin_icon.png");
		assets.pickupsIconTexture = loadTexture(renderer, "resources/images/upgrades/pickups_icon.png");
		assets.coinBackgroundTexture = loadTexture(renderer, "resources/images/upgrades/coin_background.png");
		assets.pickupBackgroundTexture = loadTexture(renderer, "resources/images/upgrades/pickup_background.png");
		assets.snowflakeTexture = loadTexture(renderer, "resources/images/particles/snowflake.png");
		assets.dustTexture = loadTexture(renderer, "resources/images/particles/dust.png");
		assets.sparkleTexture = loadTexture(renderer, "resources/images/particles/sparkle.png");
		assets.spiralCloudTexture = loadTexture(renderer, "resources/images/particles/spiral_cloud.png");
		assets.blueSwirlTexture = loadTexture(renderer, "resources/images/particles/blue_swirl.png");
		assets.lavaSpewTexture = loadTexture(renderer, "resources/images/particles/lava_spew.png");
		assets.bloodTexture = loadTexture(renderer, "resources/images/particles/blood.png");

		assets.tutorialBoard = loadTexture(renderer, "resources/images/help_boards/tutorial.png");
		assets.jumpBoard = loadTexture(renderer, "resources/images/help_boards/extra_jump.png");
		assets.changeBlockBoard = loadTexture(renderer, "resources/images/help_boards/change_block.png");
		assets.teleportBoard = loadTexture(renderer, "resources/images/help_boards/teleport.png");
		assets.powerupsUnlockedBoard = loadTexture(renderer, "resources/images/help_boards/powerups_unlocked.png");

		assets.buttonSelectSound = Mix_LoadWAV("resources/audio/sounds/button_select.wav");
		assets.coinPickupSound = Mix_LoadWAV("resources/audio/sounds/coin_pickup.wav");
		assets.powerupPickupSound = Mix_LoadWAV("resources/audio/sounds/powerup_pickup.wav");
		assets.teleportSound = Mix_LoadWAV("resources/audio/sounds/teleport.wav");
		assets.extraJumpSound = Mix_LoadWAV("resources/audio/sounds/jump.wav");
		assets.changeBlockSound = Mix_LoadWAV("resources/audio/sounds/change_block.wav");
		assets.invalidSelectionSound = Mix_LoadWAV("resources/audio/sounds/invalid_selection.wav");
		assets.squashSound = Mix_LoadWAV("resources/audio/sounds/squash.wav");
		assets.lavaDeathSound = Mix_LoadWAV("resources/audio/sounds/lava.wav");

		assets.goodTimesFont = TTF_OpenFont("resources/fonts/good_times.ttf", 50);
		assets.goodTimesFontLarge = TTF_OpenFont("resources/fonts/good_times.ttf", 200);

		Mix_VolumeMusic(MIX_MAX_VOLUME * menuState.musicVolume);
		assets.gameMusic = Mix_LoadMUS("resources/audio/music/game_music.ogg");

		assets.init = true;
	}

	Mix_PauseMusic();

	//back to game
	if (gameInput.backPressed && menuState.isPausingGame)
	{
		gameState.screen = gameState.prevScreen;
		quitClicked = false;
		dieClicked = false;
	}

	drawRect(renderer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 100, 100, 100);

	renderMenuBackground(renderer, assets, menuState, dt);

	Vec2 size = getTextSize("Kid", assets.goodTimesFont);
	renderText(renderer, "Kid", assets.goodTimesFont, Vec2{ (WINDOW_WIDTH / 2.0f) - (size.x / 2.0f), 100 - (size.y / 2.0f) }, Colour{ 0, 0, 0 });

	Vec2 tableStart = { (WINDOW_WIDTH / 2.0f), 250.0f };
	Vec2 tableSpace = { 0.0f, 90.0f };

	Vec2 buttonPos = tableStart;
	size = getTextSize("play", assets.goodTimesFont);
	if (textButton(renderer, assets, gameInput, buttonPos - (size / 2.0f), size, "Play", assets.goodTimesFont))
	{
		gameState.screen = gameState.prevScreen;
		quitClicked = false;
		dieClicked = false;
	}

	buttonPos += tableSpace;
	size = getTextSize("Options", assets.goodTimesFont);
	if (textButton(renderer, assets, gameInput, buttonPos - (size / 2.0f), size, "Options", assets.goodTimesFont))
	{
		gameState.screen = GS_OPTIONS;
		quitClicked = false;
		dieClicked = false;
	}

	if (player.init)
	{
		buttonPos += tableSpace;
		Vec2 size = getTextSize("Die", assets.goodTimesFont);
		if (textButton(renderer, assets, gameInput, buttonPos - (size / 2.0f), size, "Die", assets.goodTimesFont))
		{
			dieClicked = !dieClicked;
		}
	}

	if (dieClicked)
	{
		buttonPos += tableSpace;
		Vec2 dieSize = getTextSize("Sure?", assets.goodTimesFont);
		renderText(renderer, "Sure?", assets.goodTimesFont, buttonPos - (dieSize / 2.0f), Colour{ 0, 0, 0 });

		Vec2 yesSize = getTextSize("Yes", assets.goodTimesFont);
		Vec2 sureSize = getTextSize("Sure?", assets.goodTimesFont);
		if (textButton(renderer, assets, gameInput, Vec2{ buttonPos.x - (yesSize.x / 2.0f) + sureSize.x - 10.0f, buttonPos.y - (size.y / 2.0f) }, yesSize, "Yes", assets.goodTimesFont))
		{
			player.size.y = 0.0f;
			gameState.screen = GS_MAIN_GAME;
			dieClicked = false;
		}
		size = getTextSize("No", assets.goodTimesFont);
		if (textButton(renderer, assets, gameInput, Vec2{ buttonPos.x - (size.x / 2.0f) + sureSize.x + yesSize.x + 20.0f, buttonPos.y - (size.y / 2.0f) }, size, "No", assets.goodTimesFont))
		{
			dieClicked = false;
		}
	}

	buttonPos += tableSpace;
	size = getTextSize("Quit", assets.goodTimesFont);
	if (textButton(renderer, assets, gameInput, buttonPos - (size / 2.0f), size, "Quit", assets.goodTimesFont))
	{
		quitClicked = !quitClicked;
	}

	if (quitClicked)
	{
		buttonPos += tableSpace;
		Vec2 quitSize = getTextSize("Sure?", assets.goodTimesFont);
		renderText(renderer, "Sure?", assets.goodTimesFont, buttonPos - (quitSize / 2.0f), Colour{ 0, 0, 0 });

		Vec2 yesSize = getTextSize("Yes", assets.goodTimesFont);
		Vec2 sureSize = getTextSize("Sure?", assets.goodTimesFont);
		if (textButton(renderer, assets, gameInput, Vec2{ buttonPos.x - (yesSize.x / 2.0f) + sureSize.x - 10.0f, buttonPos.y - (size.y / 2.0f) }, yesSize, "Yes", assets.goodTimesFont))
		{
			gameState.gameRunning = false;
			quitClicked = false;
		}
		size = getTextSize("No", assets.goodTimesFont);
		if (textButton(renderer, assets, gameInput, Vec2{ buttonPos.x - (size.x / 2.0f) + sureSize.x + yesSize.x + 20.0f, buttonPos.y - (size.y / 2.0f) }, size, "No", assets.goodTimesFont))
		{
			quitClicked = false;
		}
	}

	SDL_RenderPresent(renderer);
}

void renderMenuBackground(SDL_Renderer* renderer, Assets assets, MenuState &menuState, float dt)
{
	//make particles
	while (menuState.amountParticles < menuState.particleListSize)
	{
		Vec2 center = { (WINDOW_WIDTH / 2.0f) + randInt(-500, 500), (WINDOW_HEIGHT / 2.0f) + randInt(-400, 400) };
		float size = randInt(0, 200);
		makeParticle(menuState.particleList, menuState.particleListSize, center, Vec2{ size, size }, Vec2{ 300, 300 }, randInt(0, 180), randInt(0, 180), Vec2{ randInt(-110, 100), randInt(-110, 100) }, randInt(100, 1000) / 100.0f, assets.dustTexture);
		menuState.amountParticles++;
	}

	//update particles
	for (int i = 0; i < menuState.particleListSize; i++)
	{
		Particle &particle = menuState.particleList[i];
		if (particle.init)
		{
			particle.pos += particle.vel * dt;
			particle.lifeTime += dt;
			if (particle.lifeTime >= particle.maxLifeTime)
			{
				menuState.amountParticles--;
				particle = {};
			}
		}
	}

	Camera camera = {};
	camera.size = { WINDOW_WIDTH, WINDOW_HEIGHT };

	renderParticles(menuState.particleList, menuState.particleListSize, camera, renderer, dt);
}
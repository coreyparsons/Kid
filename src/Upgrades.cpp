#include "upgrades.h"
#include "platform_layer.h"
#include "sdl_mixer.h"

#include <iostream>

void upgradesUpdateAndRender(SDL_Window* window, SDL_Renderer* renderer, GameInput gameInput, float dt, GameState &gameState)
{
	UpgradesState &upgradesState = gameState.upgradesState;
	MainState &mainState = gameState.mainState;
	Assets &assets = gameState.assets;

	TTF_Font* &goodTimesFont = assets.goodTimesFont;
	TTF_Font* &goodTimesFontLarge = assets.goodTimesFontLarge;
	float &playerMaxJumps = upgradesState.playerMaxJumps;
	float &maxBlockChanges = upgradesState.maxBlockChanges;
	float &maxTeleports = upgradesState.maxTeleports;
	float &coinIntensity = upgradesState.coinIntensity;
	float &extraJumpIntensity = upgradesState.extraJumpIntensity;
	float &pickupIntensity = upgradesState.pickupIntensity;
	float &points = upgradesState.points;

	float &savedPoints = upgradesState.savedPoints;
	float &savedPlayerMaxJumps = upgradesState.savedPlayerMaxJumps;
	float &savedMaxBlockChanges = upgradesState.savedMaxBlockChanges;
	float &savedMaxTeleports = upgradesState.savedMaxTeleports;
	float &savedExtraJumpIntensity = upgradesState.savedExtraJumpIntensity;
	float &savedCoinIntensity = upgradesState.savedCoinIntensity;
	float &savedPickupIntensity = upgradesState.savedPickupIntensity;

	bool powerupsUnlcoked = (playerMaxJumps > 0.0f && maxBlockChanges > 0.0f && maxTeleports > 0.0f);

	//tutorials / unlocks
	if (playerMaxJumps > 0.0f && upgradesState.jumpTutorial)
	{
		gameState.screen = GS_BOARD;
		gameState.menuState.boardType = BT_JUMP;
		return;
	}
	if (maxBlockChanges > 0.0f && upgradesState.changeBlockTutorial)
	{
		gameState.screen = GS_BOARD;
		gameState.menuState.boardType = BT_CHANGE_BLOCK;
		return;
	}
	if (maxTeleports > 0.0f && upgradesState.teleportTutorial)
	{
		gameState.screen = GS_BOARD;
		gameState.menuState.boardType = BT_TELEPORT;
		return;
	}
	if (powerupsUnlcoked && upgradesState.powerupsUnlock)
	{
		gameState.screen = GS_BOARD;
		gameState.menuState.boardType = BT_POWERUPS_UNLOCKED;
		return;
	}


	if (upgradesState.firstLoop)
	{
		Mix_FadeOutMusic(3000);

		savedPoints = points;
		savedPlayerMaxJumps = playerMaxJumps;
		savedMaxBlockChanges = maxBlockChanges;
		savedMaxTeleports = maxTeleports;
		savedExtraJumpIntensity = extraJumpIntensity;
		savedCoinIntensity = coinIntensity;
		savedPickupIntensity = pickupIntensity;

#if 1
		upgradesState.upgradeBoxData.upgradeLevel = -1.0f;
		upgradesState.upgradeBoxData.title = " ";
		upgradesState.upgradeBoxData.description = " ";
#endif

		upgradesState.firstLoop = false;
	}

	if (gameInput.backPressed)
	{
		gameState.prevScreen = gameState.screen;
		gameState.screen = GS_MENU;
	}

	//render
	//drawRect(renderer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 100, 100, 100);
	SDL_RenderCopy(renderer, assets.upgradesBackground, NULL, NULL);

	renderText(renderer, "Upgrades", goodTimesFont, Vec2{ 50, 40 }, Colour{ 0, 0, 0 });

	float midRight = 1050.0f + ((WINDOW_WIDTH - 1050.0f) / 2.0f);

	std::string pointsString = "Points: " + std::to_string((int)points);
	renderText(renderer, pointsString, goodTimesFont, Vec2{ midRight - (getTextSize(pointsString, goodTimesFont).x / 2.0f), 40 }, Colour{ 0, 0, 0 });

	//text box
	SDL_Rect textBox = { midRight - 350, 130, 700, 590 };
	drawRect(renderer, textBox, SDL_Colour{ 40, 40, 40 });

	//upgradeBox(renderer, assets, playerMaxJumps, "Extra Jump", "Use the W key to jump once more after your normal jump. Each upgrade level will give you one additional jump.");

	Vec2 size = getTextSize("Undo", goodTimesFont);
	if (textButton(renderer, assets, gameInput, Vec2{ midRight - (size.x * 1.5f / 2.0f), 750 }, size * 1.5f, "Undo", goodTimesFontLarge))
	{
		points = savedPoints;
		playerMaxJumps = savedPlayerMaxJumps;
		maxBlockChanges = savedMaxBlockChanges;
		maxTeleports = savedMaxTeleports;
		extraJumpIntensity = savedExtraJumpIntensity;
		coinIntensity = savedCoinIntensity;
		pickupIntensity = savedPickupIntensity;
	}

	size = getTextSize("Next Game", goodTimesFont);
	if (textButton(renderer, assets, gameInput, Vec2{ midRight - (size.x * 1.5f / 2.0f), 910 }, size * 1.5f, "Next Game", goodTimesFontLarge))
	{
		gameState.mainState = {};
		upgradesState.firstLoop = true;
		gameState.screen = GS_MAIN_GAME;
	}

	//upgrades table
	std::string costString;
	float tableGap = 150;
	SDL_Rect rect;
	UpgradeBoxData boxData;

	SDL_Colour backColour = { 50, 50, 50, 255 };

	rect = { 50, 150, 100, 100 };
	SDL_Rect backRect = { rect.x - 20, rect.y - 20, 1030, 140 };
	drawRect(renderer, backRect, backColour);
	if (pointInsideRect(gameInput.mousePos, Vec2{ (float)backRect.x, (float)backRect.y }, Vec2{ (float)backRect.w, (float)backRect.h }))
	{
		boxData.upgradeLevel = playerMaxJumps;
		boxData.title = "Extra Jump";
		boxData.description = "Use the W key to jump once more after your normal jump. Each upgrade level will give you one additional jump.";
		upgradesState.upgradeBoxData = boxData;
	}
	upgrade(renderer, assets, gameInput, rect, playerMaxJumps, points, assets.jumpIconTexture);

	rect.y += tableGap;
	backRect = { rect.x - 20, rect.y - 20, 1030, 140 };
	drawRect(renderer, backRect, backColour);
	if (pointInsideRect(gameInput.mousePos, Vec2{ (float)backRect.x, (float)backRect.y }, Vec2{ (float)backRect.w, (float)backRect.h }))
	{
		boxData.upgradeLevel = maxBlockChanges;
		boxData.title = "Block Change";
		boxData.description = "Use the LMB on a non-moving box to make it dissapear. It will behave like a normal box except you can pass through it. You can use this ability once per upgrade level.";
		upgradesState.upgradeBoxData = boxData;
	}
	upgrade(renderer, assets, gameInput, rect, maxBlockChanges, points, assets.changeBlockIconTexture);

	rect.y += tableGap;
	backRect = { rect.x - 20, rect.y - 20, 1030, 140 };
	drawRect(renderer, backRect, backColour);
	if (pointInsideRect(gameInput.mousePos, Vec2{ (float)backRect.x, (float)backRect.y }, Vec2{ (float)backRect.w, (float)backRect.h }))
	{
		boxData.upgradeLevel = maxTeleports;
		boxData.title = "Teleportation";
		boxData.description = "Use the RMB in the air to teleport to the location that you click. You can use this ability once per upgrade level.";
		upgradesState.upgradeBoxData = boxData;
	}
	upgrade(renderer, assets, gameInput, rect, maxTeleports, points, assets.teleportIconTexture);

	rect.y += tableGap;
	backRect = { rect.x - 20, rect.y - 20, 1030, 140 };
	drawRect(renderer, backRect, backColour);
	if (pointInsideRect(gameInput.mousePos, Vec2{ (float)backRect.x, (float)backRect.y }, Vec2{ (float)backRect.w, (float)backRect.h }))
	{
		boxData.upgradeLevel = coinIntensity;
		boxData.title = "Coin Frequency"	;
		boxData.description = "Each point spent in this upgrade will increase the frequency of coins that appear in the world.";
		upgradesState.upgradeBoxData = boxData;
	}
	upgrade(renderer, assets, gameInput, rect, coinIntensity, points, assets.coinIconTexture);

	rect.y += tableGap;
	if (playerMaxJumps > 0.0f)
	{
		backRect = { rect.x - 20, rect.y - 20, 1030, 140 };
		drawRect(renderer, backRect, backColour);
		if (pointInsideRect(gameInput.mousePos, Vec2{ (float)backRect.x, (float)backRect.y }, Vec2{ (float)backRect.w, (float)backRect.h }))
		{
			boxData.upgradeLevel = extraJumpIntensity;
			boxData.title = "Extra Jump Height";
			boxData.description = "Spending points into this upgrade will increase the height of your extra jumps. This will not increase your regular jump height.";
			upgradesState.upgradeBoxData = boxData;
		}
		upgrade(renderer, assets, gameInput, rect, extraJumpIntensity, points, assets.jumpIntensityIconTexture);
	}
	else
	{
		renderText(renderer, "Locked", assets.goodTimesFont, Vec2{ (float)rect.x, (float)rect.y }, Colour{});
	}

	rect.y += tableGap;
	if (powerupsUnlcoked)
	{
		backRect = { rect.x - 20, rect.y - 20, 1030, 140 };
		drawRect(renderer, backRect, backColour);
		if (pointInsideRect(gameInput.mousePos, Vec2{ (float)backRect.x, (float)backRect.y }, Vec2{ (float)backRect.w, (float)backRect.h }))
		{
			boxData.upgradeLevel = pickupIntensity;
			boxData.title = "Pickup frequency";
			boxData.description = "Each point spent in this upgrade will increase the frequency of powerups that appear in the world.";
			upgradesState.upgradeBoxData = boxData;
		}
		upgrade(renderer, assets, gameInput, rect, pickupIntensity, points, assets.pickupsIconTexture);
	}
	else
	{
		renderText(renderer, "Locked", assets.goodTimesFont, Vec2{ (float)rect.x, (float)rect.y }, Colour{});
	}

	upgradeBox(renderer, assets, upgradesState.upgradeBoxData);

	SDL_RenderPresent(renderer);
}

void upgrade(SDL_Renderer* renderer, Assets assets, GameInput gameInput, SDL_Rect rect, float &upgradeLevel, float &points, SDL_Texture* iconTexture)
{
	float cost = costOf(upgradeLevel + 1);
	SDL_RenderCopy(renderer, iconTexture, NULL, &rect);

	for (int i = 0; i < upgradeLevel; i++)
	{
		rect.x += 120;
		drawRect(renderer, rect, SDL_Color{ 0, 0, 0, 255 });
	}

	for (int i = upgradeLevel; i < 5; i++)
	{
		rect.x += 120;
		if (points >= getCostDiff(upgradeLevel, i + 1))
		{
			if (button(renderer, assets, gameInput, Vec2{ (float)rect.x, (float)rect.y }, Vec2{ (float)rect.w, (float)rect.h }, Colour{ 200, 255, 200 }, "", assets.goodTimesFont))
			{
				points -= getCostDiff(upgradeLevel, i + 1);
				upgradeLevel = i + 1;
			}
#if 0
			drawOutlineRect(renderer, rect, SDL_Colour{ 0, 0, 0, 255 });
			SDL_Rect innerRect = { rect.x + 2, rect.y + 2, rect.w - 4, rect.h - 4 };
			drawOutlineRect(renderer, innerRect, SDL_Colour{ 0, 0, 0, 255 });
#endif
		}
		else
		{
			drawRect(renderer, rect, SDL_Color{ 255, 200, 200, 255 });
		}
		drawOutlineRect(renderer, rect, SDL_Colour{ 0, 0, 0, 255 });
		SDL_Rect innerRect = { rect.x + 2, rect.y + 2, rect.w - 4, rect.h - 4 };
		drawOutlineRect(renderer, innerRect, SDL_Colour{ 0, 0, 0, 255 });
	}

	if (upgradeLevel < 5)
	{
		std::string costString = "$" + std::to_string((int)cost);
		renderText(renderer, costString, assets.goodTimesFont, Vec2{ rect.x + 240.0f, (float)(rect.y + 25) }, Colour{ 0, 0, 0 });
		if (points >= cost)
		{
			if (imgButton(renderer, assets, gameInput, assets.plusTexture, Vec2{ (float)(rect.x + 120), (float)(rect.y) }, Vec2{ 100, 100 }))
			{
				upgradeLevel++;
				points -= cost;
			}
		}
		else
		{
			SDL_Rect plusRect = { rect.x + 120, rect.y, 100, 100 };
			SDL_RenderCopy(renderer, assets.plusTexture, NULL, &plusRect);
		}
	}
	else
	{
		renderText(renderer, "Done", assets.goodTimesFont, Vec2{ rect.x + 120.0f, (float)(rect.y + 25) }, Colour{ 0, 0, 0 });
	}
}

float costOf(float upgradeLevel)
{
	float result = ((roundToI(square(upgradeLevel) / 2.0f)) * 20.0f);
	return result;
}

float getCostDiff(float oldUpgradeLevel, float newUpgradeLevel)
{
	float result = 0.0f;
	if (oldUpgradeLevel >= newUpgradeLevel)
	{
		return 0.0f;
	}
	else
	{
		result = costOf(newUpgradeLevel) + getCostDiff(oldUpgradeLevel, newUpgradeLevel - 1.0f);
		return result;
	}
}

bool button(SDL_Renderer* renderer, Assets assets, GameInput gameInput, Vec2 pos, Vec2 size, Colour colour, std::string text, TTF_Font* font)
{
	if (pointInsideRect(gameInput.mousePos, pos, size))
	{
		drawRect(renderer, pos.x, pos.y, size.x, size.y, min(colour.r + 70, 255), min(colour.g + 70, 255), min(colour.b + 70, 255));
		renderText(renderer, text, font, pos, size, Colour{ 0, 0, 0 });
		if (gameInput.mouseButton.leftClicked)
		{
			Mix_PlayChannel(-1, assets.buttonSelectSound, 0);
			return true;
		}
	}
	else
	{
		drawRect(renderer, pos.x, pos.y, size.x, size.y, colour.r, colour.g, colour.b);
		renderText(renderer, text, font, pos, size, Colour{ 0, 0, 0 });
	}
	return false;
}

bool imgButton(SDL_Renderer* renderer, Assets assets, GameInput gameInput, SDL_Texture* texture, Vec2 pos, Vec2 size, std::string text, TTF_Font* font)
{
	if (pointInsideRect(gameInput.mousePos, pos, size))
	{
		SDL_Rect textureRect = rectFromPosAndSize(pos - 10, size + 20);
		SDL_RenderCopy(renderer, texture, NULL, &textureRect);
		renderText(renderer, text, font, pos - 10, size + 20, Colour{ 0, 0, 0 });
		if (gameInput.mouseButton.leftClicked)
		{
			Mix_PlayChannel(-1, assets.buttonSelectSound, 0);
			return true;
		}
	}
	else
	{
		SDL_Rect textureRect = rectFromPosAndSize(pos, size);
		SDL_RenderCopy(renderer, texture, NULL, &textureRect);
		renderText(renderer, text, font, pos, size, Colour{ 0, 0, 0 });
	}
	return false;
}

bool imgButton(SDL_Renderer* renderer, Assets assets, GameInput gameInput, SDL_Texture* texture, Vec2 pos, Vec2 size)
{
	if (pointInsideRect(gameInput.mousePos, pos, size))
	{
		SDL_Rect textureRect = rectFromPosAndSize(pos - 10, size + 20);
		SDL_RenderCopy(renderer, texture, NULL, &textureRect);
		if (gameInput.mouseButton.leftClicked)
		{
			Mix_PlayChannel(-1, assets.buttonSelectSound, 0);
			return true;
		}
	}
	else
	{
		SDL_Rect textureRect = rectFromPosAndSize(pos, size);
		SDL_RenderCopy(renderer, texture, NULL, &textureRect);
	}
	return false;
}

bool textButton(SDL_Renderer* renderer, Assets assets, GameInput gameInput, Vec2 pos, Vec2 size, std::string text, TTF_Font* font)
{
	if (size.x == 0 && size.y == 0)
	{
		size = getTextSize(text, font);
	}

	if (pointInsideRect(gameInput.mousePos, pos, size))
	{
		renderText(renderer, text, font, pos, size, Colour{ 150, 150, 150 });
		if (gameInput.mouseButton.leftClicked)
		{
			Mix_PlayChannel(-1, assets.buttonSelectSound, 0);
			return true;
		}
	}
	else
	{
		renderText(renderer, text, font, pos, size, Colour{ 0, 0, 0 });
	}
	return false;
}

SDL_Rect rectFromPosAndSize(Vec2 pos, Vec2 size)
{
	SDL_Rect result = { roundToI(pos.x), roundToI(pos.y), roundToI(size.x), roundToI(size.y) };
	return result;
}

Vec2 getTextSize(std::string text, TTF_Font* font)
{
	SDL_Rect rect = {};
	TTF_SizeText(font, text.c_str(), &rect.w, &rect.h);
	Vec2 result = { (float)rect.w, (float)rect.h };
	return result;
}

void upgradeBox(SDL_Renderer* renderer, Assets assets, float upgradeLevel, std::string title, std::string description)
{
	float midRight = 1050.0f + ((WINDOW_WIDTH - 1050.0f) / 2.0f);
	Colour textBoxColour = { 170, 170, 170 };

	renderText(renderer, title, assets.goodTimesFont, Vec2{ midRight - (getTextSize(title, assets.goodTimesFont).x / 2.0f), 150.0f }, textBoxColour);

	if (upgradeLevel >= 0.0f)
	{
		std::string levelString = "Level: " + std::to_string((int)upgradeLevel);
		renderText(renderer, levelString, assets.goodTimesFont, Vec2{ midRight - 320.0f, 250.0f }, textBoxColour);
	}

	Vec2 descriptionSize = getTextSize(description, assets.goodTimesFont);
	renderTextWrapped(renderer, description, assets.goodTimesFont, Vec2{ midRight - 320.0f, 425.0f }, Vec2{ 0.6f, 0.6f }, textBoxColour, 640);
}

void upgradeBox(SDL_Renderer* renderer, Assets assets, UpgradeBoxData upgradeBoxData)
{
	upgradeBox(renderer, assets, upgradeBoxData.upgradeLevel, upgradeBoxData.title, upgradeBoxData.description);
}
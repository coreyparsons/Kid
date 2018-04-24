#include "options.h"
#include "platform_layer.h"
#include "menu.h"

#include <iostream>

void optionsUpdateAndRender(SDL_Window* window, SDL_Renderer* renderer, GameInput gameInput, float &dt, GameState &gameState, bool &vsync)
{
	Assets &assets = gameState.assets;
	MenuState &menuState = gameState.menuState;

	float &soundVolume = menuState.soundVolume;
	float &musicVolume = menuState.musicVolume;
	float &snowIntensity = menuState.snowIntensity;

	if (gameInput.backPressed)
	{
		gameState.screen = GS_MENU;
	}

	drawRect(renderer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 100, 100, 100);

	renderMenuBackground(renderer, assets, gameState.menuState, dt);

	Vec2 textSize = getTextSize("Options", assets.goodTimesFont);
	renderText(renderer, "Options", assets.goodTimesFont, Vec2{ (WINDOW_WIDTH / 2.0f) - (textSize.x / 2.0f), 100.0f }, Colour{ 0, 0, 0 });

	float tableGap = 90.0f;
	Vec2 textPos = { 0, 235.0f - tableGap };

	//sound
	percentOption(renderer, assets, gameInput, textPos, tableGap, "Sound: ", soundVolume);
	Mix_Volume(-1, MIX_MAX_VOLUME * soundVolume);

	//music
	percentOption(renderer, assets, gameInput, textPos, tableGap, "Music: ", musicVolume);
	Mix_VolumeMusic(MIX_MAX_VOLUME * musicVolume);

	//snow
	percentOption(renderer, assets, gameInput, textPos, tableGap, "Snow: ", snowIntensity);

	//fps
	if (!menuState.startVsync)
	{
		float fps = 1.0f / dt;
		textSize = getTextSize("FPS: ", assets.goodTimesFont);
		textPos.x = (WINDOW_WIDTH / 2.0f) - (textSize.x / 2.0f) - 135;
		textPos.y += tableGap;
		renderText(renderer, "FPS: ", assets.goodTimesFont, textPos, Colour{ 0, 0, 0 });
		std::string string;
		if (roundToI(fps) == 144)
		{
			string = std::to_string(roundToI(fps));
		}
		else
		{
			string = std::to_string(roundToI(fps / 5) * 5);
		}
		renderText(renderer, string, assets.goodTimesFont, Vec2{ textPos.x + textSize.x + 60.0f, textPos.y }, Colour{ 0, 0, 0 });
		if (imgButton(renderer, assets, gameInput, assets.minusTexture, Vec2{ textPos.x + textSize.x, textPos.y + 5.0f }, Vec2{ 50, 50 }))
		{
			if (roundToI(fps) > 20.0f && fps < 125.0f)
			{
				fps -= 5.0f;
				fps = roundToI(fps / 5.0f) * 5.0f;
				dt = 1.0f / fps;
			}
			else if (roundToI(fps) == 144.0f)
			{
				fps = 120.0f;
				dt = 1.0f / fps;
			}
		}
		if (imgButton(renderer, assets, gameInput, assets.plusTexture, Vec2{ textPos.x + textSize.x + 170, textPos.y + 5.0f }, Vec2{ 50, 50 }))
		{
			if (roundToI(fps) < 120.0f)
			{
				fps += 5.0f;
				fps = roundToI(fps / 5.0f) * 5.0f;
				dt = 1.0f / fps;
			}
			else if (roundToI(fps) == 120.0f)
			{
				fps = 144.0f;
				dt = 1.0f / fps;
			}
		}
	}

	//vsync
	std::string text;
	if (vsync)
	{
		text = "Vsync: Enabled";
	}
	else
	{
		text = "Vsync: Disabled";
	}

	textSize = getTextSize(text, assets.goodTimesFont);
	textPos.x = (WINDOW_WIDTH / 2.0f) - (textSize.x / 2.0f);
	textPos.y += tableGap;
	if (textButton(renderer, assets, gameInput, textPos, textSize, text, assets.goodTimesFont))
	{
		vsync = !vsync;
	}
	if (vsync != menuState.startVsync)
	{
		Vec2 size = getTextSize("(Requires Restart)", assets.goodTimesFont);
		renderText(renderer, "(Requires Restart)", assets.goodTimesFont, Vec2{ textPos.x + textSize.x + 20.0f, textPos.y + (size.y / 4.0f) }, size * 0.5f, Colour{ 255, 0, 0 });
	}

	//fullscreen (F11)
	textSize = getTextSize("(Fullscreen is F11)", assets.goodTimesFont);
	textPos.x = (WINDOW_WIDTH / 2.0f) - (textSize.x / 2.0f);
	textPos.y += tableGap * 1.5f;
	renderText(renderer, "(Fullscreen is F11)", assets.goodTimesFont, textPos, Colour{ 0, 0, 0 });

	//back
	textSize = getTextSize("Back", assets.goodTimesFont);
	textPos.x = (WINDOW_WIDTH / 2.0f) - (textSize.x / 2.0f);
	textPos.y += tableGap;
	if (textButton(renderer, assets, gameInput, textPos, textSize, "Back", assets.goodTimesFont))
	{
		gameState.screen = GS_MENU;
	}


	SDL_RenderPresent(renderer);

}


void percentOption(SDL_Renderer* renderer, Assets assets, GameInput gameInput, Vec2 &textPos, float tableGap, std::string text, float &value)
{
	Vec2 textSize = getTextSize(text, assets.goodTimesFont);
	textPos.x = (WINDOW_WIDTH / 2.0f) - (textSize.x / 2.0f) - 135;
	textPos.y += tableGap;
	renderText(renderer, text, assets.goodTimesFont, textPos, Colour{ 0, 0, 0 });
	std::string string = std::to_string((int)roundToI(value * 100)) + "%";
	renderText(renderer, string, assets.goodTimesFont, Vec2{ textPos.x + textSize.x + 60.0f, textPos.y }, Colour{ 0, 0, 0 });
	if (imgButton(renderer, assets, gameInput, assets.minusTexture, Vec2{ textPos.x + textSize.x, textPos.y + 5.0f }, Vec2{ 50, 50 }))
	{
		if (value > 0.05f)
		{
			value -= 0.1f;
		}
	}
	if (imgButton(renderer, assets, gameInput, assets.plusTexture, Vec2{ textPos.x + textSize.x + 220, textPos.y + 5.0f }, Vec2{ 50, 50 }))
	{
		if (value < 0.95f)
		{
			value += 0.1f;
		}
	}
}
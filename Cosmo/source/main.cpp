#include <fstream>

#include "vld.h"
#include "character.h"
#include "common.h"
#include "doorManager.h"
#include "level.h"
#include "loadManager.h"
#include "menu.h"
#include "objectManager.h"
#include "platformManager.h"
#include "soundManager.h"
#include "texture.h"
#include "world.h"
#include "window.h"

bool init();
bool loadMedia();
void close();

enum { EXIT, LOAD, ENTER };

Character* character;
Window gWindow;
Texture loadingSprite;
Menu* menu;

bool init()
{
	bool success = true;

	//initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}

		//create window
		if (!gWindow.init()) {
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			//create renderer for window
			gRenderer = gWindow.createRenderer();
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				//initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				} 

				//initialize SDL_mixer
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}

				//initialize SDL_ttf
				if (TTF_Init() == -1) {
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
				else {
					loadManager.initializeLevels();
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	bool success = true;

	if (!loadManager.startUp()) {
		printf("Unable to load start up settings!");
		success = false;
	}

	if (!objectManager.loadMedia()) {
		printf("Unable to load objects!\n");
		success = false;
	}

	if (!doorManager.loadMedia()) {
		printf("Unable to load doors!\n");
		success = false;
	}

	if (!soundManager.loadMedia()) {
		printf("Unable to load sounds!\n");
		success = false;
	}

	if (!platformManager.loadMedia()) {
		printf("Unable to load platforms!\n");
		success = false;
	}

	if (!loadManager.loadLevel(currentLevel, false)) {
		printf("Unable to load initial level!\n");
		success = false;
	}

	return success;
}

int main(int argc, char* args[])
{
	if (!init()) {
		printf("Failed to initialize!\n");
	}
	else {
		if (!loadMedia()) {
			printf("Failed to load media!\n");
		}
		else {
			character = new Character(0);
			character->loadMedia();

			SDL_Point charPosition = currentLevel->getCharacterStart();
			character->setPosition(charPosition.x, charPosition.y);

			menu = new Menu("resources/title.png");
			menu->loadMedia();

			int selection = menu->run();

			if (selection == 1) {
				close();
				return 1;
			}

			int resetFade = 0;
			int reset = NONE;
			
			SDL_ShowCursor(SDL_DISABLE);

			bool quit = false;

			SDL_Event e;
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

			int oldWorldIndex = currentWorldIndex;

			int fadeSpeed = 25;
			int currentFade = 0;

			int loadFade = 0;
			int loadState = NONE;

			Door* doorIndex = NULL;

			bool worldTransition = false;

			int frame = 0;

			Texture* fadeScreen = loadManager.getFadeScreen();
			fadeScreen->setScale(100);

			soundManager.playMusic(0);

			while (!quit)
			{
				if (frame >= INT_MAX) {
					frame = 0;
				}

				World* currentWorld = currentLevel->getWorld(currentWorldIndex);
			
				while (SDL_PollEvent(&e) != 0) {
					if (e.type == SDL_QUIT) {
						quit = true;
					}
					if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
						World* newWorld = NULL;
						switch (e.key.keysym.sym)
						{
						case SDLK_ESCAPE:
							selection = menu->run();
							if (selection == 1) {
								quit = true;
							}
							break;
						case SDLK_a:
							if (!unlock1) {
								break;
							}
							currentWorldIndex = 1;
							newWorld = currentLevel->getWorld(currentWorldIndex);
							if (newWorld != NULL && !character->checkForOverlap(currentWorldIndex)) {
								if (oldWorldIndex != currentWorldIndex) {
									currentWorld = newWorld;
									worldTransition = true;
									soundManager.playSound(5);
								}
							}
							else {
								soundManager.playSound(4);
								currentWorldIndex = oldWorldIndex;
							}
							break;
						case SDLK_w:
							if (!unlock2) {
								break;
							}
							currentWorldIndex = 2;
							newWorld = currentLevel->getWorld(currentWorldIndex);
							if (newWorld != NULL && !character->checkForOverlap(currentWorldIndex)) {
								if (oldWorldIndex != currentWorldIndex) {
									currentWorld = newWorld;
									worldTransition = true;
									soundManager.playSound(5);
								}
							}
							else {
								currentWorldIndex = oldWorldIndex;
								soundManager.playSound(4);
							}
							break;
						case SDLK_d:
							if (!unlock3) {
								break;
							}
							currentWorldIndex = 3;
							newWorld = currentLevel->getWorld(currentWorldIndex);
							if (newWorld != NULL && !character->checkForOverlap(currentWorldIndex)) {
								if (oldWorldIndex != currentWorldIndex) {
									currentWorld = newWorld;
									worldTransition = true;
									soundManager.playSound(5);
								}
							}
							else {
								currentWorldIndex = oldWorldIndex;
								soundManager.playSound(4);
							}
							break;
						}
					}

					character->handleEvent(e);
					gWindow.handleEvent(e);
				}
				
				int	worldIndex = objectManager.triggerWorld();
				if (worldIndex != NONE) {
					currentWorldIndex = worldIndex;
					World* newWorld = currentLevel->getWorld(currentWorldIndex);
					if (newWorld != NULL) {
						if (oldWorldIndex != currentWorldIndex) {
							currentWorld = newWorld;
							worldTransition = true;
						}
					}
					else {
						currentWorldIndex = oldWorldIndex;
					}
				}

				//handle loading transitions!
				if (loadState == NONE) {
					doorIndex = doorManager.triggerDoor();
					if (doorIndex != NULL) {
						loadState = EXIT;
					}
				}
				
				if (loadState == EXIT) {
					loadFade += 5;
					if (loadFade >= ALPHA_MAX) {
						loadFade = ALPHA_MAX;
						loadState = LOAD;
					}
					fadeScreen->setAlpha(loadFade);
				}

				if (loadState == LOAD) {
					if (loadManager.loadLevelfromDoor(doorIndex)) {
						SDL_Rect doorBox = doorIndex->getBox();
						character->setPosition(doorBox.x, doorBox.y);
						oldWorldIndex = currentWorldIndex;
					}
					loadState = ENTER;

					if (!loadManager.isPreloaded(currentLevel)) {
						loadManager.loadLevel(currentLevel, true);
					}
					//bool startLoadingScreen();
				}

				if (loadState == ENTER) {
					loadFade -= 5;
					if (loadFade <= 0) {
						loadState = NONE;
						loadFade = 0;
					}
					fadeScreen->setAlpha(loadFade);
				}

				//handle world transitions!
				if (worldTransition) {
					World* oldWorld = currentLevel->getWorld(oldWorldIndex);
					if (currentFade <= ALPHA_MAX) {
						currentWorld->setAlpha(currentFade);
						oldWorld->setAlpha(ALPHA_MAX - currentFade);
					}
					else {
						oldWorld->setAlpha(ALPHA_MAX);
						currentWorld->setAlpha(ALPHA_MAX);
						currentFade = 0;
						worldTransition = false;
						oldWorldIndex = currentWorldIndex;
					}
					currentFade += fadeSpeed;
				}
				

				//move the character
				character->move(camera);
				character->setCamera(camera);
				
				charPosition = character->getPosition();

				if (charPosition.y >= currentLevel->getDeathLine() && reset != ENTER) {
					soundManager.playSound(6);
					reset = ENTER;
				}

				currentLevel->update();

				/**************************************RENDER*********************************/
	
				//clear the screen
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				if (worldTransition) {
					World* oldWorld = currentLevel->getWorld(oldWorldIndex);
					oldWorld->render(-camera.x, -camera.y, camera);
				}

				currentWorld->render(-camera.x, -camera.y, camera);

				objectManager.render(-camera.x, -camera.y, currentLevel->getLevelNumber(), currentWorldIndex);
				doorManager.render(-camera.x, -camera.y, currentLevel->getLevelNumber(), currentWorldIndex);

				if (loadState != EXIT) {
					character->render(camera);
				}

				currentWorld->renderForeground(-camera.x, -camera.y);

				if (loadState != NONE) {
					fadeScreen->render(0, 0);
				}

				if (reset == ENTER) {
					resetFade += 35;
					if (resetFade >= ALPHA_MAX) {
						SDL_Point charPos = currentLevel->getCharacterStart();
						character->loadState();
						reset = EXIT;
						resetFade = ALPHA_MAX;
					}

					fadeScreen->setAlpha(resetFade);
					fadeScreen->render(0, 0);
				}

				if (reset == EXIT) {
					resetFade -= 5;
					if (resetFade <= 0) {
						reset = NONE;
						resetFade = 0;
					}
					fadeScreen->setAlpha(resetFade);
					fadeScreen->render(0, 0);
				}

				objectManager.renderKeyDisplay();
			
				//update screen
				SDL_RenderPresent(gRenderer);

				frame++;
			}
		}

		//free resources and close SDL
		close();
	}
	return 0;
}

/*int threadFunction(void *data)
{
	gThreadState = RUNNING;

	//Print incoming data
	printf("Running thread with value = %d\n", (int)data);
	//loadManager.loadNeighbors();
	loadManager.loadLevel(currentLevel, true);

	gThreadState = STOPPED;

	return 0;
}*/

/*void runLoadingScreen() {
	SDL_Event e;
	bool quit = false;

	int frame = 0;

	while (!quit) 
	{
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
		SDL_RenderClear(gRenderer);

		int x = SCREEN_WIDTH / 2 - (loadingSprite.getSpriteWidth() / 2);
		int y = SCREEN_HEIGHT / 2 - (loadingSprite.getSpriteHeight() / 2);

		loadingSprite.renderSprite(x, y, frame / 6 % loadingSprite.getSpriteCount(), false);
		SDL_RenderPresent(gRenderer);

		if (gThreadState == STOPPED) {
			quit = true;
		}

		frame++;
	}
}*/

void close()
{
	loadManager.freeLevels();
	currentLevel = NULL;

	loadManager.freeCharacterSprites();
	loadManager.freeSlopes();
	delete character;

	menu->free();
	delete menu;

	doorManager.free();
	objectManager.free();
	soundManager.free();
	platformManager.free();

	SDL_DestroyRenderer(gRenderer);

	gWindow.free();
	gRenderer = NULL;

	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

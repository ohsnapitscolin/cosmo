/*This source code copyrighted by Lazy Foo' Productions (2004-2014)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, strings, and file streams
#include <SDL.h>
//#include <vld.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include "texture.h"
#include "window.h"
#include "common.h"
#include "character.h"
#include "tileset.h"
#include "timer.h"
#include "level.h"
#include "world.h"
#include "menu.h"
#include "loadManager.h"
#include "objectManager.h"
#include "doorManager.h"
#include "soundManager.h"
#include <time.h>
#include <SDL_thread.h>

const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

enum threadState { RUNNING, STOPPED };

//Starts up SDL and creates window
bool init();

void runLoadingScreen();

//Our test thread function
int threadFunction(void* data);
int gThreadState;

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

enum { EXIT, LOAD, ENTER };
LWindow gWindow;
Character* character;

LTexture loadingSprite;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		if (!gWindow.init())
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = gWindow.createRenderer();
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				} 

				//Initialize SDL_mixer
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}

				if (TTF_Init() == -1)
				{
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
	//Loading success flag
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

	if (!loadingSprite.loadFromFile("resources/loading.png")) {
		success = false;
	}
	else {
		loadingSprite.clipSprite(96, 96);
	}

	if (!loadManager.loadLevel(currentLevel, false)) {
		success = false;
	}

	return success;
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{

			character = new Character(0);
			character->loadMedia();

			SDL_Point charStart = currentLevel->getCharacterStart();
			character->setPosition(charStart.x, charStart.y);

			Menu* menu = new Menu("resources/screen3.png");
			menu->loadMedia();
			int next = menu->start();

			gThreadState = STOPPED;

			if (next == 2) {
				close();
				return 1;
			}

			if (next == 1) {
				SDL_Rect charBox = loadManager.load();
				character->setPosition(charBox.x, charBox.y);
			}

			int resetFade = 0;
			int reset = NONE;
			//loadManager.loadNeighbors();

			SDL_ShowCursor(SDL_DISABLE);

			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//Level camera
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

			//The frames per second timer
			LTimer fpsTimer;

			//The frames per second cap timer
			LTimer capTimer;

			int oldWorldIndex = currentWorldIndex;

			//The background scrolling offset
			int scrollingOffset = 0;
			int fadeSpeed = 25;
			int currentFade = 0;

			int loadFade = 0;
			int loadState = NONE;

			Door* doorIndex = NULL;

			bool view2 = false;
			bool worldTransition = false;

			int frame = 0;

			LTexture* fadeScreen = loadManager.getFadeScreen();
			fadeScreen->setScale(100);

			int data = 0;

			soundManager.playMusic(0);

			//While application is running
			while (!quit)
			{
				if (frame >= INT_MAX) {
					frame = 0;
				}

				clock_t start = clock();
				World* currentWorld = currentLevel->getWorld(currentWorldIndex);
				//Start cap timer
				capTimer.start();
				//Handle events on queue
		
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
					{
						//Adjust the 
						World* newWorld;
						switch (e.key.keysym.sym)
						{
						case SDLK_ESCAPE:
							next = menu->start();
							if (next == 2) {
								quit = true;
							}
							if (next == 1) {
								ofstream save("resources/save.txt", std::ios::out | std::ios::trunc);
								save << currentLevel->getLevelNumber();
								save << ' ';
								save << currentWorldIndex;
								save << ' ';
								save << character->getBox().x;
								save << ' ';
								save << character->getBox().y;
								save << '\n';
							}
							break;
						case SDLK_1:
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
						case SDLK_2:
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
						case SDLK_3:
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

					//Handle input for the character
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
					//currentWorld->setAlpha(ALPHA_MAX - loadFade);*/
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
						SDL_Thread* threadID = SDL_CreateThread(threadFunction, "LoadingThread", (void*)data);
						runLoadingScreen();
						SDL_WaitThread(threadID, NULL);
					}
					//bool startLoadingScreen();
				}

				if (loadState == ENTER) {
					loadFade -= 5;
					if (loadFade <= 0) {
						loadState = NONE;
						loadFade = 0;
					}
					//currentWorld->setAlpha(ALPHA_MAX - loadFade);
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
				if (currentLevel->getFixedX()) {
					camera.x = currentLevel->getWidth() / 2 - camera.w / 2;
				}

				if (currentLevel->getFixedY()) {
					camera.y = currentLevel->getHeight() / 2 - camera.h / 2;
				}

				SDL_Point charPos = character->getPosition();

				if (charPos.y >= currentLevel->getDeathLine() && reset != ENTER) {
					soundManager.playSound(6);
					reset = ENTER;
				}

				currentLevel->update();

				/**************************************RENDER*********************************/
				clock_t render = clock();
				//clear the screen

				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				if (worldTransition) {
					World* oldWorld = currentLevel->getWorld(oldWorldIndex);
					oldWorld->render(-camera.x, -camera.y, camera);
				}

				clock_t render_new = clock();
				currentWorld->render(-camera.x, -camera.y, camera);
				render_new = clock() - render_new;
				float render_new_f = ((float)render_new) / CLOCKS_PER_SEC;

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
						//character->setPosition(charPos.x, charPos.y);
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
				//Update screen
				SDL_RenderPresent(gRenderer);

				render = clock() - render;
				float render_f = ((float)render) / CLOCKS_PER_SEC;


				/*int frameTicks = capTimer.getTicks();
				if (frameTicks < SCREEN_TICKS_PER_FRAME) {
					//Wait remaining time
					SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
				}*/

				start = clock() - start;
				float start_f = ((float)start) / CLOCKS_PER_SEC;
				/*if (start_f > .1) {
					printf("%d %f seconds\nrender %f\nrender new %f\n%d\n", frame, start_f, render_f, render_new_f, worldTransition);
				}*/

				frame++;
			}
		}

		//Free resources and close SDL
		close();
	}

	return 0;
}

int threadFunction(void *data)
{
	gThreadState = RUNNING;

	//Print incoming data
	printf("Running thread with value = %d\n", (int)data);
	//loadManager.loadNeighbors();
	loadManager.loadLevel(currentLevel, true);

	gThreadState = STOPPED;

	return 0;
}

void runLoadingScreen() {
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
}

void close()
{
	loadManager.freeLevels();
	currentLevel = NULL;

	loadManager.freeCharacterSprites();
	delete character;

	doorManager.free();
	soundManager.free();
	objectManager.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);

	gWindow.free();
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

#include "loadManager.h"
#include "common.h"
#include "level.h"
#include "character.h"
#include "slope.h"
#include "door.h"
#include <vector>
#include <fstream>
#include "menu.h"
#include "time.h"
#include "doorManager.h"
#include "objectManager.h"

const int NUM_STORED_LEVELS = 5;
const int NUM_SLOPES = 28;
const int SLOPE_OFFSET = 2;
Slope* mSlopes[NUM_SLOPES];


LoadManager::LoadManager() {
	mCharacterSprites.resize(2);
	mStoredLevels.resize(NUM_STORED_LEVELS);
	mStoredLevelIndex = 0;
}

bool LoadManager::loadCharacterSprites(int type)
{
	//Loading success flag
	bool spritesLoaded = true;
	std::string charFolder = "resources/characters/";

	std::string charName = "cosmo";
	
	//*********** TEST ***********//
	if (type == 1) {
		charName = "omsoc";
	}

	std::fstream map(charFolder + charName + "/header.txt");

	if (!map) {
		printf("Could not load character %s\n!", charName.c_str());
	}

	int totalAnimations;

	map >> totalAnimations;
	mCharacterSprites[type].resize(totalAnimations);

	for (int i = 0; i < totalAnimations; i++) {
		std::string filename;
		int width, height;

		map >> filename;
		map >> width;
		map >> height;

		if (map.fail())
		{
			printf("Error loading map: Unexpected end of file!\n");
			spritesLoaded = false;
			break;
		}

		LTexture* currentTexture = new LTexture();
		if (!currentTexture->loadFromFile(charFolder + charName + "/" + filename))
		{
			printf("Failed to load texture: %s!\n", charName + "/" + filename);
			spritesLoaded = false;
			break;
		}
		currentTexture->clipSprite(width, height);
		mCharacterSprites[type][i] = currentTexture;
	}

	map.close();

	return spritesLoaded;
}

vector<LTexture*> LoadManager::getSprite(int type) {
	return mCharacterSprites[type];
}


bool LoadManager::startUp() 
{
	bool success = true;

	/*if (!splashScreen()) {
		printf("Unable to load splash screen!\n");
		success = false;
	};*/

	success &= loadFadeTexture();
	success &= loadLoadingSprites();

	gFont = TTF_OpenFont("resources/VT323.ttf", 58);
	if (gFont == NULL) {
		printf("Font could not be loaded!\n");
		success = false;
	}

	if (!loadSlopes()) {
		printf("Unable to load slopes!\n");
		success = false;
	}

	if (!loadCharacters()) {
		printf("Unable to load charcaters!\n");
		success = false;
	}

	return success;
}

bool LoadManager::splashScreen() 
{
	SDL_ShowCursor(SDL_DISABLE);
	bool success = true;

	LTexture* screen1 = new LTexture();
	LTexture* screen2 = new LTexture();
	LTexture* screen3 = new LTexture();

	if (!screen1->loadFromFile("resources/screen1.png"))
	{
		printf("Failed to load texture!\n");
		success = false;
	}

	if (!screen2->loadFromFile("resources/screen2.png"))
	{
		printf("Failed to load texture!\n");
		success = false;
	}

	if (!screen3->loadFromFile("resources/screen3.png"))
	{
		printf("Failed to load texture!\n");
		success = false;
	}

	if (!success) {
		return false;
	}

	bool quit = false;

	int fadeSpeed = 2;
	int displayTime = 1;
	int current = 1;
	int frame = 0;
	
	bool enter = false;
	bool display = false;
	bool exit = false;

	bool pause = false;

	LTexture* screen = screen1;
	SDL_Event e;

	int screenX = SCREEN_WIDTH / 2 - (screen->getWidth() / 2);
	int screenY = SCREEN_HEIGHT / 2 - (screen->getHeight() / 2);

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			if (e.type == SDL_WINDOWEVENT) {
				switch (e.window.event) {
				case SDL_WINDOWEVENT_ENTER:
					pause = false;
					break;
				case SDL_WINDOWEVENT_LEAVE:
					pause = true;
					break;
				}
			}
		}

		if (pause) {
			continue;
		}

		if(frame >= 60 && !enter && !display && !exit) {
			enter = true;
			frame = 0;
		}

		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		if (enter || display || exit) {
			screen->render(screenX, screenY);
		}

		if (enter) {
			mFadeScreen.setAlpha(ALPHA_MAX - fadeSpeed * frame);
			if (frame >= ALPHA_MAX / fadeSpeed) {
				frame = 0;
				enter = false;
				display = true;
			}
		}

		if (display) {
			if (frame >= displayTime * 60) {
				exit = true;
				display = false;
				frame = 0;
			}
		}

		if (exit) {
			if (current == 3) {
				quit = true;
			}
			mFadeScreen.setAlpha(fadeSpeed * frame);
			if (frame >= ALPHA_MAX / fadeSpeed) {
				frame = 0;
				enter = true;
				exit = false;
				current++;
				if (current == 2) {
					screen = screen2;
				}
				if (current == 3) {
					screen = screen3;
				}
				if (current > 3) {
					quit = true;
				}
				int screenX = SCREEN_WIDTH / 2 - (screen->getWidth() / 2);
				int screenY = SCREEN_HEIGHT / 2 - (screen->getHeight() / 2);
			}
		}

		if (!display) {
			mFadeScreen.render(0, 0);
		}

		SDL_RenderPresent(gRenderer);
		frame++;
	}

	screen1->free();
	delete screen1;
	screen1 = NULL;

	screen2->free();
	delete screen2;
	screen2 = NULL;

	screen3->free();
	delete screen3;
	screen3 = NULL;

	mFadeScreen.setAlpha(ALPHA_MAX);

	return success;
}

bool LoadManager::initializeLevels() {
	mLevels.push_back(new Level(0));
	mLevels.push_back(new Level(1));
	mLevels.push_back(new Level(2));
	mLevels.push_back(new Level(3));
	mLevels.push_back(new Level(4));
	//mLevels.push_back(new Level(5));
	currentLevel = mLevels[0];
	return true;
}

SDL_Rect LoadManager::load() {
	ifstream load("resources/save.txt");
	int levelIndex, currentWorld, x, y;
	load >> levelIndex;
	load >> currentWorld;
	load >> x;
	load >> y;

	currentLevel = mLevels[levelIndex];
	currentWorldIndex = currentWorld;

	loadLevel(currentLevel, false);

	SDL_Rect charBox;
	charBox.x = x;
	charBox.y = y;
	return charBox;
}

bool LoadManager::loadLevelfromDoor(Door* door) 
{
	bool success = true;
	int levelIndex = door->getLevelIndex();

	if (levelIndex < 0 || levelIndex >= int(mLevels.size())) {
		printf("Level index %d does not exist!\n", levelIndex);
		success = false;
	}
	else { 
		currentLevel = mLevels[levelIndex];
		currentWorldIndex = door->getWorldIndex();
	}

	return success;
	//Level* newLevel = mLevels[levelIndex];
	/*
	if (loadLevel(newLevel, currentLevel)) {
		currentLevel = newLevel;
		currentWorldIndex = newDoor->getWorldIndex();
		return newDoor->getBox();
	}
	else {
		return oldDoor->getBox();
	}
	*/
}

/*SDL_Rect LoadManager::loadLevelfromDoor(int doorIndex) {
	Door* oldDoor = mDoors[doorIndex];
	Door* newDoor = mDoors[oldDoor->getDestination()];

	int levelIndex = newDoor->getLevelIndex();
	
	if (levelIndex < 0 || levelIndex >= int(mLevels.size())) {
		return oldDoor->getBox();
	}

	Level* newLevel = mLevels[levelIndex];
	
	if (loadLevel(newLevel, currentLevel)) {
		currentLevel = newLevel;
		currentWorldIndex = newDoor->getWorldIndex();
		return newDoor->getBox();
	}
	else {
		return oldDoor->getBox();
	}
}*/

bool LoadManager::loadLevel(Level* level, bool unload) 
{
	bool success = true;
	
	if (!level->loadMedia(false)) {
		printf("Unable to load level!\n");
		success = false;
	}
	else {
		storeLevel(level);
		if (unload) {
			//prerenderLevel(level);
			unloadLevel(level);
		}
	}

	return success;
}

bool LoadManager::loadSlopes() 
{
	mSlopes[0] = new Slope(15, 0, 2, DOWN);
	mSlopes[1] = new Slope(0, 15, 3, DOWN);
	mSlopes[2] = new Slope(0, 15, 4, UP);
	mSlopes[3] = new Slope(15, 0, 5, UP);
	mSlopes[4] = new Slope(0, 7, 6, UP);
	mSlopes[5] = new Slope(8, 15, 7, UP);
	mSlopes[6] = new Slope(0, 3, 8, UP);
	mSlopes[7] = new Slope(4, 7, 9, UP);
	mSlopes[8] = new Slope(8, 11, 10, UP);
	mSlopes[9] = new Slope(12, 15, 11, UP);
	mSlopes[10] = new Slope(15, 8, 12, DOWN);
	mSlopes[11] = new Slope(7, 0, 13, DOWN);
	mSlopes[12] = new Slope(15, 12, 14, DOWN);
	mSlopes[13] = new Slope(11, 8, 15, DOWN);
	mSlopes[14] = new Slope(7, 4, 16, DOWN);
	mSlopes[15] = new Slope(3, 0, 17, DOWN);
	mSlopes[16] = new Slope(15, 8, 18, UP);
	mSlopes[17] = new Slope(7, 0, 19, UP);
	mSlopes[18] = new Slope(15, 12, 20, UP);
	mSlopes[19] = new Slope(11, 8, 21, UP);
	mSlopes[20] = new Slope(7, 4, 22, UP);
	mSlopes[21] = new Slope(3, 0, 23, UP);
	mSlopes[22] = new Slope(0, 7, 24, DOWN);
	mSlopes[23] = new Slope(8, 15, 25, DOWN);
	mSlopes[24] = new Slope(0, 3, 26, DOWN);
	mSlopes[25] = new Slope(4, 7, 27, DOWN);
	mSlopes[26] = new Slope(8, 11, 28, DOWN);
	mSlopes[27] = new Slope(12, 15, 29, DOWN);

	return true;
}

Slope* LoadManager::getCurrentSlope(int type) 
{
	type -= SLOPE_OFFSET;
	if (type < 0 || type >= NUM_SLOPES) {
		return NULL;
	}
	return mSlopes[type];
}

void LoadManager::freeLevels() 
{
	for (int i = 0; i < int(mLevels.size()); i++) {
		if (mLevels[i] != NULL) {
			mLevels[i]->free();
			delete mLevels[i];
			mLevels[i] = NULL;
		}
	}
}

void LoadManager::freeCharacterSprites() 
{
	for (int i = 0; i < int(mCharacterSprites.size()); i++) {
		if (!mCharacterSprites[i].empty()) {
			for (int j = 0; j < int(mCharacterSprites[i].size()); j++) {
				if (mCharacterSprites[i][j] != NULL) {
					mCharacterSprites[i][j]->free();
					delete mCharacterSprites[i][j];
					mCharacterSprites[i][j] = NULL;
				}
			}
		}
	}
}


bool LoadManager::loadCharacters() {
	return loadCharacterSprites(0);
}

void LoadManager::storeLevel(Level* level) 
{
	bool preloaded = false;
	int freeIndex = -1;
	
	for (int i = 0; i < int(mStoredLevels.size()); i++) {
		if (mStoredLevels[i] == NULL && freeIndex < 0) {
			freeIndex = i;
		}
		if (level == mStoredLevels[i]) {
			printf("Level %d is already loaded!\n", level->getLevelNumber());
			preloaded = true;
		}
	}

	if (!preloaded) {
		mStoredLevels[freeIndex] = level;
	}
}


void LoadManager::prerenderLevel(Level* level) 
{
	printf("Prerendering level %d!\n", level->getLevelNumber());

	bool quit = false;
	SDL_Event e;

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		mFadeScreen.render(0, 0);

		SDL_Rect sample;
		sample.x = 0;
		sample.y = 0;
		sample.w = 0;
		sample.h = 0;

		for (int i = 1; i <= 3; i++) {
			if (i == currentWorldIndex) {
				continue;
			}
			World* currentWorld = level->getWorld(i);
			if (currentWorld != NULL) {
				currentWorld->render(0, 0, sample);
			}

		}
		quit = true;
	}
}


void LoadManager::loadScreen(Level* level, int screen, bool render) {

	bool quit = false;
	SDL_Event e;

	render = false;

	int alpha = 0;
	int totalSprites = mLoadingTexture.getSpriteCount();

	while (!quit) 
	{
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		mFadeScreen.render(0, 0);

		for (int i = 0; i < screen; i++) {
			int x = SCREEN_WIDTH * (i + 1) / 4 - mLoadingTexture.getSpriteWidth() / 2;
			mLoadingTexture.renderSprite(0, 500, i, false);
		}
			
		SDL_RenderPresent(gRenderer);
			
		alpha += 50;

		if (alpha >= ALPHA_MAX) {
			quit = true;
		}
	}
}

LTexture* LoadManager::getFadeScreen() {
	return &mFadeScreen;
}

//unload a stored, non neighboring level
void LoadManager::unloadLevel(Level* level) 
{
	int levelNumber = level->getLevelNumber();
	vector<int> neighbors;
	neighbors = doorManager.getNeighbors(level->getLevelNumber());
	neighbors.push_back(levelNumber);

	bool unloaded = false;
	int emptySlots = 0;
	for (int i = 0; i < int(mStoredLevels.size()); i++) {
		if (mStoredLevels[i] == NULL) {
			emptySlots++;
			continue;
		}
		bool isNeighbor = false;
		for (int j = 0; j < int(neighbors.size()); j++) {
			if (mStoredLevels[i]->getLevelNumber() == neighbors[j]) {
				isNeighbor = true;
			}
		}
		if (!isNeighbor && !unloaded) {
			mStoredLevels[i]->unload();
			mStoredLevels[i] = NULL;
			unloaded = true;
		}
	}

	if (emptySlots == 0 && !unloaded) {
		int unloadLevel = rand() % int(mStoredLevels.size());
		while (mStoredLevels[unloadLevel] == level) {
			unloadLevel = rand() % int(mStoredLevels.size());
		}
		mStoredLevels[unloadLevel]->unload();
		mStoredLevels[unloadLevel] = NULL;
	}
}

/*
//to be run in a separate thread
void LoadManager::loadNeighbors() 
{
	int levelNumber = currentLevel->getLevelNumber();
	vector<int> neighbors;
	neighbors.push_back(levelNumber);

	//find neighboring levels using doors
	for (int i = 0; i < int(mDoors.size()); i++) {
		Door* currentDoor = mDoors[i];
		if (currentDoor->getLevelIndex() == levelNumber) {
			neighbors.push_back(mDoors[currentDoor->getDestination()]->getLevelIndex());
		}
	}

	bool unload = false;
	//scroll through levels, check if they exist in neighbor array
	for (int i = 0; i < int(mLevels.size()); i++) {
		if (std::find(neighbors.begin(), neighbors.end(), i) != neighbors.end()) {
			if (mLevels[i] == currentLevel) {
				continue;
			}
			else {
				loadLevel(mLevels[i], false);
			}
		}
		//if we want multithreaded unloading, unload the first loaded level
		/*else if (!unload) {
			if (mLevels[i]->isLoaded()) {
				mLevels[i]->unload();
				unload = true;
			}
		}*/
	//}
//}*/

bool LoadManager::loadLoadingSprites() 
{
	bool success = true;
	if (!mLoadingTexture.loadFromFile("resources/load/sprites.png")) {
		printf("Unable to load loading sprites!\n");
		success = false;
	}
	else {
		mLoadingTexture.clipSprite(100, 100);
	}
	return success;
}

bool LoadManager::loadFadeTexture() 
{
	bool success = true;
	
	if (!mFadeScreen.loadFromFile("resources/load/fade.png")) {
		printf("Unable to load fade texture!\n");
		success = false;
	}
	else {
		mFadeScreen.setScale(SCREEN_WIDTH / mFadeScreen.getWidth());
	}

	return success;
}

bool LoadManager::isPreloaded(Level* level) 
{
	for (int i = 0; i < int(mStoredLevels.size()); i++) {
		if (mStoredLevels[i] == level) {
			return true;
		}
	}
	return false;
}

void LoadManager::freeSlopes() {
	for (int i = 0; i < NUM_SLOPES; i++) {
		if (mSlopes[i] != NULL) {
			delete mSlopes[i];
			mSlopes[i] = NULL;
		}
	}
}
#include "level.h"
#include "world.h"
#include "loadManager.h"
#include <fstream>
#include <algorithm>

Level::Level(int levelNumber) 
{
	mLevelNumber = levelNumber;
	std::string folder = "resources/levels/" + std::to_string(mLevelNumber) + "/";

	std::ifstream map(folder + "header.txt");
	mWorlds[0] = mWorlds[1] = mWorlds[2] = true;
	
	fixedX = false;
	fixedY = false;

	mCharStart.x = 0;
	mCharStart.y = 0;

	mDeathLine = 0;

	if (map) {
		map >> mStartWorld;

		map >> mCharStart.x;
		map >> mCharStart.y;

		int world1, world2, world3;
		map >> world1;
		map >> world2;
		map >> world3;
		mWorlds[0] = (world1 != 0);
		mWorlds[1] = (world2 != 0);
		mWorlds[2] = (world3 != 0);

		map >> fixedX;
		map >> fixedY;

		map >> mDeathLine;

		if (map.fail())
		{
			printf("Invalid header file for level %d!\n", mLevelNumber);
		}
		map.close();
	}

	mWorld1 = mWorlds[0] ? new World(1, folder) : NULL;
	mWorld2 = mWorlds[1] ? new World(2, folder) : NULL;
	mWorld3 = mWorlds[2] ? new World(3, folder) : NULL;

	mLoadAttempt = false;
	mLoadState = false;
}

Level::~Level() {
	void free();
}

bool Level::getFixedX() {
	return fixedX;
}

bool Level::getFixedY() {
	return fixedY;
}

void Level::update() {
	if (mWorld1 != NULL) {
		mWorld1->update();
	}
	if (mWorld2 != NULL) {
		mWorld2->update();
	}
	if (mWorld3 != NULL) {
		mWorld3->update();
	}
}

bool Level::loadMedia(bool renderLoad) {

	if (mLoadAttempt) {
		return mLoadState;
	}

	bool success = true;

	if (renderLoad) {
		loadManager.loadScreen(this, 1, mWorld1 != NULL);
	}
	if (mWorld1 != NULL) {
		if (!mWorld1->loadMedia()) {
			mWorld1 = NULL;
			mWorlds[0] = false;
			printf("Unable to load world 1 for level %d!\n", mLevelNumber);
		}
	}
	
	if (renderLoad) {
		loadManager.loadScreen(this, 2, mWorld2 != NULL);
	}
	if (mWorld2 != NULL) {
		if (!mWorld2->loadMedia()) {
			mWorld2 = NULL;
			mWorlds[1] = false;
			printf("Unable to load world 2 for level %d!\n", mLevelNumber);
		}
	}
	
	if (renderLoad) {
		loadManager.loadScreen(this, 3, mWorld3 != NULL);
	}
	if (mWorld3 != NULL) {
		if (!mWorld3->loadMedia()) {
			mWorld3 = NULL;
			mWorlds[2] = false;
			printf("Unable to load world 3 for level %d!\n", mLevelNumber);
		}
	}

	if (!mWorlds[0] || !mWorlds[1] || !mWorlds[2]) {
		if (mWorlds[0] && mWorlds[1]) {
			int w1 = mWorld1->getWidth();
			int w2 = mWorld2->getWidth();
			int h1 = mWorld1->getHeight();
			int h2 = mWorld2->getHeight();

			if (w1 == w2 && h1 == h2) {
				mWidth = mWorld1->getWidth();
				mHeight = mWorld1->getHeight();
			}
			else {
				printf("Unable to load level %d: Worlds are different sizes!\n", mLevelNumber);
				success = false;
			}
		}
		else if (mWorlds[0] && mWorlds[2]) {
			int w1 = mWorld1->getWidth();
			int w3 = mWorld3->getWidth();
			int h1 = mWorld1->getHeight();
			int h3 = mWorld3->getHeight();

			if (w1 == w3 && h1 == h3) {
				mWidth = mWorld1->getWidth();
				mHeight = mWorld1->getHeight();
			}
			else {
				printf("Unable to load level %d: Worlds are different sizes!\n", mLevelNumber);
				success = false;
			}
		}
		else if (mWorlds[1] && mWorlds[2]) {
			int w2 = mWorld2->getWidth();
			int w3 = mWorld3->getWidth();
			int h2 = mWorld2->getHeight();
			int h3 = mWorld3->getHeight();

			if (w2 == w3 && h2 == h3) {
				mWidth = mWorld2->getWidth();
				mHeight = mWorld2->getHeight();
			}
			else {
				printf("Unable to load level %d: Worlds are different sizes!\n", mLevelNumber);
				success = false;
			}
		}
		else if (mWorlds[0]) {
			mWidth = mWorld1->getWidth();
			mHeight = mWorld1->getHeight();
		}
		else if (mWorlds[1]) {
			mWidth = mWorld2->getWidth();
			mHeight = mWorld2->getHeight();
		}
		else if (mWorlds[2]) {
			mWidth = mWorld3->getWidth();
			mHeight = mWorld3->getHeight();
		}
	}
	else
	{
		int w1 = mWorld1->getWidth();
		int w2 = mWorld2->getWidth();
		int w3 = mWorld3->getWidth();
		int h1 = mWorld1->getHeight();
		int h2 = mWorld2->getHeight();
		int h3 = mWorld3->getHeight();

		if (w1 == w2 && w2 == w3 && h1 == h2 && h2 == h3) {
			mWidth = mWorld1->getWidth();
			mHeight = mWorld1->getHeight();
		}
		else {
			printf("Unable to load level %d: Worlds are different sizes!\n", mLevelNumber);
			success = false;
		}
	}

	if (mDeathLine == 0) {
		mDeathLine = mHeight;
	}

	mLoadAttempt = true;
	mLoadState = success;

	printf("LEVEL %d LOADED!\n", mLevelNumber);
	return success;
}

int Level::getLevelNumber() {
	return mLevelNumber;
}

int Level::getWidth() {
	return mWidth;
}

int Level::getHeight() {
	return mHeight;
}

World* Level::getWorld(int worldIndex) 
{
	switch (worldIndex) {
	case 1:
		return mWorld1;
		break;
	case 2:
		return mWorld2;
		break;
	case 3:
		return mWorld3;
		break;
	}
	return NULL;
}

SDL_Point Level::getCharacterStart() {
	return mCharStart;
}

void Level::unload() {

	if (!mLoadAttempt) {
		return;
	}

	mLoadAttempt = false;
	mLoadState = false;

	printf("Unloading level %d!\n", mLevelNumber);

	//loadManager.loadScreen(this, 1, false);
	if (mWorld1 != NULL) {
		mWorld1->unload();
	}

	//loadManager.loadScreen(this, 2, false);
	if (mWorld2 != NULL) {
		mWorld2->unload();
	}
	
	//loadManager.loadScreen(this, 3, false);
	if (mWorld3 != NULL) {
		mWorld3->unload();
	}
	
	printf("Unloaded successfully!\n");
}
void Level::free() 
{
	if (mWorld1 != NULL) {
		mWorld1->free();
		delete mWorld1;
		mWorld1 = NULL;
	}
	if (mWorld2 != NULL) {
		mWorld2->free();
		delete mWorld2;
		mWorld2 = NULL;
	}
	if (mWorld3 != NULL) {
		mWorld3->free();
		delete mWorld3;
		mWorld3 = NULL;
	}
}

bool Level::isLoaded() {
	return mLoadAttempt;
}

int Level::getDeathLine() {
	return mDeathLine;
}
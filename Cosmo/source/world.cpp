#include "world.h"
#include "level.h"
#include "objectManager.h"
#include "doorManager.h"
#include <fstream>
#include <algorithm>

World::World(int index, std::string levelFolder)
{
	mWorld = levelFolder + "world" + std::to_string(index) + "/";
	
	mTileSet = new TileSet(mWorld + "tileset.txt");
	mPlatformSet = new PlatformSet(mWorld + "platforms.txt");

	mGridTexture = new LTexture();
}

World::~World() 
{
	free();
}

bool World::loadMedia() 
{
	bool success = true;

	ifstream map(mWorld + "header.txt");

	if (!map) {
		return false;
	}

	map >> mWidth;
	map >> mHeight;

	if (!mTileSet->loadMedia(mWidth, mHeight)) {
		printf("Unable to load tilset for: %s!\n", mWorld.c_str());
		success = false;
	}

	if (!mPlatformSet->loadPlatforms()) {
		printf("Unable to load platforms for: %s\n", mWorld.c_str());
		success = false;
	}

	string gridTextureName;
	map >> gridTextureName;

	int offsetX, offsetY;
	map >> offsetX;
	map >> offsetY;
	mGridTexture->setOffset(offsetX, offsetY);

	if (!mGridTexture->loadFromFile(mWorld + gridTextureName))
	{
		printf("Failed to load texture: %s!\n", gridTextureName.c_str());
		success = false;
	}

	int offset = 0;
	for (int i = 0; i < 2; i++) {
		bool loadBackground;
		map >> loadBackground;
		if (loadBackground) {
			mBackgrounds.push_back(new LTexture());

			string backgroundName;
			map >> backgroundName;

			int scale;
			map >> offsetX;
			map >> offsetY;
			map >> scale;
			mBackgrounds[i - offset]->setOffset(offsetX, offsetY);
			mBackgrounds[i - offset]->setScale(scale);

			if (!mBackgrounds[i - offset]->loadFromFile(mWorld + backgroundName))
			{
				printf("Failed to load texture: %s\n", (mWorld + backgroundName).c_str());
				success = false;
				break;
			}
		}
		else {
			offset++;
		}
	}

	for (int i = 0; i < 1; i++) {
		bool loadForeground;
		map >> loadForeground;
		if (loadForeground) {
			mForegrounds.push_back(new LTexture());

			string foregroundName;
			map >> foregroundName;

			int scale;
			map >> offsetX;
			map >> offsetY;
			map >> scale;
			mForegrounds[i]->setOffset(offsetX, offsetY);
			mForegrounds[i]->setScale(scale);
			if (!mForegrounds[i]->loadFromFile(mWorld + foregroundName))
			{
				printf("Failed to load texture: %s\n", foregroundName.c_str());
				success = false;
				break;
			}
		}
	}

	map.close();

	return success;
}


void World::setAlpha(int alpha) 
{
	mGridTexture->setAlpha(alpha);
	for (int i = 0; i < int(mBackgrounds.size()); i++) {
		mBackgrounds[i]->setAlpha(alpha);
	}
	for (int i = 0; i < int(mForegrounds.size()); i++) {
		mForegrounds[i]->setAlpha(alpha);
	}
	mPlatformSet->setAlpha(alpha);
}


void World::render(int x, int y, SDL_Rect& camera) 
{
	for (int i = 0; i < int(mBackgrounds.size()); i++) {
		mBackgrounds[i]->render(x * (1.0 - (1.0 / (i + 2))), y);
	}
	mGridTexture->render(x, y);
	mPlatformSet->render(camera);
}

void World::renderForeground(int x, int y) 
{
	for (int i = 0; i < int(mForegrounds.size()); i++) {
		mForegrounds[i]->render(x, y);
	}
}

void World::update() {
	mPlatformSet->update();
}

int World::getWidth() {
	return mWidth;
}

int World::getHeight() {
	return mHeight;
}

PlatformSet* World::getPlatformSet() {
	return mPlatformSet;
}

TileSet* World::getTileSet() {
    return mTileSet;
}

void World::unload() 
{
	if (mTileSet != NULL) {
		mTileSet->free();
	}
	if (mPlatformSet != NULL) {
		mPlatformSet->free();
	}
	if (mGridTexture != NULL) {
		mGridTexture->free();
	}
	for (int i = 0; i < int(mBackgrounds.size()); i++) {
		if (mBackgrounds[i] != NULL) {
			mBackgrounds[i]->free();
		}
	}
	for (int i = 0; i < int(mForegrounds.size()); i++) {
		if (mForegrounds[i] != NULL) {
			mForegrounds[i]->free();
		}
	}
}

void World::free()
{
	if (mTileSet != NULL) {
		mTileSet->free();
		delete mTileSet;
		mTileSet = NULL;
	}
	if (mPlatformSet != NULL) {
		mPlatformSet->free();
		delete mPlatformSet;
		mPlatformSet = NULL;
	}
	if (mGridTexture != NULL) {
		mGridTexture->free();
		delete mGridTexture;
		mGridTexture = NULL;
	}
	for (int i = 0; i < int(mBackgrounds.size()); i++) {
		if (mBackgrounds[i] != NULL) {
			mBackgrounds[i]->free();
			delete mBackgrounds[i];
			mBackgrounds[i] = NULL;
		}
	}
	for (int i = 0; i < int(mForegrounds.size()); i++) {
		if (mForegrounds[i] != NULL) {
			mForegrounds[i]->free();
			delete mForegrounds[i];
			mForegrounds[i] = NULL;
		}
	}
}

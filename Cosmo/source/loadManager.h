#ifndef LOAD_MANAGER_H_DEFINED
#define LOAD_MANAGER_H_DEFINED

#include "common.h"

#include "level.h"
#include "door.h"
#include "slope.h"
#include "tileset.h"

class LoadManager 
{
public:
	LoadManager();

	vector<Texture*> getSprite(int type);

	bool startUp();

	bool isLocked(int index);
	bool initializeLevels();

	bool isPreloaded(Level* level);
	bool splashScreen();

	void unloadLevel(Level* level);

	void freeLevels();
	void freeCharacterSprites();

	void prerenderLevel(Level* level);
	
	SDL_Rect load();

	bool loadLevel(Level* level, bool unload);
	void loadScreen(Level* level, int screen, bool render);
	bool loadLevelfromDoor(Door* door);
	
	Texture* getFadeScreen();

	void loadNeighbors();
	void freeSlopes();

	Slope* getCurrentSlope(int type);

private:
	bool loadSlopes();
	bool loadDoors();

	bool loadLoadingSprites();
	bool loadFadeTexture();

	bool loadCharacters();
	bool loadCharacterSprites(int type);

	bool loadSprite(string filename, int index, int width, int height, int type);
	vector<vector<Texture*>> mCharacterSprites;

	vector<Level*> mStoredLevels;
	int mStoredLevelIndex;
	void storeLevel(Level* level);

	vector<Door*> mDoors;
	vector<Level*> mLevels;

	Texture mFadeScreen;
	Texture mLoadingTexture;
};
#endif
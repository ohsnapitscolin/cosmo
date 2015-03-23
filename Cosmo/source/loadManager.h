#ifndef LOAD_MANAGER_H_DEFINED
#define LOAD_MANAGER_H_DEFINED

#include <SDL.h>
#include <SDL_ttf.h>
#include "level.h"
#include "common.h"
#include "door.h"
#include "tileset.h"

class LoadManager 
{
public:

	LoadManager();

	vector<LTexture*> getSprite(int type);

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
	
	LTexture* getFadeScreen();

	void loadNeighbors();

	Slope* getCurrentSlope(int type);


private:
	bool loadSlopes();
	bool loadDoors();

	bool loadLoadingSprites();
	bool loadFadeTexture();

	bool loadCharacters();
	bool loadCharacterSprites(int type);

	bool loadSprite(string filename, int index, int width, int height, int type);
	vector<vector<LTexture*>> mCharacterSprites;

	vector<Level*> mStoredLevels;
	int mStoredLevelIndex;
	void storeLevel(Level* level);

	vector<Door*> mDoors;
	vector<Level*> mLevels;

	LTexture mFadeScreen;
	LTexture mLoadingTexture;
};
#endif
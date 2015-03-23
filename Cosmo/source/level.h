#ifndef LEVEL_H_DEFINED
#define LEVEL_H_DEFINED

#include "tileset.h"
#include "world.h"
#include "common.h"
#include <string>

class Level
{
public:
	Level(int levelNumber);
	
	~Level();

	void free();

	void unload();

	bool loadMedia(bool renderLoad);
	void update();

	World* getWorld(int worldIndex);

	SDL_Point getCharacterStart();
	
	int getLevelNumber();

	int getDeathLine();

	int getWidth();
	int getHeight();

	bool getFixedX();
	bool getFixedY();

	bool isLoaded();

private:

	int mLevelNumber;

	bool mWorlds[3];

	World* mWorld1;
	World* mWorld2;
	World* mWorld3;

	int mHeight;
	int mWidth;

	bool mLoadState;
	bool mLoadAttempt;

	bool fixedX;
	bool fixedY;

	int mDeathLine;

	int mStartWorld;

	SDL_Point mCharStart;
};
#endif
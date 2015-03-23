#ifndef DOOR_H_DEFINED
#define DOOR_H_DEFINED

#include "common.h"
#include "texture.h"
#include <SDL.h>
#include <string>

class Door 
{
public:
	Door(string id, string destination, int levelIndex, int worldIndex);

	string getDoorID();
	string getDestinationID();
	SDL_Rect getBox();
	int getLevelIndex();
	int getWorldIndex();
	void setBox(int x, int y, int w, int h);
	void setLocked(bool locked);
	bool isLocked();
	bool overlap(SDL_Rect box);
	void renderSprite(int x, int y);

private:
	bool mLocked;
	string mDoorID;
	string mDestinationID;
	int mLevelIndex;
	int mWorldIndex;
	SDL_Rect mBox;
	LTexture* mDoorTexture;

	int mFrames;
};

#endif
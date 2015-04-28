#ifndef DOOR_H_DEFINED
#define DOOR_H_DEFINED

#include "common.h"
#include "texture.h"

class Door
{
public:
	Door(string id, string destination, int levelIndex, int worldIndex, SDL_Rect box);
	void setLocked(bool locked);

	void renderSprite(int x, int y);

	bool overlap(SDL_Rect box);	
	bool isLocked();

	SDL_Rect getBox();

	string getDoorID();
	string getDestinationID();

	int getLevelIndex();
	int getWorldIndex();


private:
	string mDoorID;
	string mDestinationID;

	SDL_Rect mBox;

	Texture* mDoorTexture;

	int mLevelIndex;
	int mWorldIndex;

	bool mLocked;

	int mFrames;
};

#endif
#ifndef DOORMANAGER_H_DEFINED
#define DOORMANAGER_H_DEFINED

#include "common.h"
#include "door.h"
#include "texture.h"
#include <vector>

class DoorManager {

public:
	DoorManager();
	
	~DoorManager();
	void free();

	bool loadMedia();
	void render(int x, int y, int level, int world);
	Door* findOverlapDoor(SDL_Rect box, int level, int world);
	void interact(SDL_Rect mBox, int level, int world, bool action);
	Door* triggerDoor();
	vector<int> getNeighbors(int levelIndex);
	Door* findDestination(Door* door);

	bool loadTextures();
	LTexture* getDoorTexture(int type);

private:
	vector<vector<vector<Door*>>> mDoors;

	bool mTriggerDoor;
	Door* mDoorOverlap;

	vector<LTexture> mDoorTextures;
};

#endif
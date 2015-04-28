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
	bool loadTextures();

	void render(int x, int y, int level, int world);
	void interact(SDL_Rect mBox, int level, int world, bool action);

	vector<int> getNeighbors(int levelIndex);

	Door* findDestination(Door* door);
	Door* findOverlapDoor(SDL_Rect box, int level, int world);
	Door* triggerDoor();
	
	vector<vector<vector<Door*>>> getDoors();

	Texture* getDoorTexture(int type);

private:
	vector<vector<vector<Door*>>> mDoors;
	vector<Texture> mDoorTextures;

	Door* mDoorOverlap;
	bool mTriggerDoor;
};

#endif
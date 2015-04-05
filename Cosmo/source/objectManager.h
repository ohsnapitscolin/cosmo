#ifndef OBJECTMANAGER_H_DEFINED
#define OBJECTMANAGER_H_DEFINED

#include "common.h"
#include "object.h"
#include "menuItem.h"
#include <vector>

class ObjectManager {
public:
	ObjectManager();
	~ObjectManager();

	void free();

	bool loadMedia();
	
	void render(int x, int y, int level, int world);
	void interact(SDL_Rect mBox, int level, int world, bool action);
	int triggerWorld();

	int findStaticCollisions(SDL_Rect box, int level, int world, int delta, int direction);
	
	bool useKey();

	void renderKeyDisplay();

private:	

	bool loadObjectTextures();
	bool loadObjects();

	vector<Object*> findOverlapObjects(SDL_Rect box, int level, int world);

	vector<vector<vector<Object*>>> mObjects;
	int mKeyCount;

	bool mTriggerWorld;
	int mWorldOverlap;

	vector<LTexture> mObjectTextures;

	MenuItem* mKeyDisplay;
};

#endif
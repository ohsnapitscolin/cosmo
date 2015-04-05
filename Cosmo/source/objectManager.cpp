#include "objectmanager.h"
#include "loadManager.h"
#include "soundManager.h"
#include "menuItem.h"
#include <algorithm>
#include <fstream>

const int NUM_LEVELS = 10;
const int NUM_WORLDS = 3;

ObjectManager::ObjectManager() 
{
	mObjects.resize(NUM_LEVELS);
	for (int i = 0; i < int(mObjects.size()); i++) {
		mObjects[i].resize(NUM_WORLDS);
	}
	SDL_Color textColor = { 255, 255, 255 };
	mKeyDisplay = new MenuItem(50, 975, "0", textColor);
}

ObjectManager::~ObjectManager() 
{
	free();
}

bool ObjectManager::loadMedia()
{
	bool success = true;

	if (!loadObjectTextures()) {
		success = false;
	}
	else {
		success &= loadObjects();
		success &= mKeyDisplay->loadText();
	}
	return success;
}

bool ObjectManager::loadObjectTextures()
{
	bool success = true;

	ifstream map("resources/objects/header.txt");

	if (!map) {
		printf("Unable to load object textures\n!");
		success = false;
	}
	else {
		int textureCount;
		map >> textureCount;

		mObjectTextures.resize(textureCount);

		for (int i = 0; i < textureCount; i++) {
			string textureName;
			map >> textureName;

			int spriteWidth, spriteHeight;
			map >> spriteWidth;
			map >> spriteHeight;

			if (!mObjectTextures[i].loadFromFile("resources/objects/" + textureName)) {
				printf("Unable to load texture %d\n!", textureName.c_str());
				success = false;
				break;
			}
			else {
				mObjectTextures[i].clipSprite(spriteWidth, spriteHeight);
			}

			if (map.fail()) {
				printf("Map reached unexpected end of file!\n");
				success = false;
			}
		}
	}
	return success;
}


bool ObjectManager::loadObjects() 
{
	bool success = true;

	fstream map("resources/objects/objects.txt");

	if (!map) {
		printf("Unable to load objects!");
		success = false;
	}
	else {
		int totalWorlds;
		map >> totalWorlds;

		for (int i = 0; i < totalWorlds; i++) {
			int levelNumber, worldNumber;
			map >> levelNumber;
			map >> worldNumber;

			int objectCount;
			map >> objectCount;

			printf("%d %d %d\n", levelNumber, worldNumber, objectCount);
			mObjects[levelNumber][worldNumber - 1].resize(objectCount);
			
			for (int j = 0; j < objectCount; j++) {
				string objectId;
				map >> objectId;

				int objectType;
				map >> objectType;

				int x, y;
				bool visible;
				map >> x;
				map >> y;
				map >> visible;

				printf("%d %d %d: %d\n", levelNumber, worldNumber, j, objectType);
				mObjects[levelNumber][worldNumber - 1][j] = new Object(objectId, objectType, &mObjectTextures[objectType], x, y);
				mObjects[levelNumber][worldNumber - 1][j]->setVisible(visible);

				if (objectType == 6) {
					string interaction;
					map >> interaction;
					mObjects[levelNumber][worldNumber - 1][j]->setInteraction(interaction);						
				}

				if (map.fail()) {
					printf("Map reached unexpected end of file!\n");
					success = false;
				}
			}
		}
	}
	return success;
}

bool ObjectManager::useKey() 
{
	if (mKeyCount > 0) {
		mKeyCount--;
		mKeyDisplay->updateText(to_string(mKeyCount));
		return true;
	}
	else {
		return false;
	}
}

void ObjectManager::interact(SDL_Rect box, int level, int world, bool action) 
{
	vector<Object*> objects = findOverlapObjects(box, level, world);
	if (!objects.empty()) {
		for (int i = 0; i < int(objects.size()); i++) {
			Object* currentObject = objects[i];
			switch (currentObject->getType()) {
			case 0:
				if (action) {
					soundManager.playSound(3);
					mKeyCount++;
					mKeyDisplay->updateText(to_string(mKeyCount));
					currentObject->setVisible(false);
				}
				break;
			case 1:
				mTriggerWorld = true;
				mWorldOverlap = 1;
				soundManager.playSound(5);
				break;
			case 2:
				mTriggerWorld = true;
				mWorldOverlap = 2;
				soundManager.playSound(5);
				break;
			case 3:
				mTriggerWorld = true;
				mWorldOverlap = 3;
				soundManager.playSound(5);
				break;
			case 4:
				if (action) {
					unlock1 = true;
				}
				break;
			case 5:
				if (action) {
					unlock3 = true;
				}
				break;
			case 6:
				if (action) {
					soundManager.playSound(3);
					string interaction = currentObject->getInteraction();
					for (int i = 0; i < NUM_LEVELS; i++) {
						for (int j = 0; j < NUM_WORLDS; j++) {
							for (int k = 0; k < int(mObjects[i][j].size()); k++) {
								if (mObjects[i][j][k]->getId() == interaction) {
									mObjects[i][j][k]->setVisible(true);
								}
							}
						}
					}
				}
			}
		}
	}
}


int ObjectManager::findStaticCollisions(SDL_Rect box, int level, int world, int delta, int direction) {
	
	int topA = box.y;
	int bottomA = box.y + box.h - 1;
	int leftA = box.x;
	int rightA = box.x + box.w - 1;

	int distance = delta;

	for (int i = 0; i < int(mObjects[level][world-1].size()); i++)
	{
		Object* currentObject = mObjects[level][world - 1][i];
		/*if (!currentObject->isFixed()) {
			continue;
		}*/

		SDL_Rect objectBox = currentObject->getBox();

		int topB = objectBox.y;
		int bottomB = objectBox.y + objectBox.h - 1;
		int leftB = objectBox.x;
		int rightB = objectBox.x + objectBox.w - 1;

		int bA = bottomA;
		int tA = topA;
		int rA = rightA;
		int lA = leftA;

		if (direction == DOWN) {
			bA += delta;
			tA += delta;
		}
		if (direction == UP) {
			tA -= delta;
			bA -= delta;
		}
		if (direction == LEFT) {
			lA -= delta;
			rA -= delta;
		}
		if (direction == RIGHT) {
			rA += delta;
			lA += delta;
		}
		if (topB > bA || tA > bottomB || lA > rightB || rA < leftB) {
			//printf("CONTINUE");
			continue;
		}

		int currentDistance = distance;

		if (direction == UP) {
			if (topA - delta <= bottomB) {
				currentDistance = bottomB - topA - 1;
				currentDistance = min(currentDistance, delta);
			}
		}
		if (direction == DOWN) {
			if (bottomA + delta >= topB) {
				currentDistance = topB - bottomA - 1;
				currentDistance = min(currentDistance, delta);
			}
		}
		if (direction == LEFT) {
			printf("%d\n", delta);
			if ((leftA - delta) <= rightB) {
				currentDistance = leftA - rightB - 1;
				currentDistance = min(currentDistance, delta);
				printf("leftA: %d rightB: %d %d\n", leftA, rightB, currentDistance);
			}
		}
		if (direction == RIGHT) {
			printf("right\n");
			if (rightA + delta >= leftB) {
				currentDistance = leftB - rightA - 1;
				currentDistance = min(currentDistance, delta);
			}
		}

		if (currentDistance < distance) {
			distance = currentDistance;
		}
	}

	return distance;
}

void ObjectManager::render(int x, int y, int level, int world) 
{
	vector<Object*> objects = mObjects[level][world-1];
	if (!objects.empty()) {
		for (int i = 0; i < int(objects.size()); i++) {
			objects[i]->renderSprite(x, y);
		}
	}
}

vector<Object*> ObjectManager::findOverlapObjects(SDL_Rect box, int level, int world) 
{
	vector<Object*> objects = mObjects[level][world-1];
	vector<Object*> overlapObjects;
	if (!objects.empty()) {
		for (int i = 0; i < int(objects.size()); i++) {
			Object* currentObject = objects[i];
			if (currentObject->overlap(box)) {
				overlapObjects.push_back(currentObject);
			}
		}
	}
	return overlapObjects;
}

int ObjectManager::triggerWorld() 
{
	if (mTriggerWorld) {
		mTriggerWorld = false;
		return mWorldOverlap;
	}
	return NONE;
}

void ObjectManager::renderKeyDisplay() 
{
	mKeyDisplay->render();
}

void ObjectManager::free() 
{
	for (int i = 0; i < NUM_LEVELS; i++) {
		for (int j = 0; j < NUM_WORLDS; j++) {
			for (int k = 0; k < int(mObjects[i][j].size()); k++) {
				if (mObjects[i][j][k] != NULL) {
					delete mObjects[i][j][k];
					mObjects[i][j][k] = NULL;
				}
			}
		}
	}
	for (int i = 0; i < int(mObjectTextures.size()); i++) {
		mObjectTextures[i].free();
	}
	if (mKeyDisplay != NULL) {
		mKeyDisplay->free();
		delete mKeyDisplay;
		mKeyDisplay = NULL;
	}
}


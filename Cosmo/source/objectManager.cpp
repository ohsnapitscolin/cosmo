#include "objectmanager.h"

#include <algorithm>
#include <fstream>

#include "doorManager.h"
#include "level.h"
#include "loadManager.h"
#include "menuItem.h"
#include "platformSet.h"
#include "soundManager.h"

const int NUM_LEVELS = 10;
const int NUM_WORLDS = 3;

ObjectManager::ObjectManager() 
{
	mObjects.resize(NUM_LEVELS);
	for (int i = 0; i < int(mObjects.size()); i++) {
		mObjects[i].resize(NUM_WORLDS);
	}

	SDL_Color textColor = { 255, 255, 255 };
	mKeyDisplay = new MenuItem(0, 0, "0", textColor, textColor);
}

ObjectManager::~ObjectManager() {
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
				printf("Unable to load texture %s\n!", textureName.c_str());
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

			mObjects[levelNumber][worldNumber - 1].resize(objectCount);
			
			if (map.fail()) {
				printf("Map reached unexpected end of file!\n");
				success = false;
				break;
			}

			for (int j = 0; j < objectCount; j++) {
				string objectId;
				map >> objectId;

				int objectType;
				map >> objectType;

				int x, y;
				bool visible;
				bool fixed;
				map >> x;
				map >> y;
				map >> visible;
				map >> fixed;

				mObjects[levelNumber][worldNumber - 1][j] = new Object(objectId, objectType, &mObjectTextures[objectType], x, y);
				mObjects[levelNumber][worldNumber - 1][j]->setVisible(visible);
				mObjects[levelNumber][worldNumber - 1][j]->setFixed(fixed);

				if (objectType == 6) {
					int count;
					map >> count;
					for (int i = 0; i < count; i++) {
						string interaction;
						map >> interaction;
						mObjects[levelNumber][worldNumber - 1][j]->addInteraction(interaction);
					}
				}

				if (map.fail()) {
					printf("Map reached unexpected end of file!\n");
					success = false;
					break;
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
		mKeyDisplay->updateText(to_string(static_cast<long long>(mKeyCount)));
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
					mKeyDisplay->updateText(to_string(static_cast<long long>(mKeyCount)));
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
					soundManager.playSound(7);
					unlock1 = true;
					currentWorldIndex = 1;
					currentObject->setVisible(false);
				}
				break;
			case 5:
				if (action) {
					soundManager.playSound(7);
					unlock2 = true;
					currentWorldIndex = 2;
					currentObject->setVisible(false);
				}
				break;
			case 6:
				if (action) {
					soundManager.playSound(3);
					vector<string> interactions = currentObject->getInteractions();
					for (int i = 0; i < NUM_LEVELS; i++) {
						for (int j = 0; j < NUM_WORLDS; j++) {
							for (int k = 0; k < int(mObjects[i][j].size()); k++) {
								for (int l = 0; l < int(interactions.size()); l++) {
									if (mObjects[i][j][k]->getId() == interactions[l]) {
										mObjects[i][j][k]->toggleVisible();
									}
								}
							}
						}
					}
					vector<vector<vector<Door*>>> doors = doorManager.getDoors();
					for (int i = 0; i < NUM_LEVELS; i++) {
						for (int j = 0; j < NUM_WORLDS; j++) {
							for (int k = 0; k < int(doors[i][j].size()); k++) {
								for (int l = 0; l < int(interactions.size()); l++) {
									if (doors[i][j][k]->getDoorID() == interactions[l]) {
										doors[i][j][k]->setLocked(false);
									}
								}
							}
						}
					}
					vector<Platform*> platforms = currentLevel->getWorld(currentWorldIndex)->getPlatformSet()->getPlatforms();
					for (int i = 0; i < int(platforms.size()); i++) {
						for (int j = 0; j < int(interactions.size()); j++) {
							if (platforms[i]->getId() == interactions[j]) {
								platforms[i]->toggle();
							}
						}
					}
				}
			}
		}
	}
}


int ObjectManager::findStaticCollisions(SDL_Rect box, int level, int world, int delta, int direction) 
{	
	int topA = box.y;
	int bottomA = box.y + box.h - 1;
	int leftA = box.x;
	int rightA = box.x + box.w - 1;

	int distance = delta;

	for (int i = 0; i < int(mObjects[level][world-1].size()); i++)
	{
		Object* currentObject = mObjects[level][world - 1][i];
		if (!currentObject->isFixed()) {
			continue;
		}

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
			if ((leftA - delta) <= rightB) {
				currentDistance = leftA - rightB - 1;
				currentDistance = min(currentDistance, delta);
			}
		}
		if (direction == RIGHT) {
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

void ObjectManager::renderKeyDisplay() {
	mKeyDisplay->render(50, SCREEN_HEIGHT - 100);
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


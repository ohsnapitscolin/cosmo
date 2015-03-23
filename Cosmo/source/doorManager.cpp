#include "doorManager.h"
#include "loadManager.h"
#include "objectManager.h"
#include "soundManager.h"
#include <fstream>

const int NUM_LEVELS = 10;
const int NUM_WORLDS = 3;

DoorManager::DoorManager() 
{
	mDoors.resize(NUM_LEVELS);
	for (int i = 0; i < int(mDoors.size()); i++) {
		mDoors[i].resize(NUM_WORLDS);
	}
}

DoorManager::~DoorManager(){
	void free();
}

bool DoorManager::loadMedia() 
{
	bool success = true;

	success &= loadTextures();

	fstream map("resources/doors/doors.txt");

	if (!map) {
		printf("Unable to load doors!");
		success = false;
	}
	else {
		int totalWorlds;
		map >> totalWorlds;

		for (int i = 0; i < totalWorlds; i++) {
			int levelNumber, worldNumber;
			map >> levelNumber;
			map >> worldNumber;

			int DoorCount;
			map >> DoorCount;

			printf("%d %d %d\n", levelNumber, worldNumber, DoorCount);
			mDoors[levelNumber][worldNumber - 1].resize(DoorCount);

			for (int j = 0; j < DoorCount; j++) {
				string doorID, destinationID;
				
				map >> doorID;
				map >> destinationID;

				int x, y, w, h;
				bool locked;

				map >> x;
				map >> y;
				map >> w;
				map >> h;

				map >> locked;

				mDoors[levelNumber][worldNumber - 1][j] = new Door(doorID, destinationID, levelNumber, worldNumber);
				mDoors[levelNumber][worldNumber - 1][j]->setBox(x, y, w, h);
				mDoors[levelNumber][worldNumber - 1][j]->setLocked(locked);
				
				if (map.fail()) {
					printf("Map reached unexpected end of file!\n");
					success = false;
				}
			}
		}
	}
	return success;
}

LTexture* DoorManager::getDoorTexture(int type) {
	if (type >= 0 && type < int(mDoorTextures.size())) {
		return &mDoorTextures[type];
	}
	return NULL;
}

bool DoorManager::loadTextures()
{
	bool success = true;

	ifstream map("resources/doors/header.txt");

	if (!map) {
		printf("Unable to load door textures\n!");
		success = false;
	}
	else {
		int textureCount;
		map >> textureCount;

		mDoorTextures.resize(textureCount);

		for (int i = 0; i < textureCount; i++) {
			string textureName;
			map >> textureName;

			int spriteWidth, spriteHeight;
			map >> spriteWidth;
			map >> spriteHeight;

			if (!mDoorTextures[i].loadFromFile("resources/doors/" + textureName)) {
				printf("Unable to load texture %d\n!", textureName.c_str());
				success = false;
				break;
			}
			else {
				mDoorTextures[i].clipSprite(spriteWidth, spriteHeight);
			}

			if (map.fail()) {
				printf("Map reached unexpected end of file!\n");
				success = false;
			}
		}
	}
	printf("Door textures loaded! successfully!\n");
	return success;
}

void DoorManager::interact(SDL_Rect box, int level, int world, bool action) {
	Door* door = findOverlapDoor(box, level, world);
	if (door != NULL) {
		if (door->isLocked()) {
			if (objectManager.useKey()) {
				door->setLocked(false);
				Door* destination = findDestination(door);
				if (destination != NULL) {
					soundManager.playSound(1);
					mTriggerDoor = true;
					mDoorOverlap = destination;
				}
			}
			else {
				soundManager.playSound(2);
			}
		}
		else {
			Door* destination = findDestination(door);
			if (destination != NULL) {
				soundManager.playSound(1);
				mTriggerDoor = true;
				mDoorOverlap = destination;
			}
		}
	}
}

Door* DoorManager::findDestination(Door* door) {
	Door* destination = NULL;
	for (int i = 0; i < NUM_LEVELS; i++) {
		for (int j = 0; j < NUM_WORLDS; j++) {
			for (int k = 0; k < int(mDoors[i][j].size()); k++) {
				if (mDoors[i][j][k]->getDoorID() == door->getDestinationID()) {
					destination = mDoors[i][j][k];
				}
			}
		}
	}
	return destination;
}

vector<int> DoorManager::getNeighbors(int levelIndex) {
	vector<int> neighbors;
	vector<vector<Door*>> doors = mDoors[levelIndex];
	for (int i = 0; i < NUM_WORLDS; i++) {
		for (int j = 0; j < int(mDoors[levelIndex][i].size()); j++) {
			Door* destination = findDestination(mDoors[levelIndex][i][j]);
			if (destination != NULL) {
				neighbors.push_back(destination->getLevelIndex());
			}
		}
	}
	return neighbors;
}

void DoorManager::render(int x, int y, int level, int world) {
	vector<Door*> doors = mDoors[level][world - 1];
	if (!doors.empty()) {
		for (int i = 0; i < int(doors.size()); i++) {
			doors[i]->renderSprite(x, y);
		}
	}
}

Door* DoorManager::findOverlapDoor(SDL_Rect box, int level, int world) {
	vector<Door*> doors = mDoors[level][world - 1];
	Door* overlapDoor = NULL;
	if (!doors.empty()) {
		for (int i = 0; i < int(doors.size()); i++) {
			Door* currentDoor = doors[i];
			SDL_Rect rbox = currentDoor->getBox();
			if (currentDoor->overlap(box)) {
				overlapDoor = currentDoor;
				break;
			}
		}
	}
	return overlapDoor;
}

Door* DoorManager::triggerDoor() 
{
	if (mTriggerDoor) {
		mTriggerDoor = false;
		return mDoorOverlap;
	}
	return NULL;
}

void DoorManager::free() {
	for (int i = 0; i < NUM_LEVELS; i++) {
		for (int j = 0; j < NUM_WORLDS; j++) {
			for (int k = 0; k < int(mDoors[i][j].size()); k++) {
				if (mDoors[i][j][k] != NULL) {
					delete mDoors[i][j][k];
					mDoors[i][j][k] = NULL;
				}
			}
		}
	}
	for (int i = 0; i < int(mDoorTextures.size()); i++) {
		mDoorTextures[i].free();
	}
}

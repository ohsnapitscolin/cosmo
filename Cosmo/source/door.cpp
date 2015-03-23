#include "doorManager.h"

Door::Door(string doorID, string destinationID, int levelIndex, int worldIndex) 
{
	mDoorID = doorID;
	mDestinationID = destinationID;
	mLevelIndex = levelIndex;
	mWorldIndex = worldIndex;

	mDoorTexture = NULL;
}

void Door::setBox(int x, int y, int w, int h) 
{
	mBox.x = x;
	mBox.y = y;
	mBox.w = w;
	mBox.h = h;
}

void Door::setLocked(bool locked) 
{
	if (locked) {
		mDoorTexture = doorManager.getDoorTexture(0);
	}
	if (!locked) {
		mDoorTexture = NULL;
	}
	mLocked = locked;
}

string Door::getDoorID() {
	return mDoorID;
}

string Door::getDestinationID() {
	return mDestinationID;
}

SDL_Rect Door::getBox() {
	return mBox;
}

int Door::getLevelIndex() {
	return mLevelIndex;
}

int Door::getWorldIndex() {
	return mWorldIndex;
}

bool Door::isLocked() {
	return mLocked;
}

void Door::renderSprite(int x, int y) 
{
	if (mDoorTexture == NULL) {
		return;
	}

	if (mFrames >= INT_MAX) {
		mFrames = 0;
	}

	int currentSprite = mFrames / 8 % mDoorTexture->getSpriteCount();
	mDoorTexture->renderSprite(mBox.x + x, mBox.y + y, currentSprite, false);

	mFrames++;
}

bool Door::overlap(SDL_Rect box)
{
	int leftA = mBox.x;
	int rightA = mBox.x + mBox.w;
	int topA = mBox.y;
	int bottomA = mBox.y + mBox.h;

	int leftB = box.x;
	int rightB = box.x + box.w;
	int topB = box.y;
	int bottomB = box.y + box.h;

	if (bottomA <= topB) {
		return false;
	}

	if (topA >= bottomB) {
		return false;
	}

	if (rightA <= leftB) {
		return false;
	}

	if (leftA >= rightB) {
		return false;
	}

	return true;
}

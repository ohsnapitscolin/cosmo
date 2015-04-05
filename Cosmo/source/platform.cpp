#include "platform.h"
#include "platformManager.h"
#include "texture.h"
#include "common.h"

enum {HORIZONTAL, VERTICAL};

Platform::Platform(int x, int y, int tileType, int platformType)
{
	//Get the offsets
	mBox.x = x;
	mBox.y = y;

	mPlatformTexture = platformManager.getPlatformTexture(platformType);

	//Set the collision box
	mBox.w = mPlatformTexture->getWidth();
	mBox.h = mPlatformTexture->getHeight();

	//Get the tile type
	mType = tileType;

	mSpeed = 0;
}

void Platform::render(SDL_Rect& camera)
{
	mPlatformTexture->render(mBox.x - camera.x, mBox.y - camera.y);
}

void Platform::setMotion(int start, int end, int direction, int speed) {
	mStart = start;
	mEnd = end;
	mDirection = direction;
	mSpeed = speed;
}

void Platform::updatePosition() 
{
	if (mDirection == HORIZONTAL) {
		mBox.x += mSpeed;
		if (mBox.x < mStart) {
			mSpeed *= -1;
		}
		else if (mBox.x > mEnd) {
			mSpeed *= -1;
		}
	}
	else if (mDirection == VERTICAL) {
		mBox.y += mSpeed;
		if (mBox.y < mStart) {
			mSpeed *= -1;
		}
		if (mBox.y > mEnd) {
			mSpeed *= -1;
		}
	}
}

int Platform::getType()
{
	return mType;
}

SDL_Rect Platform::getBox()
{
	return mBox;
}

int Platform::getSpeed() {
	return mSpeed;
}

int Platform::getDirection() {
	return mDirection;
}
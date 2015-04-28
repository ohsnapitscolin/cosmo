#include "platform.h"
#include "platformManager.h"

Platform::Platform(string id, int x, int y, int tileType, int platformType)
{
	mId = id;
	mType = tileType;

	mBox.x = x;
	mBox.y = y;

	mPlatformTexture = platformManager.getPlatformTexture(platformType);

	mBox.w = mPlatformTexture->getWidth();
	mBox.h = mPlatformTexture->getHeight();

	mSpeed = 0;
	mDirection = HORIZONTAL;
	mStart = x;
	mEnd = x;
	
	mMoving = false;
}

void Platform::render(SDL_Rect& camera) {
	mPlatformTexture->render(mBox.x - camera.x, mBox.y - camera.y);
}

void Platform::setMotion(int start, int end, int direction, int speed, bool moving) 
{
	mSpeed = speed;
	mDirection = direction;
	
	mStart = start;
	mEnd = end;

	mMoving = moving;
}

void Platform::updatePosition() 
{
	if (!mMoving) {
		return;
	}

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

void Platform::toggle() {
	mMoving = !mMoving;
}

string Platform::getId() {
	return mId;
}

int Platform::getType() {
	return mType;
}

SDL_Rect Platform::getBox() {
	return mBox;
}

int Platform::getSpeed(int direction) {
	if (!mMoving) {
		return 0;
	}
	else if (direction != mDirection) {
		return 0;
	}
	return mSpeed;
}

int Platform::getDirection() {
	return mDirection;
}
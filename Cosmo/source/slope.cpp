#include "slope.h"
#include "common.h"
#include <algorithm>

Slope::Slope(int left, int right, int blockType, int slopeType)
{
	mLeft = left;
	mRight = right;

	mBlockType = blockType;
	mSlopeType = slopeType;
}

int Slope::getLeft()
{
	return mLeft;
}

int Slope::getRight()
{
	return mRight;
}

int Slope::getBlockType()
{
	return mBlockType;
}

int Slope::getSlopeType()
{
	return mSlopeType;
}


bool Slope::ignore(int direction) {
	int slopeDirection = getDirection();
	if (slopeDirection == direction) {
		return true;
	}
	else if (std::abs(mLeft - mRight) < 4) {
		return true;
	}
}

int Slope::getDirection() {
	if (mLeft < mRight) {
		if (mSlopeType == DOWN) {
			return RIGHT;
		}
		else {
			return LEFT;
		}
	}
	else {
		if (mSlopeType == DOWN) {
			return LEFT;
		}
		else {
			return RIGHT;
		}
	}
}

int Slope::getDistance(int index) 
{
	int numLevels = std::abs(mLeft - mRight) + 1;
	int pixelsPerLevel = TILE_SIZE / numLevels;

	int distance;
	if (mLeft > mRight) {
		distance = TILE_SIZE - index / pixelsPerLevel - (TILE_SIZE - mLeft);
	}
	if (mRight > mLeft) {
		distance = index / pixelsPerLevel + mLeft;
	}

	if (mSlopeType != UP) {
		return TILE_SIZE - distance;
	}

	return distance;
}
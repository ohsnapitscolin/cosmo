#ifndef SLOPE_H_DEFINED
#define SLOPE_H_DEFINED

#include "common.h"

class Slope
{
public:
	Slope(int left, int right, int blockType, int slopeType);
	int getBlockType();
	int getLeft();
	int getRight();
	int getSlopeType();

	int getDistance(int index);
	int getDirection();
	bool ignore(int direction);

private:
	int mLeft;
	int mRight;
	int mBlockType;
	int mSlopeType;
};

#endif
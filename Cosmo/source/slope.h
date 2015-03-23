#ifndef SLOPE_H_DEFINED
#define SLOPE_H_DEFINED

#include <SDL.h>

class Slope
{
public:
	//Initializes position and type
	Slope(int left, int right, int blockType, int slopeType);
	int getBlockType();
	int getLeft();
	int getRight();
	int getSlopeType();

	int getDistance(int index);
	int getDirection();
	bool ignore(int direction);

private:
	//The tile type
	int mLeft;
	int mRight;
	int mBlockType;
	int mSlopeType;
};

#endif
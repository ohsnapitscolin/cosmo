#ifndef COLLISION_H_DEFINED
#define COLLISION_H_DEFINED

#include "tile.h"

class Collision
{
public:
	//Initializes position and type
	Collision(int distance, Tile* tile);
	
	int getDistance();
	Tile* getTile();

private:
	//The tile type
	Tile* mTile;
	int mDistance;
};

#endif
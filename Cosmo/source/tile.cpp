#include "tile.h"
#include "common.h"

Tile::Tile(int x, int y, int tileType)
{
	//Get the offsets
	mBox.x = x;
	mBox.y = y;

	//Set the collision box
	mBox.w = TILE_SIZE;
	mBox.h = TILE_SIZE;

	//Get the tile type
	mType = tileType;
	mDoorIndex = -1;
}

int Tile::getType()
{
	return mType;
}

SDL_Rect Tile::getBox()
{
	return mBox;
}

void Tile::setDoorIndex(int index) {
	mDoorIndex = index;
}

int Tile::getDoorIndex() {
	return mDoorIndex;
}

void Tile::setTeleporterIndex(int index) {
	mTeleporterIndex = index;
}

int Tile::getTeleporterIndex() {
	return mTeleporterIndex;
}


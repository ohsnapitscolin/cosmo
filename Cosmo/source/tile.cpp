#include "tile.h"

Tile::Tile(int x, int y, int tileType)
{
	mBox.x = x;
	mBox.y = y;
	mBox.w = TILE_SIZE;
	mBox.h = TILE_SIZE;

	mType = tileType;
}

int Tile::getType() {
	return mType;
}

SDL_Rect Tile::getBox() {
	return mBox;
}

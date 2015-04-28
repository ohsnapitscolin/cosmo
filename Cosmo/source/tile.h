#ifndef TILE_H_DEFINED
#define TILE_H_DEFINED

#include "common.h"

class Tile
{
public:
	Tile(int x, int y, int tileType);

	void render(SDL_Rect& camera);

	int getType();
	SDL_Rect getBox();

private:
	int mType;
	SDL_Rect mBox;
};

#endif
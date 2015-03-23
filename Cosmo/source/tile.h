#ifndef TILE_H_DEFINED
#define TILE_H_DEFINED

#include <SDL.h>

class Tile
{
public:
	//Initializes position and type
	Tile(int x, int y, int tileType);

	//Shows the tile
	void render(SDL_Rect& camera);

	//Get the tile type
	int getType();
	
	void setDoorIndex(int index);
	int getDoorIndex();
	
	void setTeleporterIndex(int index);
	int getTeleporterIndex();

	//Get the collision box
	SDL_Rect getBox();

private:
	//The attributes of the tile
	SDL_Rect mBox;

	//The tile type
	int mType;
	int mTeleporterIndex;
	int mDoorIndex;
};

#endif
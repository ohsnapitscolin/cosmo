#ifndef TILESET_H_DEFINED
#define TILESET_H_DEFINED

#include <SDL.h>
#include <string>
#include <vector>
#include "common.h"
#include "tile.h"
#include "texture.h"
#include "collision.h"


class TileSet
{
public:
	//Initializes position and type
	TileSet(std::string mapName);
	~TileSet();

	void free();

	bool loadMedia(int width, int height);

	int getWidth();
	int getHeight();

	void findOverlapTiles(SDL_Rect box, vector<Tile*>& overlapTiles);
	bool findStaticCollisions(SDL_Rect box, int direction, int& distance, int& type);

	//Shows the tile
	void render(SDL_Rect& camera);
	void setAlpha(int value);

private:
	int mTileSetHeight;
	int mTileSetWidth;

	//The character sprite location.
	std::string mMapName;

	std::vector<std::vector<Tile*>> mTiles;

	bool setTiles();

	int getPixelIndex(int point);
	int getDistance(int index, int direction);
	int checkForSlopes(int type, int line, int distance, int direction);
	bool ignoreSlopes(int tileType, int direction);
	bool ignoreTile(int tileType, int direction);
};
#endif
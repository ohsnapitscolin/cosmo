#include "tileset.h"
#include <fstream>
#include <algorithm>
#include "slope.h"
#include "loadManager.h"

TileSet::TileSet(std::string mapName) 
{
	mMapName = mapName;
}

TileSet::~TileSet() 
{
	free();
}

bool TileSet::loadMedia(int width, int height)
{
	bool success = true;

	mTileSetWidth = width;
	mTileSetHeight = height;

	if (!setTiles()) {
		printf("Unable to process tileset: %s\n!", mMapName.c_str());
		success = false;
	}

	return success;
}

int TileSet::getWidth()
{
	return mTileSetWidth;
}


int TileSet::getHeight()
{
	return mTileSetHeight;
}

bool TileSet::setTiles()
{
	printf("SET %s\n", mMapName.c_str());

	//Success flag
	bool tilesLoaded = true;

	mTiles.resize(mTileSetHeight / TILE_SIZE, std::vector<Tile*>(mTileSetWidth / TILE_SIZE));

	//Open the map
	std::ifstream map(mMapName);

	int tileCount;
	map >> tileCount;

	/*for (int i = 0; i < mTileSetHeight / TILE_SIZE; i++) {
		for (int j = 0; j < mTileSetWidth / TILE_SIZE; j++) {
			if (mTiles[i][j] != NULL) {
				printf("WHOA %d %d\n", i, j);
			}
		}
	}*/

	for (int i = 0; i < tileCount; i++) {
		//determines what kind of tile will be made
		int x, y, width, height, tileType;

		//read tile box from map file
		map >> tileType;
		map >> x;
		map >> y;
		map >> width;
		map >> height;

		//if the was a problem in reading the map
		if (map.fail()) {
			//stop loading map
			printf("Error loading map: Unexpected end of file!\n");
			tilesLoaded = false;
			break;
		}

		int tile_y = y;
		for (int i = y / TILE_SIZE; i < (y + height) / TILE_SIZE && i < mTileSetHeight / TILE_SIZE; i++) {
			int tile_x = x;
			for (int j = x / TILE_SIZE; j < (x + width) / TILE_SIZE && j < mTileSetWidth / TILE_SIZE; j++) {
				if (mTiles[i][j] == NULL) {
					mTiles[i][j] = new Tile(tile_x, tile_y, tileType);
				}
				tile_x += TILE_SIZE;
			}
			tile_y += TILE_SIZE;
		}
	}

	//close the file
	map.close();

	//if the set was successfully loaded
	return tilesLoaded;
}


bool TileSet::findStaticCollisions(SDL_Rect box, int direction, int& distance, int& type)
{
	int tile_x, tile_y, currentLine, slopeRow;
	Tile* currentTile;
	
	int magnitude = distance;

	switch (direction) {
	case LEFT:
		currentLine = box.x;
		tile_x = currentLine / TILE_SIZE;
		slopeRow = -1;
		distance = -1 * getDistance(getPixelIndex(currentLine), RIGHT) - 1;

		while (distance < magnitude) {
			if (tile_x < 0) {
				return true;
			}
			
			bool collide = false;
			for (int tile_y = box.y / TILE_SIZE; tile_y <= (box.y + box.h) / TILE_SIZE; tile_y++) {
				//ignore all tiles in the same row as our first encountered slope
				if (tile_y == slopeRow) {
					continue;
				}
				if (tile_y >= mTileSetHeight / TILE_SIZE || tile_y < 0) {
					return false;
				}
				currentTile = mTiles[tile_y][tile_x];
				if (currentTile != NULL) {
					collide = true;
					int tileType = currentTile->getType();
				
					collide &= !ignoreTile(tileType, LEFT);
					
					if (ignoreSlopes(tileType, LEFT)) {
						collide = false;
						if (slopeRow < 0) {
							slopeRow = tile_y;
						}
					}

					if (collide) {
						type = tileType;
						return true;
					}
				}
			}
			tile_x--;
			distance += TILE_SIZE;
		}
		break;
	case RIGHT:
		currentLine = box.x + box.w;
		tile_x = currentLine / TILE_SIZE;
		distance = -1 * getDistance(getPixelIndex(currentLine), LEFT) - 1;
		slopeRow = -1;

		while (distance < magnitude) {
			if (tile_x >= mTileSetWidth / TILE_SIZE) {
				return true;
			}

			bool collide = false;
			for (int tile_y = box.y / TILE_SIZE; tile_y <= (box.y + box.h) / TILE_SIZE; tile_y++) {
				//ignore all tiles in the same row as our first encountered slope
				if (tile_y == slopeRow) {
					continue;
				}
				if (tile_y >= mTileSetHeight / TILE_SIZE || tile_y < 0) {
					return false;
				}
				currentTile = mTiles[tile_y][tile_x];
				if (currentTile != NULL) {
					collide = true;
					int tileType = currentTile->getType();
					
					collide &= !ignoreTile(tileType, RIGHT);
					
					if (ignoreSlopes(tileType, RIGHT)) {
						collide = false;
						if (slopeRow < 0) {
							slopeRow = tile_y;
						}
					}

					if (collide) {
						type = tileType;
						return true;
					}
				}
			}
			tile_x++;
			distance += TILE_SIZE;
		}
		break; 
	case UP: 
		currentLine = box.y;
		tile_y = currentLine / TILE_SIZE;
		distance = -1 * getDistance(getPixelIndex(currentLine), DOWN) - 1;
		
		while (distance < magnitude) {
			if (tile_y < 0) {
				return false;
			}

			int verticalEdge = box.x + box.w;
			currentTile = mTiles[tile_y][verticalEdge / TILE_SIZE];	
			if (currentTile != NULL) {
				int tileType = currentTile->getType();
				int slopeDistance = checkForSlopes(tileType, verticalEdge, distance, RIGHT);

				if (slopeDistance != distance) {
					distance = slopeDistance;
					type = tileType;
					return true;
				}
			}

			verticalEdge = box.x;
			currentTile = mTiles[tile_y][verticalEdge / TILE_SIZE];
			if (currentTile != NULL) {
				int tileType = currentTile->getType();
				int slopeDistance = checkForSlopes(tileType, verticalEdge, distance, LEFT);

				if (slopeDistance != distance) {
					distance = slopeDistance;
					type = tileType;
					return true;
				}
			}

			for (int tile_x = box.x / TILE_SIZE; tile_x < (box.x + box.w) / TILE_SIZE + 1; tile_x++) {
				currentTile = mTiles[tile_y][tile_x];
				if (currentTile != NULL) {
					int tileType = currentTile->getType();
					if (!ignoreTile(tileType, UP)) {
						type = tileType;
						return true;
					}
				}
			}
			tile_y--;
			distance += TILE_SIZE;
		}
		break;
	case DOWN:
		currentLine = box.y + box.h;

		//start off checking or current tile
		tile_y = currentLine / TILE_SIZE;

		//represents distance to the bottom edge of a given tile (pixel location = TILE_SIZE * n - 1).
		//inital distance is set to the edge of the tile one above our current tile, and is initially negative. 
		//this allows us to add to our distance in increments less than TILE_SIZE if we encounter a slope.
		distance = -1 * getDistance(getPixelIndex(currentLine), UP) - 1;

		while (distance < magnitude) {
			//check if our current tile is out of bounds
			if (tile_y >= mTileSetHeight / TILE_SIZE) {
				return false;
			}
			//check for a slope on the right and left edges of the current collision box, if a slope is
			//found, then slopeDistance != distance, as slopeDistance is adjusted. this will not return
			//if our character is currently on a slope of height = TILE_SIZE.
			int verticalEdge = box.x + box.w;
			currentTile = mTiles[tile_y][verticalEdge / TILE_SIZE];
			if (currentTile != NULL) {
				int tileType = currentTile->getType();
				int slopeDistance = checkForSlopes(tileType, verticalEdge, distance, RIGHT);

				if (slopeDistance != distance) {
					distance = slopeDistance;
					type = tileType;
					return true;
				}
			}

			verticalEdge = box.x;
			currentTile = mTiles[tile_y][verticalEdge / TILE_SIZE];
			if (currentTile != NULL) {
				int tileType = currentTile->getType();
				int slopeDistance = checkForSlopes(tileType, verticalEdge, distance, LEFT);

				if (slopeDistance != distance) {
					distance = slopeDistance;
					type = tileType;
					return true;
				}
			}

			//if we don't encounter a slope, check the horizontal boxes that overlap with our
			//current y position and our collision box
			for (int tile_x = box.x / TILE_SIZE; tile_x < (box.x + box.w) / TILE_SIZE + 1; tile_x++) {
				currentTile = mTiles[tile_y][tile_x];
				if (currentTile != NULL) {
					int tileType = currentTile->getType();
					if (!ignoreTile(tileType, DOWN)) {
						type = tileType;
						return true;
					}
				}
			}
			//if not collisions were found, check to tiles below our current tile,
			//repeat until distance is greater than or equal to velocity
			tile_y++;
			distance += TILE_SIZE;
		}
	break;
	}
	return false;
}

int TileSet::getPixelIndex(int point) {
	return point % TILE_SIZE;
}

int TileSet::getDistance(int index, int direction) 
{
	//RIGHT represnts distance to the next tile to our character's right.
	//DOWN represents distance to the next tile below our character.
	//we subtract 1 because of 0 based indexing. if index were equal to 0,
	//the distance the next tile would be TILE_SIZE - 1.
	if (direction == RIGHT || direction == DOWN) {
		return TILE_SIZE - index - 1;
	}
	return index;
}


int TileSet::checkForSlopes(int tileType, int verticalEdge, int distance, int direction) 
{
	Slope* currentSlope = loadManager.getCurrentSlope(tileType);
	if (currentSlope != NULL) {
		if (currentSlope->getDirection() == direction) {
			int index = getPixelIndex(verticalEdge);
			distance += currentSlope->getDistance(index);
			return distance + 1;
		}
	}
	return distance;
}

bool TileSet::ignoreSlopes(int tileType, int direction) 
{
	Slope* currentSlope = loadManager.getCurrentSlope(tileType);

	if (currentSlope != NULL) {
		return currentSlope->ignore(direction);
	}
	return false;
}

bool TileSet::ignoreTile(int tileType, int direction)
{
	if (tileType == 30 || tileType == 32 || tileType == 33) {
		return true;
	}
	else if (direction != DOWN) {
		if (tileType == 31) {
			return true;
		}
	}
	return false;
}


void TileSet::findOverlapTiles(SDL_Rect box, vector<Tile*>& overlapTiles) 
{
	for (int tile_y = box.y / TILE_SIZE; tile_y <= (box.y + box.h) / TILE_SIZE; tile_y++) {
		for (int tile_x = box.x / TILE_SIZE; tile_x <= (box.x + box.w) / TILE_SIZE; tile_x++) {
			if (tile_y >= mTileSetHeight / TILE_SIZE || tile_y < 0) {
				continue;
			}
			Tile* currentTile = mTiles[tile_y][tile_x];
			if (currentTile != NULL) {
				overlapTiles.push_back(currentTile);
			}
		}
	}
}

/* DEFINITELY OBJECTS BEING LOST WHEN PASSED TO CHARACTER */
void TileSet::free() 
{
	for (int i = 0; i < mTileSetHeight / TILE_SIZE; i++) {
		for (int j = 0; j < mTileSetWidth / TILE_SIZE; j++) {
			if (mTiles[i][j] != NULL) {
				delete mTiles[i][j];
				mTiles[i][j] = NULL;
			}
		}
	}

	mTileSetWidth = 0;
	mTileSetHeight = 0;
}
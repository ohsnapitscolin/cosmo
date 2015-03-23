#ifndef WORLD_H_DEFINED
#define WORLD_H_DEFINED

#include <string>
#include "common.h"
#include "tileset.h"
#include "platformset.h"

struct worldTexture {
	int offsetH;
	int offsetV;
	string name;
	LTexture* texture;
};

class World
{
public:
	World(int index, std::string levelFolder);
	
	~World();

	void free();
	void unload();

	bool loadMedia();

	void update();

	TileSet* getTileSet();
	PlatformSet* getPlatformSet();

	void setAlpha(int alpha);
	void render(int x, int y, SDL_Rect& camera);
	void renderForeground(int x, int y);

	int getWidth();
	int getHeight();
	
private:
	TileSet* mTileSet;
	PlatformSet* mPlatformSet;

	LTexture* mGridTexture;
	vector<LTexture*> mBackgrounds;
	vector<LTexture*> mForegrounds;

	std::string mWorld;

	int mHeight;
	int mWidth;
};
#endif
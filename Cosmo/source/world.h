#ifndef WORLD_H_DEFINED
#define WORLD_H_DEFINED

#include "common.h"
#include "tileset.h"
#include "platformset.h"

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

	Texture* mGridTexture;
	vector<Texture*> mBackgrounds;
	vector<Texture*> mForegrounds;

	string mWorld;

	int mWidth;
	int mHeight;
};

#endif
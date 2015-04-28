#ifndef PLATFORM_MANAGER_H_DEFINED
#define PLATFORM_MANAGER_H_DEFINED

#include "common.h"
#include "texture.h"

class PlatformManager
{
public:
	PlatformManager();
	~PlatformManager();
	void free();

	bool loadMedia();

	Texture* getPlatformTexture(int type);

private:
	vector<Texture> mPlatformTextures;
};

#endif
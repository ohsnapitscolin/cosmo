#ifndef PLATFORM_MANAGER_H_DEFINED
#define PLATFORM_MANAGER_H_DEFINED

#include "common.h"
#include "texture.h"
#include <vector>

class PlatformManager
{
public:
	PlatformManager();
	~PlatformManager();
	void free();
	bool loadMedia();

	LTexture* getPlatformTexture(int type);
private:

	vector<LTexture> mPlatformTextures;
};

#endif
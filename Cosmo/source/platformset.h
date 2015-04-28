#ifndef PLATFORMSET_H_DEFINED
#define PLATFORMSET_H_DEFINED

#include "common.h"
#include "platform.h"

class PlatformSet
{
public:
	PlatformSet(std::string filename);

	~PlatformSet();	
	void free();

	bool loadPlatforms();

	void update();

	void render(SDL_Rect& camera);

	Platform* findPlatformCollisions(SDL_Rect box, int direction, int& distance);

	vector<Platform*> getPlatforms();

private:
	string mFilename;
	vector<Platform*> mPlatforms;
};

#endif
#ifndef PLATFORMSET_H_DEFINED
#define PLATFORMSET_H_DEFINED

#include <SDL.h>
#include <vector>
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

	void setAlpha(int alpha);
	void render(SDL_Rect& camera);

	bool findPlatformCollisions(SDL_Rect box, int direction, int& distance, int& index);
	int findDistance(SDL_Rect mBox, int index, int direction);
	
	int getSpeed(int index, int direction);
private:
	std::string mFilename;
	std::vector<Platform*> mPlatforms;
};
#endif
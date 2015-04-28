#ifndef PLATFORM_H_DEFINED
#define PLATFORM_H_DEFINED

#include "common.h"
#include "texture.h"

class Platform
{
public:
	Platform(string id, int x, int y, int tileType, int platformType);
	void setMotion(int start, int end, int direction, int speed, bool moving);

	void setAlpha(int alpha);
	void render(SDL_Rect& camera);

	void updatePosition();
	void toggle();

	string getId();
	int getType();

	SDL_Rect getBox();

	int getSpeed(int direction);
	int getDirection();

private:
	int mType;
	string mId;

	SDL_Rect mBox;

	Texture* mPlatformTexture;

	int mSpeed;
	int mDirection;
	
	int mStart;
	int mEnd;

	bool mMoving;
};

#endif
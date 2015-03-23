#ifndef PLATFORM_H_DEFINED
#define PLATFORM_H_DEFINED

#include <SDL.h>
#include "texture.h"

class Platform
{
public:
	//Initializes position and type
	Platform(int x, int y, int w, int h, int tileType);

	bool loadMedia();

	void setMotion(int start, int end, int direction, int speed);

	void setAlpha(int alpha);

	//Shows the tile
	void render(SDL_Rect& camera);
	
	//Get the tile type
	int getType();

	void updatePosition();

	//Get the collision box
	SDL_Rect getBox();
	int getSpeed();
	int getDirection();

private:
	//The attributes of the tile
	SDL_Rect mBox;

	LTexture mPlatformTexture;

	//The tile type
	int mType;

	int mSpeed;
	int mDirection;
	
	int mStart;
	int mEnd;
};

#endif
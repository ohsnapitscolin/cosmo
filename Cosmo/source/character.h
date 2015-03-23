#ifndef CHARACTER_H
#define CHARACTER_H

#include <SDL.h>
#include <string>
#include <vector>
#include "texture.h"
#include "object.h"

enum {STANDING, JUMPING, FALLING, CLIMBING};

class Character
{
public:
	Character(int type);

	void loadMedia();

	void loadState();

	bool checkForOverlap(int worldIndex);
	void setAlpha(int alpha);

	void handleEvent(SDL_Event& e);
	void move(SDL_Rect& camera);

	void saveState();

	SDL_Rect getBox();

	void setCamera(SDL_Rect& camera);
	void setPosition(int x, int y);
	SDL_Point getPosition();
	void render(SDL_Rect& camera);

	int getKeyCount();

private:

	SDL_Rect mBox;

	float mVelX, mVelY;

	int mFrame;

	int currentDirection;

	bool mIdleJump;
	bool mInAir;

	int mCharState;
	int mPrevState;

	bool mLadderOverlap;
	bool mLadderBelow;

	void updateCharSpeed();

	int mCurrentPlatform;

	bool sheetComplete(int currentAnimation, int speed);
	void checkForOverlaps();

	std::vector<LTexture*> mTextures;

	int mType;

	float mTargetSpeedX;
};
#endif
#ifndef CHARACTER_H
#define CHARACTER_H

#include "platform.h"
#include "texture.h"

enum states { STANDING, JUMPING, FALLING, CLIMBING };
enum spirtes { WALK, JUMP, IDLE, FALL, CLIMB, LAND };

class Character
{
public:
	Character(int type);

	bool loadMedia();

	void move(SDL_Rect& camera);
	void handleEvent(SDL_Event& e);

	void render(SDL_Rect& camera);

	bool saveState();
	bool loadState();

	bool checkForOverlap(int worldIndex);

	void setAlpha(int alpha);
	void setCamera(SDL_Rect& camera);
	void setPosition(int x, int y);

	SDL_Point getPosition();
	SDL_Rect getBox();

private:
	int mType;
	
	SDL_Rect mBox;

	float mVelX;
	float mVelY;
	float mTargetSpeedX;

	std::vector<Texture*> mTextures;

	int currentDirection;

	bool mIdleJump;
	bool mInAir;

	int mCurrentState;
	int mPrevState;

	bool mLadderOverlap;
	bool mLadderBelow;

	Platform* mCurrentPlatform;

	void updateCharSpeed();
	void checkForOverlaps();
	bool sheetComplete(int currentAnimation, int speed);

	int mFrame;
};

#endif
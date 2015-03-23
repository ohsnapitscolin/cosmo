#include "character.h"
#include <algorithm>
#include "common.h"
#include "collision.h"
#include "tileset.h"
#include "platformset.h"
#include "level.h"
#include "world.h"
#include "loadManager.h"
#include "doorManager.h"
#include "objectManager.h"
#include "soundManager.h"
#include <fstream>

float JUMP_FORCE = 13.5f;
const float CHAR_VEL = 5.0;
const float CLIMB_VEL = 3.0f;
const float aX = 0.5f;
const float jumpX = 0.05f;

int oldWorld = 1;

Character::Character(int type) {

	//Initialize the collision box
	mBox.x = 100;
	mBox.y = 0;
	mBox.w = 32;
	mBox.h = 96;

	//Initialize the velocity
	mVelX = 0.0;
	mVelY = 0.0;

	//Initialize the sprites
	mFrame = 0;

	//Initial the sprite location
	mLadderOverlap = false;
	mLadderBelow = false;
	
	mPrevState = 0;

	mIdleJump = false;
	mInAir = false;

	mCurrentPlatform = NONE;
	currentDirection = RIGHT;

	mType = type;
}


void Character::loadMedia() {
	mTextures = loadManager.getSprite(mType);
}

/*void Character::handleEvent(SDL_Event& e)
{
	if (currentWorldIndex != oldWorld) {
		mCurrentPlatform = NONE;
	}

	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP:
			objectManager.interact(mBox, currentLevel->getLevelNumber(), currentWorldIndex, true);			
			doorManager.interact(mBox, currentLevel->getLevelNumber(), currentWorldIndex, true);
			if (mDoorOverlap != NONE) {
				mTriggerLoad = true;
			}
			if (mCharState != CLIMBING && mLadderOverlap) {
				mCharState = CLIMBING;
				mVelY = -1 * CLIMB_VEL;
				break;
			}
			if (mCharState == CLIMBING) {
				mVelY -= CLIMB_VEL;
				break;
			}
			break;
		case SDLK_SPACE:
			if (mCharState == STANDING || mCharState == CLIMBING) {
				mVelY = -1 * JUMP_FORCE;
				mCharState = JUMPING;
				soundManager.playSound(0);
			}
			break;
		case SDLK_DOWN: 
			if (mCharState != CLIMBING && (mLadderOverlap || mLadderBelow)) {
				mCharState = CLIMBING;
				mVelY = CLIMB_VEL;
			}
			else {
				mVelY += CLIMB_VEL;
			}
			break;
		case SDLK_LEFT:
			currentDirection = LEFT;
			mVelX -= CHAR_VEL;
			break;
		case SDLK_RIGHT: 
			currentDirection = RIGHT;
			mVelX += CHAR_VEL;
			break;
		}
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP:
			if (mCharState == CLIMBING) {
				mVelY += CLIMB_VEL;
			}
			break;
		case SDLK_LEFT: 
			mVelX += CHAR_VEL; 
			break;
		case SDLK_RIGHT: 
			mVelX -= CHAR_VEL; 
			break;
		case SDLK_DOWN:
			if (mCharState == CLIMBING) {
				mVelY -= CLIMB_VEL;
			}
			break;
		}
	}
}*/

void Character::handleEvent(SDL_Event& e)
{
	if (currentWorldIndex != oldWorld) {
		mCurrentPlatform = NONE;
	}

	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_UP:
			objectManager.interact(mBox, currentLevel->getLevelNumber(), currentWorldIndex, true);
			doorManager.interact(mBox, currentLevel->getLevelNumber(), currentWorldIndex, true);
			break;
		case SDLK_SPACE:
			if (mCharState == STANDING || mCharState == CLIMBING) {
				mVelY = -1 * JUMP_FORCE;
				mCharState = JUMPING;
				soundManager.playSound(0);
			}
			break;
		}
	}

	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

	if (currentKeyStates[SDL_SCANCODE_UP]) {
		if (mCharState != CLIMBING && mLadderOverlap) {
			mCharState = CLIMBING;
			mVelY = 0.0f;
		}
		if (mCharState == CLIMBING) {
			mVelY = -3.0f;
		}
	}
	else if (currentKeyStates[SDL_SCANCODE_DOWN]) {
		if (mCharState != CLIMBING && (mLadderBelow)) {
			mCharState = CLIMBING;
			mVelY = 0.0f;
		}
		if (mCharState == CLIMBING) {
			mVelY = 3.0f;
		}
	}
	else {
		if (mCharState == CLIMBING) {
			mVelY = 0.0f;
		}
	}

	if (currentKeyStates[SDL_SCANCODE_LEFT]) {
		currentDirection = LEFT;
		mTargetSpeedX = -1 * CHAR_VEL;
		if (mCharState == CLIMBING) {
			mTargetSpeedX = -2.0f;
		}
	}
	else if (currentKeyStates[SDL_SCANCODE_RIGHT]) {
		currentDirection = RIGHT;
		mTargetSpeedX = CHAR_VEL;
		if (mCharState == CLIMBING) {
			mTargetSpeedX = 2.0f;
		}
	}
	else {
		mTargetSpeedX = 0.0f;
	}
}

void Character::move(SDL_Rect& camera)
{
	updateCharSpeed();

	World* currentWorld = currentLevel->getWorld(currentWorldIndex);
	TileSet* currentTileSet = currentWorld->getTileSet();
	PlatformSet* currentPlatforms = currentWorld->getPlatformSet();
	
	mLadderBelow = false;
	mLadderOverlap = false;

	//apply gravity
	if (mCharState != CLIMBING) {
		if (mVelY < TERMINAL) {
			mVelY += GRAVITY;
		}
		else {
			mVelY = TERMINAL;
		}
	}
	else {
		mCurrentPlatform = NONE;
	}

	//break velocity into direction and magnitude
	int dirVelX = mVelX < 0 ? LEFT : mVelX > 0 ? RIGHT : NONE;
	int dirVelY = mVelY < 0 ? UP : mVelY > 0 ? DOWN : NONE;

	int magVelX = int(std::abs(mVelX) + .05);
	int magVelY = int(std::abs(mVelY) + .05);


	//**************************************** HORIZONTAL MOVEMENT ****************************************//

	int deltaX = magVelX;

	//update x position, no horizontal movement is allowed while climbing
	//if (mCharState != CLIMBING) {
		int distanceX = magVelX;
		int typeX = NONE;
		if (currentTileSet->findStaticCollisions(mBox, dirVelX, distanceX, typeX)) {
			deltaX = distanceX;
		}

		if (dirVelX == LEFT) {
			mBox.x -= deltaX;
		}
		else if (dirVelX == RIGHT) {
			mBox.x += deltaX;
		}
	//}

	if (mCurrentPlatform != NONE) {
		if (mCharState == STANDING) {
			mBox.x += currentPlatforms->getSpeed(mCurrentPlatform, 0);
		}
		if (mCharState != STANDING) {
			mCurrentPlatform = NONE;
		}
	}

	//ensure character is not out of level bounds 
	if (mBox.x < 0) {
		mBox.x = 0;
	}
	if (mBox.x + mBox.w > currentLevel->getWidth()) {
		mBox.x = currentLevel->getWidth() - mBox.w;
	}

	//**************************************** VERTCIAL MOVEMENT ****************************************//
	int deltaY = magVelY;
	
	if (mCharState != CLIMBING) {
		if (dirVelY == UP && deltaY >= 0) {
			mCharState = JUMPING;
		}
		else if ((dirVelY == DOWN && deltaY >= 0) || dirVelY == NONE) {
			mCharState = FALLING;
		}
		else {
			mCharState = STANDING;
		}
	}

	if (mCharState != CLIMBING) {
		
		int distanceY = magVelY;
		int typeY = NONE;
		
		if (currentTileSet->findStaticCollisions(mBox, dirVelY, distanceY, typeY)) {
			//if (typeY != NONE) {
				if (dirVelY == DOWN) {
					mCharState = STANDING;
				}
				if (dirVelY == UP) {
					mVelY = 0;
				}
			//}
			deltaY = distanceY;
		}

		int index = NONE;

		if (!currentPlatforms->findPlatformCollisions(mBox, dirVelY, distanceY, index)) {
			mCurrentPlatform = NONE;
		};
		
		if (index != NONE) {
			if (dirVelY == DOWN) {
				mCurrentPlatform = index;
				mCharState = STANDING;
			}
			deltaY = distanceY;
		}
	}
	
	if (mCurrentPlatform != NONE) {
		if (mCharState == STANDING) {
			mBox.y += currentPlatforms->getSpeed(mCurrentPlatform, 1);
		}
		if (mCharState != STANDING) {
			mCurrentPlatform = NONE;
		}
	}

	if (dirVelY == UP) {
		mBox.y -= deltaY;
	}
	else if (dirVelY == DOWN) {
		mBox.y += deltaY;
	}

	//ensure character is not out of level bounds 
	/*if (mBox.y < 0) {
		mBox.y = 0;
	}
	if (mBox.y + mBox.h > currentLevel->getHeight()) {
		mBox.y = currentLevel->getHeight() - mBox.h;
	}**/

	checkForOverlaps();
	saveState();
}


void Character::loadState() {
	ifstream load("resources/characters/save.txt");
	int levelIndex, currentWorld, x, y;
	load >> levelIndex;
	load >> currentWorld;
	load >> x;
	load >> y;

	if (levelIndex != currentLevel->getLevelNumber()) {
		SDL_Point charPos = currentLevel->getCharacterStart();
		mBox.x = charPos.x;
		mBox.y = charPos.y;
	}
	else {
		currentWorldIndex = currentWorld;
		mBox.x = x;
		mBox.y = y;
	}
}

void Character::checkForOverlaps() 
{
	World* currentWorld = currentLevel->getWorld(currentWorldIndex);
	TileSet* currentTileSet = currentWorld->getTileSet();
	
	objectManager.interact(mBox, currentLevel->getLevelNumber(), currentWorldIndex, false);
	int dirVelY = mVelY < 0 ? UP : mVelY > 0 ? DOWN : NONE;

	vector<Tile*> overlapTiles;
	currentTileSet->findOverlapTiles(mBox, overlapTiles);

	for (int i = 0; i < int(overlapTiles.size()); i++) {
		Tile* currentTile = overlapTiles[i];
		if (currentTile->getType() == 30) {
			mLadderOverlap = true;
			if (mCharState == CLIMBING) {
				SDL_Rect box = currentTile->getBox();
				//center character on ladder;
				mBox.x = box.x + box.w / 2 - mBox.w / 2;
				break;
			}
		}
		if (currentTile->getType() == 33) {
			mLadderOverlap = true;
		}
	}

	if (mCharState == CLIMBING && !mLadderOverlap) {
		mCharState = STANDING;
	}

	overlapTiles.clear();

	SDL_Rect underBox;
	underBox.x = mBox.x;
	underBox.y = mBox.y + mBox.h;
	underBox.w = mBox.w;
	underBox.h = TILE_SIZE;
	
	currentTileSet->findOverlapTiles(underBox, overlapTiles);
	
	for (int i = 0; i < int(overlapTiles.size()); i++) {
		Tile* currentTile = overlapTiles[i];
		if (currentTile->getType() == 30 || currentTile->getType() == 33) {
			mLadderBelow = true;
		}
	}

	if (!mLadderBelow) {
		if (mCharState == CLIMBING && dirVelY == DOWN) {
			mCharState = FALLING;
		}
	}

	overlapTiles.clear();

	oldWorld = currentWorldIndex;
}


void Character::setCamera(SDL_Rect& camera)
{
	//Center the camera over the character
	camera.x = (mBox.x + mBox.w / 2) - SCREEN_WIDTH / 2;
	camera.y = (mBox.y + mBox.h / 2) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
	if (camera.x < 0) {	
		camera.x = 0;
	}
	if (camera.y < 0) {
		camera.y = 0;
	}
	if (camera.x > currentLevel->getWidth() - camera.w) {
		camera.x = currentLevel->getWidth() - camera.w;
	}
	if (camera.y > currentLevel->getHeight() - camera.h) {
		camera.y = currentLevel->getHeight() - camera.h;
	}
}

void Character::render(SDL_Rect& camera)
{
	if (mFrame >= INT_MAX || mPrevState != mCharState) {
		mFrame = 0;
	}

	int x = mBox.x - camera.x;
	int y = mBox.y - camera.y;

	int currentAnimation = 0;
	int currentSprite = 0;

	int speed = 4;
	bool updateFrame = false;

	if (mCharState != JUMPING) {
		mIdleJump = false;
	}

	if (mCharState == JUMPING) {
		mInAir = true;
		currentAnimation = JUMP;
		if (!mIdleJump && sheetComplete(currentAnimation, speed)) {
			mIdleJump = true;
		}
		if (mIdleJump) {
			currentAnimation = IDLE;
		}
		updateFrame = true;
	}

	if (mCharState == STANDING) {
		if (mInAir) {
			currentAnimation = LAND;
			if (sheetComplete(currentAnimation, speed)) {
				mInAir = false;
			}
			updateFrame = true;
		}
		else {
			currentAnimation = WALK;
			if (mVelX != 0) {
				speed = 5;
				updateFrame = true;
			}
		}
	}

	if (mCharState == CLIMBING) {
		currentAnimation = CLIMB;
		if (mVelY != 0) {
			speed = 8;
			updateFrame = true;
		}
	}

	if (mCharState == FALLING) {
		mInAir = true;
		speed = 6;
		currentAnimation = FALL;
		updateFrame = true;
	}

	if (updateFrame) {
		currentSprite = mFrame / speed % mTextures[currentAnimation]->getSpriteCount();
	}
	
	bool flip = false;
	if (currentDirection == LEFT) {
		flip = true;
	}

	mTextures[currentAnimation]->renderSprite(x, y, currentSprite, flip);
	mFrame++;

	mPrevState = mCharState;
}


bool Character::sheetComplete(int currentAnimation, int speed) 
{
	return mFrame / speed >= mTextures[currentAnimation]->getSpriteCount();
}

void Character::setPosition(int x, int y) {
	mBox.x = x;
	mBox.y = y;
}

void Character::saveState() {
	if (mCharState == STANDING) {
		ofstream save("resources/characters/save.txt", std::ios::out | std::ios::trunc);
		save << currentLevel->getLevelNumber();
		save << ' ';
		save << currentWorldIndex;
		save << ' ';
		save << mBox.x;
		save << ' ';
		save << mBox.y;
		save << '\n';
	}
}

void Character::setAlpha(int alpha) {
	for (int i = 0; i < int(mTextures.size()); i++) {
		mTextures[i]->setAlpha(alpha);
	}
}

SDL_Rect Character::getBox() {
	return mBox;
}

bool Character::checkForOverlap(int worldIndex) 
{
	World* world  = currentLevel->getWorld(worldIndex);
	if (world != NULL) {
		TileSet* tileSet = world->getTileSet();
		vector<Tile*> overlapTiles;
		tileSet->findOverlapTiles(mBox, overlapTiles);

		bool solidTile = false;
		for (int i = 0; i < int(overlapTiles.size()); i++) {
			if (overlapTiles[i]->getType() == 1) {
				solidTile = true;
				break;
			}
		}

		if (solidTile) {
			return true;
		}
	}
	else {
		return true;
	}

	return false;
}

SDL_Point Character::getPosition() {
	SDL_Point charPos;
	charPos.x = mBox.x;
	charPos.y = mBox.y;
	return charPos;
}

void Character::updateCharSpeed() 
{
	float threshold = 0.3f;

	float acceleration = aX;
	if (mCharState == JUMPING || mCharState == FALLING) {
		if (abs(mVelX) > 0.0) {
			acceleration = jumpX;
		}
	}
	
	mVelX = acceleration * mTargetSpeedX + (1 - acceleration) * mVelX;
	if (abs(mVelX) <= threshold) {
		mVelX = 0.0f;
	}
}

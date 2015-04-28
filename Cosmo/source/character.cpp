#include "character.h"

#include <algorithm>
#include <fstream>

#include "common.h"
#include "doorManager.h"
#include "level.h"
#include "loadManager.h"
#include "objectManager.h"
#include "platformset.h"
#include "soundManager.h"
#include "tileset.h"
#include "world.h"

const float JUMP_FORCE = 13.5f;
const float CHAR_VEL = 5.0;
const float CLIMB_VEL = 3.0f;
const float CHAR_ACC = 0.5f;
const float JUMP_ACC = 0.05f;

const int CHAR_WIDTH = 32;
const int CHAR_HEIGHT = 96;

int prevWorldIndex = 1;

Character::Character(int type) 
{
	mType = type;

	mBox.x = 100;
	mBox.y = 0;
	mBox.w = CHAR_WIDTH;
	mBox.h = CHAR_HEIGHT;

	mVelX = 0.0f;
	mVelY = 0.0f;
	mTargetSpeedX = 0.0f;

	mFrame = 0;

	mLadderOverlap = false;
	mLadderBelow = false;
	
	mPrevState = 0;

	mIdleJump = false;
	mInAir = false;

	mCurrentPlatform = NULL;
	currentDirection = RIGHT;
}


bool Character::loadMedia() {
	mTextures = loadManager.getSprite(mType);
	if (mTextures.empty()) {
		return false;
	}
	return true;
}


void Character::handleEvent(SDL_Event& e)
{
	if (currentWorldIndex != prevWorldIndex) {
		mCurrentPlatform = NULL;
	}

	if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
		switch (e.key.keysym.sym)
		{
		case SDLK_UP:
			objectManager.interact(mBox, currentLevel->getLevelNumber(), currentWorldIndex, true);
			doorManager.interact(mBox, currentLevel->getLevelNumber(), currentWorldIndex, true);
			break;
		case SDLK_SPACE:
			if (mCurrentState == STANDING || mCurrentState == CLIMBING) {
				mVelY = -1 * JUMP_FORCE;
				mCurrentState = JUMPING;
				soundManager.playSound(0);
			}
			break;
		}
	}

	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

	if (currentKeyStates[SDL_SCANCODE_UP]) {
		if (mCurrentState != CLIMBING && mLadderOverlap) {
			mCurrentState = CLIMBING;
			mVelY = 0.0f;
		}
		if (mCurrentState == CLIMBING) {
			mVelY = -CLIMB_VEL;
		}
	}
	else if (currentKeyStates[SDL_SCANCODE_DOWN]) {
		if (mCurrentState != CLIMBING && (mLadderBelow)) {
			mCurrentState = CLIMBING;
			mVelY = 0.0f;
		}
		if (mCurrentState == CLIMBING) {
			mVelY = CLIMB_VEL;
		}
	}
	else {
		if (mCurrentState == CLIMBING) {
			mVelY = 0.0f;
		}
	}

	if (currentKeyStates[SDL_SCANCODE_LEFT]) {
		currentDirection = LEFT;
		mTargetSpeedX = -1 * CHAR_VEL;
		if (mCurrentState == CLIMBING) {
			mTargetSpeedX = -CLIMB_VEL;
		}
	}
	else if (currentKeyStates[SDL_SCANCODE_RIGHT]) {
		currentDirection = RIGHT;
		mTargetSpeedX = CHAR_VEL;
		if (mCurrentState == CLIMBING) {
			mTargetSpeedX = CLIMB_VEL;
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

	PlatformSet* currentPlatforms = currentWorld->getPlatformSet();
	TileSet* currentTileSet = currentWorld->getTileSet();
	
	mLadderBelow = false;
	mLadderOverlap = false;

	//apply gravity
	if (mCurrentState != CLIMBING) {
		if (mVelY < TERMINAL) {
			mVelY += GRAVITY;
		}
		else {
			mVelY = TERMINAL;
		}
	}

	//break velocity into direction and magnitude
	int dirVelX = mVelX < 0 ? LEFT : mVelX > 0 ? RIGHT : NONE;
	int dirVelY = mVelY < 0 ? UP : mVelY > 0 ? DOWN : NONE;

	int magVelX = int(std::abs(mVelX) + .05);
	int magVelY = int(std::abs(mVelY) + .05);


	//**************************************** HORIZONTAL MOVEMENT ****************************************//

	int deltaX = magVelX;
	
	//what type of collision? currently unused
	int typeX = NONE;
	int distanceX = magVelX;

	//updates distanceX
	if (currentTileSet->findStaticCollisions(mBox, dirVelX, distanceX, typeX)) {
		deltaX = distanceX;
	}

	deltaX = objectManager.findStaticCollisions(mBox, currentLevel->getLevelNumber(), currentWorldIndex, deltaX, dirVelX);

	if (dirVelX == LEFT) {
		mBox.x -= deltaX;
	}
	else if (dirVelX == RIGHT) {
		mBox.x += deltaX;
	}
	
	//keep character in level bounds 
	if (mBox.x < 0) {
		mBox.x = 0;
	}
	if (mBox.x + mBox.w > currentLevel->getWidth()) {
		mBox.x = currentLevel->getWidth() - mBox.w;
	}

	//**************************************** VERTCIAL MOVEMENT ****************************************//
	int deltaY = magVelY;
	
	if (mCurrentState != CLIMBING) {
		if (dirVelY == UP && deltaY >= 0) {
			mCurrentState = JUMPING;
		}
		else if ((dirVelY == DOWN && deltaY >= 0) || dirVelY == NONE) {
			mCurrentState = FALLING;
		}
		else {
			mCurrentState = STANDING;
		}
	}

	if (mCurrentState != CLIMBING) {

		//what type of collision? currently unused
		int typeY = NONE;
		int distanceY = magVelY;
		
		//updates distanceY
		if (currentTileSet->findStaticCollisions(mBox, dirVelY, distanceY, typeY)) {
			deltaY = distanceY;
			if (dirVelY == DOWN) {
				mCurrentState = STANDING;
			}
			if (dirVelY == UP) {
				mVelY = 0;
			}
		}

		//updates distanceY
		mCurrentPlatform = currentPlatforms->findPlatformCollisions(mBox, dirVelY, distanceY);
		
		if (mCurrentPlatform != NULL) {
			deltaY = distanceY;
			if (dirVelY == DOWN) {
				mCurrentState = STANDING;
			}
		}

		distanceY = objectManager.findStaticCollisions(mBox, currentLevel->getLevelNumber(), currentWorldIndex, deltaY, dirVelY);
		if (deltaY != distanceY && distanceY == 0) {
			deltaY = distanceY;
			mCurrentState = STANDING;
		}
	}
	
	if (mCurrentPlatform != NULL) {
		if (mCurrentState == STANDING) {
			mBox.y += mCurrentPlatform->getSpeed(VERTICAL);
		}
		if (mCurrentState != STANDING) {
			mCurrentPlatform = NULL;
		}
	}

	if (dirVelY == UP) {
		mBox.y -= deltaY;
	}
	else if (dirVelY == DOWN) {
		mBox.y += deltaY;
	}

	//keep character in level bounds, unused 
	/*if (mBox.y < 0) {
		mBox.y = 0;
	}
	if (mBox.y + mBox.h > currentLevel->getHeight()) {
		mBox.y = currentLevel->getHeight() - mBox.h;
	}*/

	checkForOverlaps();
	saveState();

	prevWorldIndex = currentWorldIndex;
}

void Character::updateCharSpeed()
{
	float threshold = 0.3f;

	float acceleration = CHAR_ACC;
	if (mCurrentState == JUMPING || mCurrentState == FALLING) {
		if (abs(mVelX) > 0.0) {
			acceleration = JUMP_ACC;
		}
	}

	float targetSpeed = mTargetSpeedX;
	if (mCurrentPlatform != NULL) {
		if (mCurrentState == STANDING) {
			targetSpeed += (float) mCurrentPlatform->getSpeed(HORIZONTAL);
		}
	}
	mVelX = acceleration * targetSpeed + (1 - acceleration) * mVelX;

	if (abs(mVelX) <= threshold) {
		mVelX = 0.0f;
	}
}

void Character::checkForOverlaps() 
{
	World* currentWorld = currentLevel->getWorld(currentWorldIndex);
	TileSet* currentTileSet = currentWorld->getTileSet();

	int dirVelY = mVelY < 0 ? UP : mVelY > 0 ? DOWN : NONE;

	vector<Tile*> overlapTiles;
	currentTileSet->findOverlapTiles(mBox, overlapTiles);

	for (int i = 0; i < int(overlapTiles.size()); i++) {
		Tile* currentTile = overlapTiles[i];
		if (currentTile->getType() == 30) {
			mLadderOverlap = true;
			if (mCurrentState == CLIMBING) {
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

	if (mCurrentState == CLIMBING && !mLadderOverlap) {
		mCurrentState = STANDING;
	}

	overlapTiles.clear();

	//check tiles immediately below character
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
		if (mCurrentState == CLIMBING && dirVelY == DOWN) {
			mCurrentState = FALLING;
		}
	}

	overlapTiles.clear();
}


void Character::setCamera(SDL_Rect& camera)
{
	//center the camera over the character
	camera.x = (mBox.x + mBox.w / 2) - SCREEN_WIDTH / 2;
	camera.y = (mBox.y + mBox.h / 2) - SCREEN_HEIGHT / 2;

	//keep the camera in bounds
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
	if (mFrame >= INT_MAX || mPrevState != mCurrentState) {
		mFrame = 0;
	}

	int x = mBox.x - camera.x;
	int y = mBox.y - camera.y;

	int currentSprite = 0;
	int spriteIndex = 0;

	int speed = 4;
	bool updateFrame = false;

	if (mCurrentState != JUMPING) {
		mIdleJump = false;
	}

	if (mCurrentState == JUMPING) {
		mInAir = true;
		currentSprite = JUMP;
		if (!mIdleJump && sheetComplete(currentSprite, speed)) {
			mIdleJump = true;
		}
		if (mIdleJump) {
			currentSprite = IDLE;
		}
		updateFrame = true;
	}

	if (mCurrentState == STANDING) {
		if (mInAir) {
			currentSprite = LAND;
			if (sheetComplete(currentSprite, speed)) {
				mInAir = false;
			}
			updateFrame = true;
		}
		else {
			currentSprite = WALK;
			if (mCurrentPlatform != NULL) {
				if (mVelX != mCurrentPlatform->getSpeed(0)) {
					speed = 5;
					updateFrame = true;
				}
			}
			else if (mVelX != 0) {
				speed = 5;
				updateFrame = true;
			}
		}
	}

	if (mCurrentState == CLIMBING) {
		currentSprite = CLIMB;
		if (mVelY != 0) {
			speed = 8;
			updateFrame = true;
		}
	}

	if (mCurrentState == FALLING) {
		mInAir = true;
		speed = 6;
		currentSprite = FALL;
		updateFrame = true;
	}

	if (updateFrame) {
		spriteIndex = mFrame / speed % mTextures[currentSprite]->getSpriteCount();
	}
	
	bool flip = false;
	if (currentDirection == LEFT) {
		flip = true;
	}

	mTextures[currentSprite]->renderSprite(x, y, spriteIndex, flip);
	mFrame++;

	mPrevState = mCurrentState;
}


bool Character::sheetComplete(int currentSprite, int speed) {
	return mFrame / speed >= mTextures[currentSprite]->getSpriteCount();
}

void Character::setPosition(int x, int y) {
	mBox.x = x;
	mBox.y = y;
}

bool Character::loadState() 
{
	bool success = true;

	ifstream load("resources/characters/save.txt");
	
	if (!load) {
		printf("Unable to load character position!\n");
		success = false;
	}
	else {
		int levelIndex, currentWorld, x, y;
		load >> levelIndex;
		load >> currentWorld;
		load >> x;
		load >> y;
	
		if (levelIndex != currentLevel->getLevelNumber()) {
			SDL_Point charPosition = currentLevel->getCharacterStart();
			mBox.x = charPosition.x;
			mBox.y = charPosition.y;
		}
		else {
			currentWorldIndex = currentWorld;
			mBox.x = x;
			mBox.y = y;
		}
	}
	return success;
}

bool Character::saveState() 
{
	bool success = true;

	if (mCurrentState == STANDING && mCurrentPlatform == NULL) {
		ofstream save("resources/characters/save.txt", std::ios::out | std::ios::trunc);

		if (!save) {
			printf("Unable to save character state!\n");
			success = false;
		}
		else {
			save << currentLevel->getLevelNumber() << ' ';
			save << currentWorldIndex << ' ';
			save << mBox.x << ' ';
			save << mBox.y << '\n';
		}
	}
	return success;
}

void Character::setAlpha(int alpha) {
	for (int i = 0; i < int(mTextures.size()); i++) {
		mTextures[i]->setAlpha(alpha);
	}
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

SDL_Rect Character::getBox() {
	return mBox;
}

SDL_Point Character::getPosition() {
	SDL_Point charPos;
	charPos.x = mBox.x;
	charPos.y = mBox.y;
	return charPos;
}

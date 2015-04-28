#include "object.h"

Object::Object(string id, int type, Texture* texture, int x, int y) 
{
	mType = type;
	mTexture = texture;

	mId = id;
	
	mBox.x = x;
	mBox.y = y;
	mBox.w = texture->getWidth() / texture->getSpriteCount();
	mBox.h = texture->getWidth() / texture->getSpriteCount();

	mVisible = true;
	mFixed = false;
}

string Object::getId() {
	return mId;
}

int Object::getType() {
	return mType;
}

SDL_Rect Object::getBox() {
	return mBox;
}

void Object::renderSprite(int x, int y) 
{
	if (!mVisible) {
		return;
	}

	if (mFrames >= INT_MAX) {
		mFrames = 0;
	}

	int currentSprite = mFrames / 8 % mTexture->getSpriteCount();
	mTexture->renderSprite(mBox.x + x, mBox.y + y, currentSprite, false);
	
	mFrames++;
}

void Object::addInteraction(string interaction) {
	mInteractions.push_back(interaction);
}

vector<string> Object::getInteractions() {
	return mInteractions;
}

bool Object::overlap(SDL_Rect box) 
{
	if (!mVisible) {
		return false;
	}

	int leftA = mBox.x;
	int rightA = mBox.x + mBox.w;
	int topA = mBox.y;
	int bottomA = mBox.y + mBox.h;

	int leftB = box.x;
	int rightB = box.x + box.w;
	int topB = box.y;
	int bottomB = box.y + box.h;

	if (bottomA <= topB) {
		return false;
	}

	if (topA >= bottomB) {
		return false;
	}

	if (rightA <= leftB) {
		return false;
	}

	if (leftA >= rightB) {
		return false;
	}

	return true;
}

void Object::setVisible(bool visible) {
	mVisible = visible;
}

void Object::toggleVisible() {
	if (mVisible) {
		mVisible = false;
	}
	else {
		mVisible = true;
	}
}

void Object::setFixed(bool fixed) {
	mFixed = fixed;
}

bool Object::isFixed() {
	return mFixed;
}

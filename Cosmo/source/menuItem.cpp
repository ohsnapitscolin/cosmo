#include "menuItem.h"

MenuItem::MenuItem(int x, int y, std::string text, SDL_Color color, SDL_Color hoverColor)
{
	mText = text;
	mColor = color;
	mHoverColor = hoverColor;

	mItemBox.x = x;
	mItemBox.y = y;

	mHover = false;
}

MenuItem::~MenuItem() {
	void free();
}

bool MenuItem::loadText()
{
	bool success = true;

	if (!mNormalTexture.loadFromRenderedText(gFont, mText, mColor)) {
		printf("Failed to load menu item: %s!\n", mText.c_str());
		success = false;
	}
	else {
		mItemBox.w = mNormalTexture.getWidth();
		mItemBox.h = mNormalTexture.getHeight();
	}

	if (!mHoverTexture.loadFromRenderedText(gFont, mText, mHoverColor)) {
		printf("Failed to load menu item: %s!\n", mText.c_str());
		success = false;
	}

	return success;
}

bool MenuItem::updateText(string text) {
	mText = text;
	return loadText();
}

SDL_Rect MenuItem::getBox() {
	return mItemBox;
}

void MenuItem::render()
{
	if (mHover) {
		mHoverTexture.render(mItemBox.x, mItemBox.y);
	}
	else {
		mNormalTexture.render(mItemBox.x, mItemBox.y);
	}
}

void MenuItem::render(int x, int y)
{
	mItemBox.x = x;
	mItemBox.y = y;

	if (mHover) {
		mHoverTexture.render(mItemBox.x, mItemBox.y);
	}
	else {
		mNormalTexture.render(mItemBox.x, mItemBox.y);
	}
}

bool MenuItem::hover(int x, int y)
{
	if (x >= mItemBox.x && x <= mItemBox.x + mItemBox.w &&
		y >= mItemBox.y && y <= mItemBox.y + mItemBox.h) {
		return true;
	}
	return false;
}

void MenuItem::setHover(bool hover) {
	mHover = hover;
}

void MenuItem::free() 
{
	mNormalTexture.free();
	mHoverTexture.free();
}
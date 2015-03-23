#include "menuItem.h"

MenuItem::MenuItem(int x, int y, std::string text, SDL_Color color) {
	mText = text;
	mColor = color;
	mItemBox.x = x;
	mItemBox.y = y;
	mItemBox.w = 0;
	mItemBox.h = 0;

	mHover = false;
}

bool MenuItem::loadText() {
	bool success = true;

	if (!mNormalTexture.loadFromRenderedText(mText, mColor)) {
		printf("Failed to load menu item: %s!\n", mText.c_str());
		success = false;
	}

	SDL_Color hoverColor = { 175, 175, 175 };
	if (!mHoverTexture.loadFromRenderedText(mText, hoverColor)) {
		printf("Failed to load menu item: %s!\n", mText.c_str());
		success = false;
	}
	else {
		mItemBox.w = mNormalTexture.getWidth();
		mItemBox.h = mNormalTexture.getHeight();
	}
	return success;
}

bool MenuItem::updateText(std::string text) {
	mText = text;
	return loadText();
}

bool MenuItem::updateColor(SDL_Color color) {
	mColor = color;
	return loadText();
}

SDL_Rect MenuItem::getBox() {
	return mItemBox;
}

void MenuItem::render() {
	if (mHover) {
		mHoverTexture.render(mItemBox.x, mItemBox.y);
	}
	else {
		mNormalTexture.render(mItemBox.x, mItemBox.y);
	}
}

void MenuItem::render(int x, int y) {
	mItemBox.x = x;
	mItemBox.y = y;
	mNormalTexture.render(mItemBox.x, mItemBox.y);
}

bool MenuItem::hover(int x, int y) {
	if (x >= mItemBox.x && x <= mItemBox.x + mItemBox.w &&
		y >= mItemBox.y && y <= mItemBox.y + mItemBox.h) {
		return true;
	}
	return false;
}

void MenuItem::setHover(bool hover) {
	mHover = hover;
}
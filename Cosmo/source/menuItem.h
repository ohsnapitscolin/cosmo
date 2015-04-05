#ifndef MENUITEM_H_DEFINED
#define MENUITEM_H_DEFINED

#include "common.h"
#include "texture.h"
#include <string>

class MenuItem
{
public:
	MenuItem(int x, int y, std::string text, SDL_Color color);
	
	~MenuItem();
	void free();

	bool loadText();
	bool updateText(std::string text);
	bool updateColor(SDL_Color color);

	void render();
	void render(int x, int y);
	bool hover(int x, int y);

	void setHover(bool hover);

	SDL_Rect getBox();

private:
	SDL_Rect mItemBox;
	LTexture mNormalTexture;
	LTexture mHoverTexture;
	std::string mText;
	SDL_Color mColor;

	bool mHover;
};

#endif
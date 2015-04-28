#ifndef MENUITEM_H_DEFINED
#define MENUITEM_H_DEFINED

#include "common.h"
#include "texture.h"

class MenuItem
{
public:
	MenuItem(int x, int y, string text, SDL_Color color, SDL_Color hoverColor);
	
	~MenuItem();
	void free();

	bool loadText();
	bool updateText(string text);

	void render();
	void render(int x, int y);

	bool hover(int x, int y);
	void setHover(bool hover);

	SDL_Rect getBox();

private:
	Texture mNormalTexture;
	Texture mHoverTexture;

	SDL_Rect mItemBox;
	
	string mText;

	SDL_Color mColor;
	SDL_Color mHoverColor;

	bool mHover;
};

#endif
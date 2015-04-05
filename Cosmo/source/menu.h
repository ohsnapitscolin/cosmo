#ifndef MENU_H_DEFINED
#define MENU_H_DEFINED

#include <SDL.h>
#include <vector>
#include "texture.h"
#include "menuItem.h"

class Menu
{
public:
	Menu(std::string textureName);
	~Menu();
	void free();

	bool loadMedia();
	int start();

private:
	LTexture mMenuTexture;
	std::vector<MenuItem*> mMenuItems;

	std::string mTextureName;
};

#endif
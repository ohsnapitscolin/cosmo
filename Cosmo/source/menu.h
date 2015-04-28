#ifndef MENU_H_DEFINED
#define MENU_H_DEFINED

#include "texture.h"
#include "menuItem.h"

class Menu
{
public:
	Menu(std::string textureName);
	
	~Menu();
	void free();

	bool loadMedia();
	
	int run();

private:
	string mTextureName;
	Texture mMenuTexture;

	vector<MenuItem*> mMenuItems;
};

#endif
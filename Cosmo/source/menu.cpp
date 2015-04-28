#include "menu.h"

Menu::Menu(std::string textureName) 
{
	SDL_Color textColor = { 255, 255, 255 };
	SDL_Color hoverColor = { 175, 175, 175 };
	
	int startX = SCREEN_WIDTH / 2 - 400;
	int startY = SCREEN_HEIGHT / 2 - 50;
	mMenuItems.push_back(new MenuItem(startX, startY, "Continue", textColor, hoverColor));
	mMenuItems.push_back(new MenuItem(startX, startY + 100, "Exit", textColor, hoverColor));
	mTextureName = textureName;
}

Menu::~Menu() {
	void free();
}

bool Menu::loadMedia() 
{
	bool success = true;
	for (int i = 0; i < int(mMenuItems.size()); i++) {
		success &= mMenuItems[i]->loadText();
	}

	if (!mMenuTexture.loadFromFile(mTextureName)) {
		printf("Unable to load menu texture!\n");
		success = false;
	}
	
	return success;
}

int Menu::run() 
{
	SDL_ShowCursor(SDL_ENABLE);

	SDL_Event e;
	bool quit = false;
	
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}

			int x, y;
			SDL_GetMouseState(&x, &y);

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				for (int i = 0; i < int(mMenuItems.size()); i++) {
					if (mMenuItems[i]->hover(x, y)) {
						return i;
					}
				}
			}

			for (int i = 0; i < int(mMenuItems.size()); i++) {
				if (mMenuItems[i]->hover(x, y)) {
					mMenuItems[i]->setHover(true);
				}
				else {
					mMenuItems[i]->setHover(false);
				}
			}
		}

		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		int centerX = SCREEN_WIDTH / 2 - mMenuTexture.getWidth() / 2;
		int centerY = SCREEN_HEIGHT / 2 - mMenuTexture.getHeight() / 2;
		mMenuTexture.render(centerX, centerY);

		for (int i = 0; i < int(mMenuItems.size()); i++) {
			mMenuItems[i]->render();
		}
		
		SDL_RenderPresent(gRenderer);
	}

	return -1;
}

void Menu::free() 
{
	for (int i = 0; i < int(mMenuItems.size()); i++) {
		if (mMenuItems[i] != NULL) {
			mMenuItems[i]->free();
			delete mMenuItems[i];
			mMenuItems[i] = NULL;
		}
	}
}
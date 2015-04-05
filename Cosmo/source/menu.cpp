#include "menu.h"

Menu::Menu(std::string textureName) {
	SDL_Color textColor = { 50, 50, 50 };
	mMenuItems.push_back(new MenuItem(500, 500, "Continue", textColor));
	mMenuItems.push_back(new MenuItem(500, 600, "Save", textColor));
	mMenuItems.push_back(new MenuItem(500, 700, "Exit", textColor));
	mTextureName = textureName;
}

Menu::~Menu() {
	void free();
}

bool Menu::loadMedia() {
	bool success = true;
	for (int i = 0; i < int(mMenuItems.size()); i++) {
		success &= mMenuItems[i]->loadText();
	}

	if (!mMenuTexture.loadFromFile(mTextureName)) {
		printf("Unable to load texture!\n");
		success = false;
	}
	return success;
}

int Menu::start() {

	SDL_ShowCursor(SDL_ENABLE);

	SDL_Event e;
	bool quit = false;
	
	int x, y;

	while (!quit) {

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			if (e.type == SDL_MOUSEBUTTONDOWN) {
				for (int i = 0; i < int(mMenuItems.size()); i++) {
					if (mMenuItems[i]->hover(x, y)) {
						return i;
					}
				}
			}
			SDL_GetMouseState(&x, &y);
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

		mMenuTexture.render(0, 0);
		for (int i = 0; i < int(mMenuItems.size()); i++) {
			mMenuItems[i]->render();
		}
		
		SDL_RenderPresent(gRenderer);
	}

	return -1;
}

void Menu::free() {
	for (int i = 0; i < int(mMenuItems.size()); i++) {
		if (mMenuItems[i] != NULL) {
			mMenuItems[i]->free();
			delete mMenuItems[i];
			mMenuItems[i] = NULL;
		}
	}
}
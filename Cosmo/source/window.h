#ifndef WINDOW_H_DEFINED
#define WINDOW_H_DEFINED

#include "common.h"

class Window
{
public:
	Window();

	void free();

	bool init();

	SDL_Renderer* createRenderer();

	void handleEvent(SDL_Event& e);
	
	int getWidth();
	int getHeight();

	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimized();

	SDL_Window* getWindow();

private:
	SDL_Window* mWindow;

	int mWidth;
	int mHeight;

	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimized;
};

#endif
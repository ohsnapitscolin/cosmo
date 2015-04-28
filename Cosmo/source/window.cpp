#include "window.h"
#include "soundManager.h"

Window::Window()
{
	mWindow = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimized = false;
	mWidth = 0;
	mHeight = 0;
}

SDL_Window* Window::getWindow() {
	return mWindow;
}

bool Window::init()
{
	SDL_DisplayMode mDesktop;
	
	if (SDL_GetDesktopDisplayMode(0, &mDesktop) != 0) {
		printf("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
		return 1;
	}

	bool windowed = true;

	SCREEN_WIDTH = mDesktop.w;
	SCREEN_HEIGHT = mDesktop.h;

	if (mDesktop.w >= 2000 || mDesktop.h >= 1600 || windowed == true) {
		bool windowed = true;
		SCREEN_WIDTH = 1600;
		SCREEN_HEIGHT = 900;
	}

	if (mDesktop.w < 1024 || mDesktop.h < 756) {
		printf("Unable to support screen resolution!\n");
	}
	
	if (windowed) {
		mWindow = SDL_CreateWindow("Cosmo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	}
	else {
		mWindow = SDL_CreateWindow("Cosmo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
	}

	if (mWindow != NULL) {
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = SCREEN_WIDTH;
		mHeight = SCREEN_HEIGHT; 		
	}

	return mWindow != NULL;
}

SDL_Renderer* Window::createRenderer() {
	return SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
}

void Window::handleEvent( SDL_Event& e )
{
	if( e.type == SDL_WINDOWEVENT ) {

		switch( e.window.event ) {
		//repaint on exposure
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent( gRenderer );
			break;
		//mouse entered window
		case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			break;
		//mouse left window
		case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			break;
		//window has keyboard focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			soundManager.resumeMusic();
			mKeyboardFocus = true;
			break;
		//window lost keyboard focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			soundManager.pauseMusic();
			mKeyboardFocus = false;
			break;
		//window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			soundManager.pauseMusic();
            mMinimized = true;
            break;
		//window maxized
			case SDL_WINDOWEVENT_MAXIMIZED:
			soundManager.resumeMusic();
			mMinimized = false;
            break;
		//window restored
			case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
            break;
		}
	}
}

void Window::free()
{
	if( mWindow != NULL ) {
		SDL_DestroyWindow( mWindow );
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int Window::getWidth() {
	return mWidth;
}

int Window::getHeight() {
	return mHeight;
}

bool Window::hasMouseFocus() {
	return mMouseFocus;
}

bool Window::hasKeyboardFocus() {
	return mKeyboardFocus;
}

bool Window::isMinimized() {
	return mMinimized;
}

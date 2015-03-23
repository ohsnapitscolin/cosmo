#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include "soundManager.h"
#include "window.h"
#include "common.h"

LWindow::LWindow()
{
	//Initialize non-existant window
	mWindow = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimized = false;
	mWidth = 0;
	mHeight = 0;
}

SDL_Window* LWindow::getWindow() {
	return mWindow;
}

bool LWindow::init()
{
	SDL_DisplayMode mDesktop;
	if (SDL_GetDesktopDisplayMode(0, &mDesktop) != 0) {
		SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
		return 1;
	}

	bool windowed = false;

	int windowWidth = 1200;
	int windowHeight = 900;

	SCREEN_WIDTH = mDesktop.w;
	SCREEN_HEIGHT = mDesktop.h;

	printf("SCREEN_WIDTH %d\n", SCREEN_WIDTH);
	printf("SCREEN_HEIGHT %d\n", SCREEN_HEIGHT);
	
	/*if (mDesktop.w >= mDesktop.h) {
		if (mDesktop.w > maxX || mDesktop.h > maxY) {
			SCREEN_WIDTH /= 2;
			SCREEN_HEIGHT /= 2;
		}
	}
	else {
		if (mDesktop.w > maxY || mDesktop.h > maxX) {
			SCREEN_WIDTH /= 2;
			SCREEN_HEIGHT /= 2;
		}
	}*/
	
	/*if (mDesktop.w > mDesktop.h) {
		if (SCREEN_WIDTH >= 1920) {
			SCREEN_WIDTH = 1920;
			SCREEN_HEIGHT = 900;
		}
		else if (SCREEN_WIDTH >= 1600) {
			SCREEN_WIDTH = 1600;
			SCREEN_HEIGHT = 900;
		}
		else {
			SCREEN_WIDTH = 1024;
			SCREEN_HEIGHT = 768;
		}
	}*/

	if (windowed) {
		mWindow = SDL_CreateWindow("Cosmo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
		SCREEN_WIDTH = windowWidth;
		SCREEN_HEIGHT = windowHeight;
	}
	else {
		mWindow = SDL_CreateWindow("Cosmo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
	}

	//Create window
	if (mWindow != NULL)
	{
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = SCREEN_WIDTH;
		mHeight = SCREEN_HEIGHT; 		
	}

	return mWindow != NULL;
}

SDL_Renderer* LWindow::createRenderer()
{
	return SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
}

void LWindow::handleEvent( SDL_Event& e )
{
	//Window event occured
	if( e.type == SDL_WINDOWEVENT )
	{
		//Caption update flag
		bool updateCaption = false;

		switch( e.window.event )
		{
			//Get new dimensions and repaint on window size change
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			SDL_RenderPresent( gRenderer );
			break;

			//Repaint on exposure
			case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent( gRenderer );
			break;

			//Mouse entered window
			case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			updateCaption = true;
			break;
			
			//Mouse left window
			case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			updateCaption = true;
			break;

			//Window has keyboard focus
			case SDL_WINDOWEVENT_FOCUS_GAINED:
			soundManager.resumeMusic();
			mKeyboardFocus = true;
			updateCaption = true;
			break;

			//Window lost keyboard focus
			case SDL_WINDOWEVENT_FOCUS_LOST:
			soundManager.pauseMusic();
			mKeyboardFocus = false;
			updateCaption = true;
			break;

			//Window minimized
			case SDL_WINDOWEVENT_MINIMIZED:
            mMinimized = true;
            break;

			//Window maxized
			case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimized = false;
            break;
			
			//Window restored
			case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
            break;
		}

		//Update window caption with new data
		if( updateCaption )
		{
			std::stringstream caption;
			caption << "SDL Tutorial - MouseFocus:" << ( ( mMouseFocus ) ? "On" : "Off" ) << " KeyboardFocus:" << ( ( mKeyboardFocus ) ? "On" : "Off" );
			SDL_SetWindowTitle( mWindow, caption.str().c_str() );
		}
	}
	//Enter exit full screen on return key
	else if( e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN )
	{
		if( mFullScreen )
		{
			SDL_SetWindowFullscreen( mWindow, SDL_FALSE );
			mFullScreen = false;
		}
		else
		{
			SDL_SetWindowFullscreen( mWindow, SDL_TRUE );
			mFullScreen = true;
			mMinimized = false;
		}
	}
}

void LWindow::free()
{
	if( mWindow != NULL )
	{
		SDL_DestroyWindow( mWindow );
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int LWindow::getWidth()
{
	return mWidth;
}

int LWindow::getHeight()
{
	return mHeight;
}

bool LWindow::hasMouseFocus()
{
	return mMouseFocus;
}

bool LWindow::hasKeyboardFocus()
{
	return mKeyboardFocus;
}

bool LWindow::isMinimized()
{
	return mMinimized;
}
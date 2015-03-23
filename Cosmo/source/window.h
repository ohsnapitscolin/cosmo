#ifndef WINDOW_H_DEFINED
#define WINDOW_H_DEFINED

class LWindow
{
	public:
		//Intializes internals
		LWindow();

		//Creates window
		bool init();

		//Creates renderer from internal window
		SDL_Renderer* createRenderer();

		//Handles window events
		void handleEvent( SDL_Event& e );
	
		//Deallocates internals
		void free();

		//Window dimensions
		int getWidth();
		int getHeight();

		//Window focii
		bool hasMouseFocus();
		bool hasKeyboardFocus();
		bool isMinimized();

		SDL_Window* getWindow();

	private:
		//Window data
		SDL_Window* mWindow;

		//Window dimensions
		int mWidth;
		int mHeight;

		//Window focus
		bool mMouseFocus;
		bool mKeyboardFocus;
		bool mFullScreen;
		bool mMinimized;
};

#endif
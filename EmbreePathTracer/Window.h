#pragma once
#include <SDL\SDL.h>
#include <cstdio>
#include "PathTracer.h"

// class that handles the SDL drawing and input

namespace path_tracer
{
	class Window
	{
	public:
		// static factory constructor method
		static Window* createWindow(const char* title = "Embree Intersector", const int screen_width = 640, const int screen_height = 480);
		
		// destructor
		~Window();

		// entry method to start rendering
		// calls methods for input handling, rendering, etc
		void startRender();

		inline void setIntersector(PathTracer* pt) { this->pt = pt; }
		inline PathTracer* getIntersector() { return pt; }


	private:
		// constructor
		Window(const char* title, const int screen_width, const int screen_height, SDL_Window* sdl_win) : 
			title(title), screen_width(screen_width), screen_height(screen_height), sdl_win(sdl_win) {}

		// member variables
		PathTracer* pt;
		const char* title;
		const int screen_width;
		const int screen_height;
		SDL_Window* sdl_win;
		SDL_Surface* screen_surface;
		glm::dvec3* img_buffer;
		size_t cur_samples;
		bool quit;

		// methods

		// initialize arrays and more variables
		void init();

		// reset the image buffer
		void resetBuffer();

		// handles input from the user
		void handleInput();

		// renders a single image frame
		void renderFrame();
	};

}
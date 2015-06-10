#include "Window.h"


namespace path_tracer
{
	// initialize SDL
	inline int initSDL()
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0){
			fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
			return 1;
		}
		return 0;
	}

	// create window
	inline SDL_Window* createSDLWindow(const char* title, const int width, const int height)
	{
		SDL_Window *win = SDL_CreateWindow(title, 100, 100, width, height, SDL_WINDOW_SHOWN);
		if (win == nullptr){
			fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		}
		return win;
	}

	Window* Window::createWindow(const char* title, const int screen_width, const int screen_height)
	{
		if (initSDL() != 0) return nullptr;

		SDL_Window* sdl_win = createSDLWindow(title, screen_width, screen_height);
		if (sdl_win == nullptr)
		{
			SDL_Quit();
			return nullptr;
		}

		Window * window = new Window(title, screen_width, screen_height, sdl_win);
		window->init();
		return window;
	}

	Window::~Window()
	{
		delete img_buffer;
		SDL_DestroyWindow(sdl_win);
		SDL_Quit();
	}

	void Window::init()
	{
		screen_surface = SDL_GetWindowSurface(sdl_win);
		img_buffer = new glm::dvec3[screen_width * screen_height];
		resetBuffer();
	}

	void Window::resetBuffer()
	{
		int i = 0;
		glm::dvec3* cur_vec = img_buffer;
		while (i++ < screen_width * screen_height)
		{
			cur_vec->x = 0.0; cur_vec->y = 0.0; cur_vec->z = 0.0;
			cur_vec++;
		}
		cur_samples = 0;
	}

	void Window::startRender()
	{
		// check if an intersector exists
		if (pt == nullptr)
		{
			fprintf(stderr, "No intersector was assigned to the window. Closing...");
			return;
		}

		//test
		fprintf(stderr, "bits per pixel: %d", screen_surface->format->BitsPerPixel);

		// main rendering loop
		while (true)
		{
			handleInput();
			if (quit) break;
			renderFrame();
		}

		fprintf(stdout, "Exiting...");
	}

	void Window::handleInput()
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
				break;
			}
			//TODO: handle more events
		}
	}

	void Window::renderFrame()
	{
		pt->render(img_buffer);
		cur_samples++;
		SDL_LockSurface(screen_surface);
		glm::dvec3 *cur_color = &img_buffer[0];
		for (int y = 0; y < screen_height; y++)
		{
			for (int x = 0; x < screen_width; x++)
			{
				Uint32 color = SDL_MapRGB(screen_surface->format, cur_color->x / cur_samples, cur_color->y / cur_samples, cur_color->z / cur_samples);
				Uint8* cur_pixel = (Uint8*)screen_surface->pixels + y * screen_surface->pitch + x * screen_surface->format->BytesPerPixel;
				*(Uint32*)cur_pixel = color;
				cur_color++;
			}
		}
		SDL_UnlockSurface(screen_surface);
		SDL_UpdateWindowSurface(sdl_win);
	}
}
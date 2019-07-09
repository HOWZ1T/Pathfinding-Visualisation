#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>

#include "errors.h"

#define SCREEN_W 640
#define SCREEN_H 480
#define SCREEN_SCALE 1
#define SCREEN_NAME "Prototype"

// defining the singleton App struct and methods
void init(void);
void quit(void);
void handle_events(SDL_Event* event_ptr);

static struct {
	// defines attributes
	SDL_bool running;

	// screen struct containing window attributes
	struct {
		unsigned int width;
		unsigned int height;
		const char* name;
		SDL_Window* window;
		SDL_Renderer* renderer;
	} screen;

	// define methods
	void (*init)(void);
	void (*quit)(void);
	void (*handle_events)(SDL_Event* event_ptr);
} App = {
	SDL_FALSE,
	{
		SCREEN_SCALE*SCREEN_W,
		SCREEN_SCALE*SCREEN_H,
		SCREEN_NAME,
		NULL,
		NULL
	},
	init,
	quit,
	handle_events,
};

// implementing App methods
void init(void) {
	printf("initializing app...\n");
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL error -> %s\n", SDL_GetError());
		exit(INIT_ERROR);
	}

	// creating window and renderer
	unsigned int w = App.screen.width;
	unsigned int h = App.screen.height;

	const char* name = App.screen.name;

	App.screen.window = SDL_CreateWindow(name,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		w, h, 0);

	App.screen.renderer = SDL_CreateRenderer(App.screen.window, -1, 
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// checking that the window and renderer were actually created
	if (App.screen.window == NULL)
	{
		printf("SDLError -> %s\n", SDL_GetError());
		exit(INIT_WINDOW_ERROR);
	}

	if (App.screen.renderer == NULL)
	{
		printf("SDLError -> %s\n", SDL_GetError());
		exit(INIT_RENDERER_ERROR);
	}

	App.running = SDL_TRUE;
	printf("initialized.\n");
}

void quit(void) {
	printf("quiting app...\n");

	// freeing memory
	SDL_DestroyRenderer(App.screen.renderer);
	SDL_DestroyWindow(App.screen.window);

	App.screen.window = NULL;
	App.screen.renderer = NULL;

	SDL_Quit();
	App.running = SDL_FALSE;
}

void handle_events(SDL_Event* event_ptr)
{
	while (SDL_PollEvent(event_ptr))
	{
		switch ((*event_ptr).type)
		{
			case SDL_QUIT: {
				App.running = SDL_FALSE;
			} break;
		}
	}
}

int main(int argc, char* argv[])
{
	App.init();

	printf("app running...\n");
	SDL_Event event;

	SDL_SetRenderDrawColor(App.screen.renderer, 255, 0, 0, 0);
	while (App.running)
	{
		App.handle_events(&event);
		SDL_RenderClear(App.screen.renderer);

		// TODO
		SDL_RenderPresent(App.screen.renderer);
	}

	App.quit();
	printf("app closed.\n");

	return 0;
}
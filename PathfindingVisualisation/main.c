#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>

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

	// definition for a drawing surface
	struct {
		SDL_Surface* surface;
		SDL_Texture* texture;
		Uint32 rmask, gmask, bmask, amask;
		unsigned int width;
		unsigned int height;
	} canvas;

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
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	init,
	quit,
	handle_events,
};

// implementing App methods
void init(void) {
	printf("initializing app...\n");
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL error -> %s\n", SDL_GetError());
		exit(1);
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
		exit(2);
	}

	if (App.screen.renderer == NULL)
	{
		printf("SDLError -> %s\n", SDL_GetError());
		exit(3);
	}

	// creating the surface
	// setting up RGBA bitmasks based on SDL byteorder
	Uint32 rmask = 0x0, gmask = 0x0, bmask = 0x0, amask = 0x0;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	App.canvas.rmask = rmask;
	App.canvas.gmask = gmask;
	App.canvas.bmask = bmask;
	App.canvas.amask = amask;

	App.canvas.width = App.screen.width;
	App.canvas.height = App.screen.height;
	
	// parameters are as follows: flags, width, height, bit depth, rgba masks...
	App.canvas.surface = SDL_CreateRGBSurface(0, App.canvas.width, App.canvas.height, 32, rmask, gmask, bmask, amask);
	if (App.canvas.surface == NULL)
	{
		printf("SDL_Error -> %s\n", SDL_GetError());
		exit(4);
	}

	// creating the initial texture
	App.canvas.texture = SDL_CreateTextureFromSurface(App.screen.renderer, App.canvas.surface);
	if (App.canvas.texture == NULL)
	{
		printf("SDL_Error -> %s\n", SDL_GetError());
		exit(5);
	}

	App.running = SDL_TRUE;
	printf("initialized.\n");
}

void quit(void) {
	printf("quiting app...\n");

	// freeing memory
	SDL_FreeSurface(App.canvas.surface);
	SDL_DestroyTexture(App.canvas.texture);
	SDL_DestroyRenderer(App.screen.renderer);
	SDL_DestroyWindow(App.screen.window);

	App.canvas.surface = NULL;
	App.canvas.texture = NULL;
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
	while (App.running)
	{
		App.handle_events(&event);

		SDL_RenderClear(App.screen.renderer);

		// TODO: setup turtle as the character to pathfind
		// TODO: draw objects directly to the renderer

		// drawing surface
		SDL_FillRect(App.canvas.surface, NULL, SDL_MapRGBA(App.canvas.surface->format, 255, 100, 150, 255));
		
		// updating texture from the changed surface
		SDL_DestroyTexture(App.canvas.texture);
		App.canvas.texture = SDL_CreateTextureFromSurface(App.screen.renderer, App.canvas.surface);

		// renderering to the screen
		SDL_RenderCopy(App.screen.renderer, App.canvas.texture, NULL, NULL);
		SDL_RenderPresent(App.screen.renderer);
	}

	App.quit();
	printf("app closed.\n");

	return 0;
}
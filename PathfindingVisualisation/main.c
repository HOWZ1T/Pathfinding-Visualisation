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

// renders the play grid
void render_grid(SDL_Renderer* renderer, int line_thickness, int columns, int rows, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	int cell_width = App.screen.width / columns;
	int cell_height = App.screen.height / rows;

	// stores original drawing color
	Uint8 or , og, ob, oa;
	int result = SDL_GetRenderDrawColor(renderer, &or , &og, &ob, &oa);

	// setting up the draw color for the current drawing process
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	if (result != 0)
	{
		printf("SDL_Error -> %s\n", SDL_GetError());
		exit(ERROR);
	}

	for (unsigned int x = 0; x < App.screen.width; x++)
	{
		for (unsigned int y = 0; y < App.screen.height; y++)
		{
			if (x % cell_width == 0)
			{
				for (int i = 0; i < line_thickness; i++)
				{
					if ((x + i) >= App.screen.width) // ensures we don't try to draw over the screen
					{
						break;
					}

					int result = SDL_RenderDrawPoint(renderer, x + i, y);
					if (result != 0)
					{
						printf("SDL_Error -> %s\n", SDL_GetError());
						exit(ERROR);
					}
				}
			}

			if (y % cell_height == 0)
			{
				for (int i = 0; i < line_thickness; i++)
				{
					if ((y + i) >= App.screen.height) // ensures we don't try to draw over the screen
					{
						break;
					}

					int result = SDL_RenderDrawPoint(renderer, x, y + i);
					if (result != 0)
					{
						printf("SDL_Error -> %s\n", SDL_GetError());
						exit(ERROR);
					}
				}
			}
		}
	}

	// reset the renderer's draw color
	SDL_SetRenderDrawColor(renderer, or , og, ob, oa);
}

struct Color
{
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
};


struct {
	unsigned int columns;
	unsigned int rows;
	struct Color** cells;
} board = {
	5,
	10,
	NULL
};

int main(int argc, char* argv[])
{
	App.init();

	printf("app running...\n");
	SDL_Event event;

	SDL_SetRenderDrawColor(App.screen.renderer, 0, 0, 0, 255);

	// TODO fix the buffer overrun
	board.cells = malloc(sizeof(struct Color*) * board.columns);
	for (int i = 0; i < board.columns; i++)
	{
		board.cells[i] = malloc(sizeof(struct Color) * board.rows);
	}

	while (App.running)
	{
		App.handle_events(&event);
		SDL_RenderClear(App.screen.renderer);

		// TODO List:
		// render grid & cells of the board
		// abstract generic error handling code as it is repeated

		render_grid(App.screen.renderer, 10, 2, 2, 255, 255, 255, 255);

		SDL_RenderPresent(App.screen.renderer);
	}

	// TODO fix this free to actually free the multi dimensional data
	free(board.cells);

	App.quit();
	printf("app closed.\n");

	return 0;
}

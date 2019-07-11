#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <time.h>
#include <inttypes.h>

#include "errors.h"
#include "math_utils.h"

#define SCREEN_W 800
#define SCREEN_H 600
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

struct Color
{
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
};

struct Color WHITE = { 255, 255, 255, 255 };
struct Color BLACK = { 0, 0, 0, 255 };
struct Color RED = { 255, 0, 0, 255 };
struct Color GREEN = { 0, 255, 0, 255 };
struct Color BLUE = { 0, 0, 255, 255 };


struct {
	unsigned int columns;
	unsigned int rows;
	struct Color** cells;
} board = {
	4,
	4,
	NULL
};

// renders the play grid
void render_grid(SDL_Renderer* renderer, int line_thickness, struct Color color)
{
	int cell_width = App.screen.width / board.columns;
	int cell_height = App.screen.height / board.rows;

	// stores original drawing color
	Uint8 or , og, ob, oa;
	int result = SDL_GetRenderDrawColor(renderer, &or , &og, &ob, &oa);

	// setting up the draw color for the current drawing process
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	if (result != 0)
	{
		printf("SDL_Error -> %s\n", SDL_GetError());
		exit(ERROR);
	}

	for (unsigned int x = 0; x < App.screen.width; x += cell_width)
	{
		for (unsigned int y = 0; y < App.screen.height; y += cell_height)
		{
			// TODO handle thickness & centering
			for (int xi = 0; xi < cell_width; xi++)
			{
				int result = SDL_RenderDrawPoint(renderer, x + xi, y);
				if (result != 0)
				{
					printf("SDL_Error -> %s\n", SDL_GetError());
					exit(ERROR);
				}
			}

			for (int yi = 0; yi < cell_height; yi++)
			{
				int result = SDL_RenderDrawPoint(renderer, x, y + yi);
				if (result != 0)
				{
					printf("SDL_Error -> %s\n", SDL_GetError());
					exit(ERROR);
				}
			}
		}
	}

	// reset the renderer's draw color to the original draw color before calling this function
	SDL_SetRenderDrawColor(renderer, or , og, ob, oa);
}

void render_cells(SDL_Renderer* renderer)
{
	//int cell_width = App.screen.width / columns;
	//int cell_height = App.screen.height / rows;

	// stores original drawing color
	Uint8 or , og, ob, oa;
	int result = SDL_GetRenderDrawColor(renderer, &or , &og, &ob, &oa);

	if (result != 0)
	{
		printf("SDL_Error -> %s\n", SDL_GetError());
		exit(ERROR);
	}

	for (unsigned int x = 0; x < App.screen.width; x++)
	{
		for (unsigned int y = 0; y < App.screen.height; y++)
		{
			// map x y to board cell location
			int cell_x = imap(x, 0, App.screen.width, 0, board.columns);
			int cell_y = imap(y, 0, App.screen.height, 0, board.rows);

			struct Color* cell_color = &board.cells[cell_x][cell_y];
			if (cell_color != NULL)
			{
				// setting up the draw color for the current cell drawing process
				SDL_SetRenderDrawColor(renderer, cell_color->r, cell_color->g, cell_color->b, cell_color->a);
				int result = SDL_RenderDrawPoint(renderer, x, y);
				if (result != 0)
				{
					printf("SDL_Error -> %s\n", SDL_GetError());
					exit(ERROR);
				}
			}
		}
	}

	// reset the renderer's draw color
	SDL_SetRenderDrawColor(renderer, or , og, ob, oa);
}

int main(int argc, char* argv[])
{
	App.init();

	printf("app running...\n");
	SDL_Event event;

	SDL_SetRenderDrawColor(App.screen.renderer, 0, 0, 0, 255);

	// sets up randomisation based on time.
	// Avoid use for cryptography!
	srand(time(NULL)); // initialization. must only be called once!

	/* 
	- assigning memory to board.cells for storing the color of the cell in 2d space
	- notice: calloc handles overflow :D 
	- a way to check for overflow manually (if using malloc) 
	       is: if(x > T_MAX / y) where x, y are dimensions and T_MAX is type max.
	- note: always check that malloc / calloc actually succeeded!
	*/
	board.cells = calloc(board.columns, sizeof(struct Color*));
	if (!board.cells)
	{
		printf("Calloc failed to allocate memory for board.cells!\n");
		exit(MEMORY_ALLOCATION_ERROR);
	}
	for (unsigned int i = 0; i < board.columns; i++)
	{
		board.cells[i] = calloc(board.rows, sizeof(struct Color)); 
		if (!board.cells[i])
		{
			// freeing allocated memory prior to the failure
			for (unsigned int j = 0; j < i; j++)
			{
				free(board.cells[j]);
			}
			free(board.cells);

			printf("Calloc failed to allocate memory for board[%d].cells!\n", i);
			exit(MEMORY_ALLOCATION_ERROR);
		}
	}

	for (int x = 0; x < board.columns; x++)
	{
		for (int y = 0; y < board.rows; y++)
		{
			board.cells[x][y] = (struct Color){ rand() % 256, rand() % 256, rand() % 256, 255};
		}
	}

	while (App.running)
	{
		App.handle_events(&event);
		SDL_RenderClear(App.screen.renderer);

		// TODO List:
		// render grid
		// abstract generic error handling code as it is repeated
		// might have an memory leak ? 

		render_cells(App.screen.renderer);
		render_grid(App.screen.renderer, 3, RED);

		SDL_RenderPresent(App.screen.renderer);
	}

	// freeing 2d array
	for (unsigned int i = 0; i < board.columns; i++)
	{
		free(board.cells[i]);
	}
	free(board.cells);

	App.quit();
	printf("app closed.\n");

	return 0;
}

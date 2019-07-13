#include "maps.h"

// this function's code was sourced from: http://sdl.beuc.net/sdl.wiki/Pixel_Access
Uint32 get_pixel_32(SDL_Surface* surface, int x, int y)
{
	// TODO DEBUG
	int bpp = surface->format->BytesPerPixel;
	Uint8* pixel = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
		case 1:
			return *pixel;

		case 2:
			return *(Uint16*)pixel;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
			else
				return pixel[0] | pixel[1] << 8 | pixel[2] << 16;

		case 4:
			return *(Uint32*)pixel;

		default:
			return 0; /* shouldn't happen, but avoids warnings */
	}
}

SDL_Color convert_pixel_data_to_color(SDL_Surface* surface, Uint32 data)
{
	SDL_Color rgb;

	SDL_GetRGB(data, surface->format, &rgb.r, &rgb.g, &rgb.b);
	if (!&rgb)
	{
		fflush(stdout);
		printf("Pixel to Color Conversion Error!\n");
		exit(PIXEL_TO_COLOR_CONVERSION_ERROR);
	}

	return rgb;
}

// loads a map from the specified image filepath
// returns NULL on failure
struct Map* load_map_from_image(const char* filepath)
{
	int** cells;
	int w, h;
	int has_start = 0, has_end = 0;

	// reading image file
	SDL_Surface* surf = SDL_LoadBMP(filepath);
	if (!surf)
	{
		fflush(stdout);
		printf("SDL_Error -> %s\n", SDL_GetError());
		exit(LOAD_IMAGE_ERROR);
	}

	// allocating memory for map
	w = surf->w;
	h = surf->h;
	cells = calloc(w, sizeof(int*));
	if (!cells)
	{
		fflush(stdout);
		printf("Calloc failed to allocate memory for map cells!\n");
		SDL_FreeSurface(surf);
		exit(MEMORY_ALLOCATION_ERROR);
	}
	for (int i = 0; i < w; i++)
	{
		cells[i] = calloc(h, sizeof(int));
		if (!cells[i])
		{
			// freeing allocated memory prior to the failure
			for (unsigned int j = 0; j < i; j++)
			{
				free(cells[i]);
			}
			free(cells);

			fflush(stdout);
			printf("Calloc failed to allocate memory for map cells[%d]!\n", i);
			SDL_FreeSurface(surf);
			exit(MEMORY_ALLOCATION_ERROR);
		}
	}

	// read pixels of map and convert it into the relevant int values
	for (int x = 0; x < w; x++)
	{
		for(int y = 0; y < h; y++)
		{
			Uint32 data = get_pixel_32(surf, x, y);
			SDL_Color color = convert_pixel_data_to_color(surf, data);

			// green = start (0, 255, 0)
			// red = end (255, 0, 0)
			// white = obstacle(255, 255, 255)
			// black = empty(0, 0, 0)
			if (color.r == (Uint8)255 && color.g == (Uint8)0 && color.b == (Uint8)0)
			{
				if (!has_end) { has_end = 1; }
				else {
					fflush(stdout);
					printf("WARNING: invalid map %s, has more than one ending point!\n", filepath);
					SDL_FreeSurface(surf);
					return NULL;
				}

				cells[x][y] = END;
			}
			else if (color.r == (Uint8)0 && color.g == (Uint8)255 && color.b == (Uint8)0)
			{
				if (!has_start) { has_start = 1; }
				else {
					fflush(stdout);
					printf("WARNING: invalid map %s, has more than one starting point!\n", filepath);
					SDL_FreeSurface(surf);
					return NULL;
				}

				cells[x][y] = START;
			}
			else if (color.r == (Uint8)255 && color.g == (Uint8)255 && color.b == (Uint8)255)
			{
				cells[x][y] = OBSTACLE;
			}
			else if (color.r == (Uint8)0 && color.g == (Uint8)0 && color.b == (Uint8)0)
			{
				cells[x][y] = EMPTY;
			}
			else
			{
				fflush(stdout);
				printf("WARNING: unknown color detected! Assuming empty cell and continuing...\n");
				cells[x][y] = EMPTY;
			}
		}
	}

	// for a map to valid it must have an start and an end
	if (!(has_start && has_end))
	{
		fflush(stdout);
		printf("WARNING: invalid map %s, has no starting and/or ending point(s)!\n", filepath);
		SDL_FreeSurface(surf);
		return NULL;
	}

	SDL_FreeSurface(surf); // surface is no longer needed, thus freeing it from memory
	return &(struct Map) { cells, w, h };
}

// frees the map struct and is internal data from memory
void free_map(struct Map* map)
{
	for (int x = 0; x < map->w; x++)
	{
		free(map->cells[x]);
	}
	free(map->cells);
	free(map);
}
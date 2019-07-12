#include "maps.h"

// code sourced from: http://sdl.beuc.net/sdl.wiki/Pixel_Access
Uint32 get_pixel_data(SDL_Surface* surface, int x, int y)
{
	// locking the surface to access pixels directly
	if (SDL_LockSurface(surface) != 0)
	{
		printf("SDL_Error (Error locking surface) -> %s\n", SDL_GetError());
		exit(SURFACE_LOCK_ERROR);
	}

	int bpp = surface->format->BytesPerPixel;
	Uint8* pixel = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	// unlocking the surface as we are finished accessing the pixels directly
	SDL_UnlockSurface(surface);

	switch (bpp)
	{
		case 1:
			return *pixel;
			break;

		case 2:
			return *(Uint16*)pixel;
			break;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
			else
				return pixel[0] | pixel[1] << 8 | pixel[2] << 16;
			break;

		case 4:
			return *(Uint32*)pixel;
			break;

		default:
			return 0; /* shouldn't happen, but avoids warnings */
	}
}
// TODO DEBUG

SDL_Color convert_pixel_data_to_color(SDL_Surface* surface, Uint32 data)
{
	SDL_Color rgb;

	SDL_GetRGB(data, surface->format, &rgb.r, &rgb.g, &rgb.b);
	if (!&rgb)
	{
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
		printf("SDL_Error -> %s\n", SDL_GetError());
		exit(LOAD_IMAGE_ERROR);
	}

	// allocating memory for map
	w = surf->w;
	h = surf->h;
	cells = calloc(w, sizeof(int*));
	if (!cells)
	{
		printf("Calloc failed to allocate memory for map cells!\n");
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

			printf("Calloc failed to allocate memory for map cells[%d]!\n", i);
			exit(MEMORY_ALLOCATION_ERROR);
		}
	}

	// read pixels of map and convert it into the relevant int values
	for (int x = 0; x < w; x++)
	{
		for(int y = 0; y < h; y++)
		{
			Uint32 data = get_pixel_data(surf, x, y);
			SDL_Color color = convert_pixel_data_to_color(surf, data);

			// green = start (0, 255, 0)
			// red = end (255, 0, 0)
			// white = obstacle(255, 255, 255)
			// black = empty(0, 0, 0)

			int r = color.r, g = color.g, b = color.b;
			if (r == g == b)
			{
				// white
				if (r == 255)
				{
					cells[x][y] = OBSTACLE;
				}
				else if (r == 0) // black
				{
					cells[x][y] = EMPTY;
				}
				else // undesired
				{
					printf("WARNING: unknown color in map %s encountered at xy: %d, %d\nAssuming it is empty space...\n", 
						filepath, x, y);
					cells[x][y] = EMPTY;
				}
			}
			else
			{
				// green
				if (r == 0 && g == 255 && b == 0)
				{
					if (has_start == 0)
					{
						cells[x][y] = START;
						has_start == 1;
					}
					else
					{
						printf("WARNING: Failed to load map, multiple starting points detected!\n");
						return NULL;
					}
				}
				else if(r == 255 && g == 0 && b == 0) // red
				{
					if (has_end == 0)
					{
						cells[x][y] = END;
						has_end == 1;
					}
					else
					{
						printf("WARNING: Failed to load map, multiple ending points detected!\n");
						return NULL;
					}
				}
				else // undesired
				{
					printf("WARNING: unknown color in map %s encountered at xy: %d, %d\nAssuming it is empty space...\n",
						filepath, x, y);
					cells[x][y] = EMPTY;
				}
			}
		}
	}

	// for a map to valid it must have an start and an end
	if (!(has_start && has_end))
	{
		printf("WARNING: invalid map %s, has no starting and/or ending point(s)!\n");
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
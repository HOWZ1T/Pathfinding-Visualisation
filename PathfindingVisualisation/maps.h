#pragma once
#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>

#include "errors.h"

#define EMPTY 1
#define START 2
#define END 3
#define OBSTACLE 4

struct Map
{
	int** cells;
	int w, h;
};

// loads a map from the specified image filepath
// returns NULL on failure
struct Map* load_map_from_image(const char* filepath);

// frees the map struct and is internal data from memory
void free_map(struct Map* map);
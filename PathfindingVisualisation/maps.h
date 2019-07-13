#pragma once
#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>

#include "errors.h"

#define EMPTY 0
#define START 1
#define END 2
#define OBSTACLE 3

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
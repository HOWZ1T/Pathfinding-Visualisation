#pragma once

#include "utils.h"
#include "maps.h"

struct Agent {
	int x, y;
	Color color;
};

typedef struct Agent Agent;

void move_agent(Agent* agent, Map* map, int x, int y);
#pragma once

#include "maps.h"

struct Node
{
	/* f is total cost of node (f = g + h), 
	   g is distance between node and the start, 
	   h is the heuristic (estimated distance from the current node to the end)*/
	int f, g, h;
};

typedef struct Node Node;

Node** create_node_map_from_map(Map* map);
void free_node_map(Node** node_map);
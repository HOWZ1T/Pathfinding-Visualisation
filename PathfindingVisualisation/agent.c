#include "agent.h"

void move_agent(Agent* agent, Map* map, int x, int y)
{
	int ax = agent->x + x;
	int ay = agent->y + y;

	// if we are outside the bounds of the map return without updating the agent
	if (ax < 0 || ay < 0 || ax >= map->w || ay >= map->h) { return; }

	if (map->cells[ax][ay] == EMPTY || map->cells[ax][ay] == START || map->cells[ax][ay] == END)
	{
		agent->x = ax;
		agent->y = ay;
	}
}
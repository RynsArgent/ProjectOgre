#include "formation.h"

Formation::Formation()
	: width(FORMATION_WIDTH), height(FORMATION_HEIGHT), grid(), targetOrder(TARGET_RANDOM)
{
	grid = vector<vector<Character*> >(width, vector<Character*>(height));
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
			grid[i][j] = NULL;
}

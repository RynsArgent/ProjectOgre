#include "group.h"

Group::Group(Formation *formation)
	: formation(formation)
{
	carryOverFormationStatistics();
}

void Group::carryOverFormationStatistics()
{
	width = formation->getWidth();
	height = formation->getHeight();
	grid = std::vector<std::vector<Unit*> >(width, std::vector<Unit*>(height, NULL));
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y)
		{
			grid[x][y] = new Unit(formation->getCharacterAt(x, y), x, y);
		}
}
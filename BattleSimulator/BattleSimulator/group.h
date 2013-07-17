#pragma once

#include <vector>
#include "unit.h"
#include "formation.h"

class Group
{
private:
	Formation *formation;

	int width;
	int height;
	
	std::vector<std::vector<Unit*> > grid;
	
	void carryOverFormationStatistics();
public:
	Group(Formation *formation);

	int getWidth() const {
		return width;
	}
	
	int getHeight() const {
		return height;
	}

	Unit* getUnitAt(int x, int y) const {
		return grid[x][y];
	}
	
	void setUnitAt(int x, int y, Unit* unit) {
		grid[x][y] = unit;
	}
};

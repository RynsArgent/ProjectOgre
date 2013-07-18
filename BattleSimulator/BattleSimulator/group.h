#pragma once

#include <vector>
#include "unit.h"
#include "formation.h"

// Should not be associated with Battle group simulator, but here for now. Used to determine facing
enum Direction { DIRECTION_NONE, DIRECTION_NORTH, DIRECTION_EAST, DIRECTION_SOUTH, DIRECTION_WEST };
Direction turnLeft(Direction face);
Direction turnRight(Direction face);
Direction fullTurn(Direction face);

// Facing will start FORWARD, and will be adjusted depending on the direction
// of the Unit Group and the position of the other interacting entity
enum Facing { FACING_FORWARD, FACING_LEFT, FACING_RIGHT, FACING_BACKWARD };
Facing turnLeft(Facing face);
Facing turnRight(Facing face);
Facing fullTurn(Facing face);

class Group
{
private:
	int gid;
	Formation* formation;

	int width;
	int height;
	
	std::vector<std::vector<Unit*> > base;
	std::vector<std::vector<Unit*> > grid;

	Facing facing;
	
	void carryOverFormationStatistics();
public:
	Group(Formation* formation);
	
	int getGid() const {
		return gid;
	}
	
	int getWidth() const {
		return width;
	}
	
	int getHeight() const {
		return height;
	}

	bool hasUnitAt(int x, int y) const {
		return grid[x][y] != nullptr;
	}

	Unit* getUnitAt(int x, int y) const {
		return grid[x][y];
	}
	
	void setUnitAt(int x, int y, Unit* unit) {
		grid[x][y] = unit;
	}
	
	Facing getFacing() const {
		return facing;
	}

	void turnToFace(Facing face);

	std::vector<Unit*> theseUnits() const;
	std::vector<Unit*> theseUnitsAt(int xmin, int xmax, int ymin, int ymax) const;
	std::vector<Unit*> theseUnitsFurthestInFront(int xmin, int xmax) const;
	
	std::vector<Unit*> enemyUnits(Group* target) const;
	std::vector<Unit*> enemyUnitsAt(Group* target, int xmin, int xmax, int ymin, int ymax) const;
	std::vector<Unit*> enemyUnitsFurthestInFront(Group* target, int xmin, int xmax) const;

	~Group() {}
};

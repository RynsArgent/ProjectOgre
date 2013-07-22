#pragma once

#include "pch.h"

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
	
	vector<vector<Unit*> > base;
	vector<vector<Unit*> > grid;

	Facing facing;
	
	vector<Unit*> dead;

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
	
	bool groupIsAvailable() const;
	bool groupIsDead() const;
	bool withinColumnRange(int x, int xmin, int xmax) const;
	void cleanDead();

	vector<Unit*> allyUnits() const;
	vector<Unit*> allyUnitsAt(int xmin, int xmax, int ymin, int ymax) const;
	vector<Unit*> allyUnitsAtColumn(int x) const;
	vector<Unit*> allyUnitsAtRow(int y) const;
	vector<Unit*> allyUnitsAtColumns(int xmin, int xmax) const;
	vector<Unit*> allyUnitsAtRows(int ymin, int ymax) const;
	Unit* allyUnitFurthestInFront(int x) const;
	vector<Unit*> allyUnitsFurthestInFront(int xmin, int xmax) const;
	
	vector<Unit*> enemyUnits(Group* target) const;
	vector<Unit*> enemyUnitsAt(Group* target, int xmin, int xmax, int ymin, int ymax) const;
	vector<Unit*> enemyUnitsAtColumn(Group* target, int x) const;
	vector<Unit*> enemyUnitsAtRow(Group* target, int y) const;
	vector<Unit*> enemyUnitsAtColumns(Group* target, int xmin, int xmax) const;
	vector<Unit*> enemyUnitsAtRows(Group* target, int ymin, int ymax) const;
	Unit* enemyUnitFurthestInFront(Group* target, int x) const;
	vector<Unit*> enemyUnitsFurthestInFront(Group* target, int xmin, int xmax) const;

	void printGroup(bool mirrored) const;

	~Group() {}
};

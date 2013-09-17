#ifndef __GROUP_H__
#define __GROUP_H__

#include "pch.h"

#include <vector>
#include "unit.h"
#include "formation.h"
#include "gridpoint.h"

class Group
{
protected:
	int gid;
	Formation* formation;

	int width;
	int height;
	
	vector<vector<Unit*> > base; // Original representation of units
	vector<vector<Unit*> > grid; // Modified representation of units due to facing and battle
	Facing facing;
	
	Unit* leader;

	vector<Unit*> dead; // Collects dead units in battle

	void carryOverFormationStatistics();
public:
	Group(Formation* formation);
	
	TargetType getTargetOrder() const {
		return formation->getTargetOrder();
	}

	int getGrid() const {
		return gid;
	}
	
	int getWidth() const {
		return width;
	}
	
	int getHeight() const {
		return height;
	}

	bool hasUnitAt(int x, int y) const {
		return grid[x][y] != NULL && grid[x][y]->isAvailable();
	}
    bool hasUnitAt(const GridPoint & p) const {
		return grid[p.x][p.y] != NULL && grid[p.x][p.y]->isAvailable();
	}
    
	Unit* getUnitAt(int x, int y) const {
		return grid[x][y];
	}
	Unit* getUnitAt(const GridPoint & p) const {
		return grid[p.x][p.y];
	}
	
	void setUnitAt(int x, int y, Unit* unit) {
		grid[x][y] = unit;
	}
	void setUnitAt(const GridPoint & p, Unit* unit) {
		grid[p.x][p.y] = unit;
	}

	Unit* getLeader() const {
		return leader;
	}
	void setLeader(Unit* value) {
		if (leader != NULL)
			leader->setLeader(false);
		leader = value;
		value->setLeader(true);
	}

    vector<GridPoint> getAvailablePoints() const {
        vector<GridPoint> ret;
        for (int x = 0; x < width; ++x)
            for (int y = 0; y < height; ++y)
            {
                if (!hasUnitAt(x, y))
                    ret.push_back(GridPoint(x, y));
            }
        return ret;
    }
	Facing getFacing() const {
		return facing;
	}

	void turnToFace(Facing face);
	void cleanDead();
	
	bool groupIsAvailable() const;
	bool withinColumnRange(int x, int xmin, int xmax) const;
    
	// NOTE: The functions below returns only available units
	// Returns units of this group based on function request
	vector<Unit*> allyUnits() const;
	vector<Unit*> allyUnitsAt(int xmin, int xmax, int ymin, int ymax) const;
	vector<Unit*> allyUnitsAtColumn(int x) const;
	vector<Unit*> allyUnitsAtRow(int y) const;
	vector<Unit*> allyUnitsAtColumns(int xmin, int xmax) const;
	vector<Unit*> allyUnitsAtRows(int ymin, int ymax) const;
	vector<Unit*> allyUnitsFurthestInFront(int xmin, int xmax, int depth = 1) const;
	vector<Unit*> allyUnitsInRowFurthestInFront(int rows = 1) const;
	
	// Returns unit that are on the opposite side of this group. The parameters passed belong to
	// this group's x and y values that will be translated to the enemy group coordinate ranges.
	vector<Unit*> enemyUnits(Group* target) const;
	vector<Unit*> enemyUnitsAt(Group* target, int xmin, int xmax, int ymin, int ymax) const;
	vector<Unit*> enemyUnitsAtColumn(Group* target, int x) const;
	vector<Unit*> enemyUnitsAtRow(Group* target, int y) const;
	vector<Unit*> enemyUnitsAtColumns(Group* target, int xmin, int xmax) const;
	vector<Unit*> enemyUnitsAtRows(Group* target, int ymin, int ymax) const;
	vector<Unit*> enemyUnitsFurthestInFront(Group* target, int xmin, int xmax, int depth = 1) const;
	vector<Unit*> enemyUnitsInRowFurthestInFront(Group* target, int rows = 1) const;

	void printGroup(bool mirrored) const;

	~Group() {}
};

#endif

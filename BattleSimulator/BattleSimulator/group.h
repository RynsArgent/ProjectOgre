#ifndef __GROUP_H__
#define __GROUP_H__

#include "pch.h"

#include <vector>
#include "unit.h"
#include "formation.h"
#include "gridpoint.h"

// This is an actual instance of a Formation

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

    vector<Unit*> units; // All units include dead and unavailable ones
    vector<Unit*> summoned; // All summoned units include dead and unavailable
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

    void addUnitToGroup(Unit* unit) {
        units.push_back(unit);
    }
    
    // Returns all Units dead, alive, or gone
    vector<Unit*> getAllUnits() const {
        vector<Unit*> ret(units.size() + summoned.size());
		int i = 0;
		for (; i < units.size(); ++i)
			ret[i] = units[i];
		for (int j = 0; j < summoned.size(); ++j)
			ret[i + j] = summoned[j];
		return ret;
    }

    // Returns all Units from original group, dead or alive
    vector<Unit*> getUnits() const {
        return units;
    }
	
    // Returns all Summoned units
    vector<Unit*> getSummoned() const {
        return summoned;
    }
    
    // Returns all dead units
    vector<Unit*> getDead() const {
        return dead;
    }
    
	void setSummoned(const vector<Unit*> & value) {
		summoned = value;
	}
	
	void setDead(const vector<Unit*> & value) {
		dead = value;
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
	
	Unit* getSummon(Unit* summoner)
	{
		for (int i = 0; i < summoned.size(); ++i)
			if (summoned[i]->getSummoner() == summoner)
				return summoned[i];
		return NULL;
	}

	Unit* addSummonAt(int x, int y, Character* character, Unit* summoner) {
		character->setBackSkillIndex(0);
		character->setMidSkillIndex(0);
		character->setFrontSkillIndex(0);
		character->setBasicSkillIndex(0);
		
		Unit* unit = new Unit(character, gid, x, y);
		unit->applyCharacterSkillSets();
		unit->setSummoner(summoner);
		setUnitAt(x, y, unit);

		summoned.push_back(unit);
		return unit;
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

	// Resets the grid without considered summoned units (initial battle group)
	void resetGrid() {
		for (int x = 0; x < width; ++x)
			for (int y = 0; y < height; ++y)
			{
				if (base[x][y] && !base[x][y]->isDead()) {
					setUnitAt(x, y, base[x][y]);
					base[x][y]->setOnGrid(x, y);
				}
				else
					setUnitAt(x, y, NULL);
			}
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
    vector<GridPoint> getAvailableAdjacentPoints(int x, int y) const {
        vector<GridPoint> ret;
		if (x - 1 >= 0 && !hasUnitAt(x - 1, y))
			ret.push_back(GridPoint(x - 1, y));
		if (x + 1 <= 2 && !hasUnitAt(x + 1, y))
			ret.push_back(GridPoint(x + 1, y));
		if (y - 1 >= 0 && !hasUnitAt(x, y - 1))
			ret.push_back(GridPoint(x, y - 1));
		if (y + 1 <= 2 && !hasUnitAt(x, y + 1))
			ret.push_back(GridPoint(x, y + 1));
        return ret;
    }
	Facing getFacing() const {
		return facing;
	}

	void turnToFace(Facing face);
	void cleanDead();
	void eraseDead(Unit* unit);
	
	bool groupIsAvailable() const;
	bool withinColumnRange(int x, int xmin, int xmax) const;
	bool enemyWithinRange(int x, int y, Unit* enemy, Group* group, int rowRange, int columnRange) const;
    
	vector<Unit*> unitsAdjacentTo(int x, int y) const;

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

	~Group();
};

#endif

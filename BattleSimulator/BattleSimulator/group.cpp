#include "group.h"

static int groupCount = 0;

Group::Group(Formation *formation)
	: gid(groupCount++), formation(formation), leader(NULL), dead(), facing(FACING_FORWARD)
{
	carryOverFormationStatistics();
}

void Group::carryOverFormationStatistics()
{
	width = formation->getWidth();
	height = formation->getHeight();
	base = vector<vector<Unit*> >(width, vector<Unit*>(height));
	grid = vector<vector<Unit*> >(width, vector<Unit*>(height));
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y)
		{
			if (formation->getCharacterAt(x, y) != NULL) {
				Unit* unit = new Unit(formation->getCharacterAt(x, y), gid, x, y);
				unit->applyCharacterSkillSets();
				base[x][y] = unit;
				grid[x][y] = base[x][y];
				
				if (formation->getLeaderPosition() == GridPoint(x, y)) {
					leader = unit;
					unit->setLeader(true);
				}
                addUnitToGroup(unit);
			}
			else
			{
				base[x][y] = NULL;
				grid[x][y] = NULL;
			}
		}
}

void Group::turnToFace(Facing face)
{
	facing = face;
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y)
		{
			grid[x][y] = NULL;
		}
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y)
		{
			int xmod;
			int ymod;
			switch (facing)
			{
				case FACING_FORWARD:
					xmod = x;
					ymod = y;
					break;
				case FACING_LEFT:
					xmod = y;
					ymod = height - x - 1;
					break;
				case FACING_RIGHT:
					xmod = width - y - 1;
					ymod = x;
					break;
				case FACING_BACKWARD:
					xmod = width - x - 1;
					ymod = height - y - 1;
					break;
				default:
					xmod = x;
					ymod = y;
					break;
			}
			grid[xmod][ymod] = base[x][y];
			if (base[x][y] != NULL)
				base[x][y]->setOnGrid(xmod, ymod);
		}
}

void Group::cleanDead()
{
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y)
		{
			Unit* unit = grid[x][y];
			if (unit != NULL && unit->isDead())
			{
				int x = unit->getGridX();
				int y = unit->getGridY();
				grid[x][y] = NULL;
				dead.push_back(unit);
			}
		}
}

bool Group::groupIsAvailable() const
{
	vector<Unit*> units = allyUnits();
	return units.size() > 0;
}

// returns true if x is within min and max for this group
bool Group::withinColumnRange(int x, int xmin, int xmax) const
{
	return xmin <= x && x <= xmax;
}

vector<Unit*> Group::unitsAdjacentTo(int x, int y) const
{
	vector<Unit*> ret;
	if (x - 1 >= 0 && hasUnitAt(x - 1, y))
		ret.push_back(getUnitAt(x - 1, y));
	if (y - 1 >= 0 && hasUnitAt(x, y - 1))
		ret.push_back(getUnitAt(x, y - 1));
	if (x + 1 <= 2 && hasUnitAt(x + 1, y))
		ret.push_back(getUnitAt(x + 1, y));
	if (y + 1 <= 2 && hasUnitAt(x, y + 1))
		ret.push_back(getUnitAt(x, y + 1));
	return ret;
}

vector<Unit*> Group::allyUnits() const
{
	vector<Unit*> ret;
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y)
		{
			if (hasUnitAt(x, y))
				ret.push_back(grid[x][y]);
		}
	return ret;
}

vector<Unit*> Group::allyUnitsAt(int xmin, int xmax, int ymin, int ymax) const
{
	xmin = max(0, xmin);
	xmax = min(width - 1, xmax);
	ymin = max(0, ymin);
	ymax = min(height - 1, ymax);

	vector<Unit*> ret;
	for (int x = xmin; x <= xmax; ++x)
		for (int y = ymin; y <= ymax; ++y)
		{
			if (hasUnitAt(x, y))
				ret.push_back(grid[x][y]);
		}
	return ret;
}

vector<Unit*> Group::allyUnitsAtColumn(int x) const
{
	if (x < 0 || x >= width)
		return vector<Unit*>();

	vector<Unit*> ret;
	for (int y = 0; y < height; ++y)
	{
		if (hasUnitAt(x, y))
			ret.push_back(grid[x][y]);
	}
	return ret;
}


vector<Unit*> Group::allyUnitsAtRow(int y) const
{
	if (y < 0 || y >= height)
		return vector<Unit*>();

	vector<Unit*> ret;
	for (int x = 0; x < width; ++x)
	{
		if (hasUnitAt(x, y))
			ret.push_back(grid[x][y]);
	}
	return ret;
}

vector<Unit*> Group::allyUnitsAtColumns(int xmin, int xmax) const
{
	xmin = max(0, xmin);
	xmax = min(width - 1, xmax);

	vector<Unit*> ret;
	for (int x = xmin; x <= xmax; ++x)
		for (int y = 0; y < height; ++y)
		{
			if (hasUnitAt(x, y))
				ret.push_back(grid[x][y]);
		}
	return ret;
}

vector<Unit*> Group::allyUnitsAtRows(int ymin, int ymax) const
{
	ymin = max(0, ymin);
	ymax = min(height - 1, ymax);

	vector<Unit*> ret;
	for (int x = 0; x < width; ++x)
		for (int y = ymin; y <= ymax; ++y)
		{
			if (hasUnitAt(x, y))
				ret.push_back(grid[x][y]);
		}
	return ret;
}

vector<Unit*> Group::allyUnitsFurthestInFront(int xmin, int xmax, int depth) const
{
	xmin = max(0, xmin);
	xmax = min(width - 1, xmax);

	vector<Unit*> ret;
	for (int x = xmin; x <= xmax; ++x)
	{
		int d = 0;
		for (int y = 0; y < height; ++y)
		{
			if (hasUnitAt(x, y))
			{
				ret.push_back(grid[x][y]);
				++d;
				if (d >= depth)
					break;
			}
		}
	}
	return ret;
}

vector<Unit*> Group::allyUnitsInRowFurthestInFront(int rows) const
{
	vector<Unit*> ret;
	bool firstFound = false;
	int r = 0;
	for (int y = 0; y < height && r < rows; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (hasUnitAt(x, y))
			{
				ret.push_back(grid[x][y]);
				firstFound = true;
			}
		}
		if (firstFound)
			++r;
	}
	return ret;
}

vector<Unit*> Group::enemyUnits(Group* target) const
{
	return target->allyUnits();
}

vector<Unit*> Group::enemyUnitsAt(Group* target, int xmin, int xmax, int ymin, int ymax) const
{
	int xminmod = target->getWidth() - xmax - 1;
	int xmaxmod = target->getWidth() - xmin - 1;
	int yminmod = ymin;
	int ymaxmod = ymax;

	return target->allyUnitsAt(xminmod, xmaxmod, yminmod, ymaxmod);
}

vector<Unit*> Group::enemyUnitsAtColumn(Group* target, int x) const
{
	int xmod = target->getWidth() - x - 1;

	return target->allyUnitsAtColumn(xmod);
}

vector<Unit*> Group::enemyUnitsAtRow(Group* target, int y) const
{
	int ymod = y;

	return target->allyUnitsAtColumn(ymod);
}

vector<Unit*> Group::enemyUnitsAtColumns(Group* target, int xmin, int xmax) const
{
	int xminmod = target->getWidth() - xmax - 1;
	int xmaxmod = target->getWidth() - xmin - 1;

	return target->allyUnitsAtColumns(xminmod, xmaxmod);
}

vector<Unit*> Group::enemyUnitsAtRows(Group* target, int ymin, int ymax) const
{
	int yminmod = ymin;
	int ymaxmod = ymax;
	
	return target->allyUnitsAtRows(yminmod, ymaxmod);
}

vector<Unit*> Group::enemyUnitsFurthestInFront(Group* target, int xmin, int xmax, int depth) const
{
	int xminmod = target->getWidth() - xmax - 1;
	int xmaxmod = target->getWidth() - xmin - 1;

	return target->allyUnitsFurthestInFront(xminmod, xmaxmod, depth);
}

vector<Unit*> Group::enemyUnitsInRowFurthestInFront(Group* target, int rows) const 
{
	// Redundant but makes class functions nice and parallel
	return target->allyUnitsInRowFurthestInFront(rows);
}

void Group::printGroup(bool mirrored) const
{
	if (mirrored)
	{
		for (int y = height - 1; y >= 0; --y)
		{
			for (int x = width - 1; x >= 0; --x)
			{
				if (grid[x][y] != NULL)
					cout << grid[x][y]->currentHealth << " ";
				else
					cout << "*" << " ";
			}
			cout << endl;
		}
	}
	else
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (grid[x][y] != NULL)
					cout << grid[x][y]->currentHealth << " ";
				else
					cout << "*" << " ";
			}
			cout << endl;
		}
	}
}

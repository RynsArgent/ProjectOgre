#include "group.h"

Direction turnLeft(Direction face)
{
	switch (face)
	{
	case DIRECTION_NORTH:
		return DIRECTION_WEST;
	case DIRECTION_EAST:
		return DIRECTION_NORTH;
	case DIRECTION_SOUTH:
		return DIRECTION_EAST;
	case DIRECTION_WEST:
		return DIRECTION_SOUTH;
	default:
		return DIRECTION_NONE;
	}
}

Direction turnRight(Direction face)
{
	switch (face)
	{
	case DIRECTION_NORTH:
		return DIRECTION_EAST;
	case DIRECTION_EAST:
		return DIRECTION_SOUTH;
	case DIRECTION_SOUTH:
		return DIRECTION_WEST;
	case DIRECTION_WEST:
		return DIRECTION_NORTH;
	default:
		return DIRECTION_NONE;
	}
}
Direction fullTurn(Direction face)
{
	switch (face)
	{
	case DIRECTION_NORTH:
		return DIRECTION_SOUTH;
	case DIRECTION_EAST:
		return DIRECTION_WEST;
	case DIRECTION_SOUTH:
		return DIRECTION_NORTH;
	case DIRECTION_WEST:
		return DIRECTION_EAST;
	default:
		return DIRECTION_NONE;
	}
}

Facing turnLeft(Facing face)
{
	switch (face)
	{
	case FACING_FORWARD:
		return FACING_LEFT;
	case FACING_LEFT:
		return FACING_BACKWARD;
	case FACING_RIGHT:
		return FACING_FORWARD;
	case FACING_BACKWARD:
		return FACING_RIGHT;
	default:
		return FACING_FORWARD;
	}
}

Facing turnRight(Facing face)
{
	switch (face)
	{
	case FACING_FORWARD:
		return FACING_RIGHT;
	case FACING_LEFT:
		return FACING_FORWARD;
	case FACING_RIGHT:
		return FACING_BACKWARD;
	case FACING_BACKWARD:
		return FACING_LEFT;
	default:
		return FACING_FORWARD;
	}
}

Facing fullTurn(Facing face)
{
	switch (face)
	{
	case FACING_FORWARD:
		return FACING_BACKWARD;
	case FACING_LEFT:
		return FACING_RIGHT;
	case FACING_RIGHT:
		return FACING_LEFT;
	case FACING_BACKWARD:
		return FACING_FORWARD;
	default:
		return FACING_FORWARD;
	}
}

static int groupCount = 0;

Group::Group(Formation *formation)
	: gid(groupCount++), formation(formation), facing(FACING_FORWARD)
{
	carryOverFormationStatistics();
}

void Group::carryOverFormationStatistics()
{
	width = formation->getWidth();
	height = formation->getHeight();
	base = std::vector<std::vector<Unit*> >(width, std::vector<Unit*>(height, NULL));
	grid = std::vector<std::vector<Unit*> >(width, std::vector<Unit*>(height, NULL));
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y)
		{
			Unit* unit = new Unit(formation->getCharacterAt(x, y), gid, x, y);
			base[x][y] = unit;
			grid[x][y] = base[x][y];
		}
}

void Group::turnToFace(Facing face)
{
	facing = face;
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
			base[x][y]->setOnGrid(xmod, ymod);
		}
}

std::vector<Unit*> Group::theseUnits() const
{
	vector<Unit*> ret;
	for (int x = 0; x < width; ++x)
		for (int y = 0; y < height; ++y)
		{
			if (hasUnitAt(x, y) && grid[x][y]->isAvailable())
				ret.push_back(grid[x][y]);
		}
	return ret;
}

std::vector<Unit*> Group::theseUnitsAt(int xmin, int xmax, int ymin, int ymax) const
{
	vector<Unit*> ret;
	for (int x = xmin; x <= xmax; ++x)
		for (int y = ymin; y <= ymax; ++y)
		{
			if (hasUnitAt(x, y) && grid[x][y]->isAvailable())
				ret.push_back(grid[x][y]);
		}
	return ret;
}

std::vector<Unit*> Group::theseUnitsFurthestInFront(int xmin, int xmax) const
{
	vector<Unit*> ret;
	for (int x = xmin; x <= xmax; ++x)
		for (int y = 0; y < height - 1; ++y)
		{
			if (hasUnitAt(x, y) && grid[x][y]->isAvailable())
			{
				ret.push_back(grid[x][y]);
				break;
			}
		}
	return ret;
}

std::vector<Unit*> Group::enemyUnits(Group* target) const
{
	return target->theseUnits();
}

std::vector<Unit*> Group::enemyUnitsAt(Group* target, int xmin, int xmax, int ymin, int ymax) const
{
	int xminmod = target->getWidth() - xmax - 1;
	int xmaxmod = target->getWidth() - xmin - 1;
	int yminmod = ymin;
	int ymaxmod = ymax;

	return target->theseUnitsAt(xminmod, xmaxmod, yminmod, ymaxmod);
}

std::vector<Unit*> Group::enemyUnitsFurthestInFront(Group* target, int xmin, int xmax) const
{
	int xminmod = target->getWidth() - xmax - 1;
	int xmaxmod = target->getWidth() - xmin - 1;

	return target->theseUnitsFurthestInFront(xminmod, xmaxmod);
}
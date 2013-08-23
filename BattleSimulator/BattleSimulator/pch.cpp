#include "pch.h"

#include <sstream>

int bound(int value, int min, int max)
{
	if (value < min) return min;
	else if (value > max) return max;
	else return value;
}

int bound(int value, AttributeType type)
{
	switch (type)
	{
		case VALUE_HEALTH:
			return bound(value, 0, 999);
		case VALUE_SPEED:
			return bound(value, 0, 10);
		case VALUE_PHYSICAL_ATTACK: 
			return bound(value, 0, 999);
		case VALUE_MAGIC_ATTACK:
			return bound(value, 0, 999);
		case VALUE_PHYSICAL_DEFENSE: 
			return bound(value, 0, 999);
		case VALUE_FIRE_DEFENSE:
			return bound(value, 0, 999);
		case VALUE_WATER_DEFENSE: 
			return bound(value, 0, 999);
		case VALUE_EARTH_DEFENSE: 
			return bound(value, 0, 999);
		case VALUE_ICE_DEFENSE:
			return bound(value, 0, 999);
		case VALUE_LIGHTNING_DEFENSE:
			return bound(value, 0, 999);
		case VALUE_SIZE:
			return bound(value, 0, 3);
		default:
			return value;
	}
}

string toStringAT(AbilityType val)
{
    switch (val) {
        case ABILITY_NONE:
            return "None";
        case ABILITY_ATTACK_MELEE:
            return "Melee Attack";
        case ABILITY_ATTACK_RANGE:
            return "Range Attack";
        case ABILITY_SPECIAL:
            return "Special";
    }
	return "";
}

string toStringDT(DamageType val)
{
    switch (val) {
        case DAMAGE_TYPELESS:
            return "None";
        case DAMAGE_PHYSICAL:
            return "Physical";
        case DAMAGE_FIRE:
            return "Fire";
        case DAMAGE_WATER:
            return "Water";
        case DAMAGE_EARTH:
            return "Earth";
        case DAMAGE_ICE:
            return "Ice";
        case DAMAGE_LIGHTNING:
            return "Lightning";
		case DAMAGE_HEALING:
			return "Life";
    }
	return "";
}

string toStringET(ElementType val)
{
    switch (val) {
        case ELEMENT_NONE:
            return "None";
        case ELEMENT_PHYSICAL:
            return "Physical";
        case ELEMENT_FIRE:
            return "Fire";
        case ELEMENT_WATER:
            return "Water";
        case ELEMENT_EARTH:
            return "Earth";
        case ELEMENT_ICE:
            return "Ice";
        case ELEMENT_LIGHTNING:
            return "Lightning";
    }
	return "";
}

string toStringInt(int val)
{
	stringstream ss;
	ss << val;
	return ss.str();
}

string toStringDouble(double val)
{
	stringstream ss;
	ss << val;
	return ss.str();
}

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

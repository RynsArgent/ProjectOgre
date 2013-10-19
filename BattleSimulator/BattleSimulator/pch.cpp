#include "pch.h"

#include <sstream>

Skill retrieveSkill(SkillSet set, ElementType element)
{
	if (set == SPELL1) {
		switch (element) {
			case ELEMENT_NONE:
				return ARCANE_BOLT;
			case ELEMENT_PHYSICAL:
				return ARCANE_BOLT;
			case ELEMENT_ARCANE:
				return ARCANE_BOLT;
			case ELEMENT_FIRE:
				return FIREBALL;
			case ELEMENT_WATER:
				return WATER_JET;
			case ELEMENT_EARTH:
				return ACID_DART;
			case ELEMENT_ICE:
				return FROST_SHARD;
			case ELEMENT_LIGHTNING:
				return LIGHTNING_BOLT;
		}
	} else if (set == SPELL2) {
		switch (element) {
			case ELEMENT_NONE:
				return ARCANE_MISSIL;
			case ELEMENT_PHYSICAL:
				return ARCANE_MISSIL;
			case ELEMENT_ARCANE:
				return ARCANE_MISSIL;
			case ELEMENT_FIRE:
				return FLAMESTRIKE;
			case ELEMENT_WATER:
				return WATERBLAST;
			case ELEMENT_EARTH:
				return ROCKSLIDE;
			case ELEMENT_ICE:
				return ICICLE;
			case ELEMENT_LIGHTNING:
				return ELECTRIC_CURTAIN;
		}
    } else if (set == SPELL_DEBUFF1) {
		switch (element) {
			case ELEMENT_NONE:
				return STUN_WAVE;
			case ELEMENT_PHYSICAL:
				return STUN_WAVE;
			case ELEMENT_ARCANE:
				return STUN_WAVE;
			case ELEMENT_FIRE:
				return DISORIENTING_MIST;
			case ELEMENT_WATER:
				return SLEEP_MIASMA;
			case ELEMENT_EARTH:
				return NOXIOUS_SPORES;
			case ELEMENT_ICE:
				return CHILLING_FOG;
			case ELEMENT_LIGHTNING:
				return PARALYTIC_CLOUD;
		}
    }
	return NO_STANDARD_SKILL;
}

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
		case VALUE_DAMAGE: 
			return bound(value, 0, 999);
		case VALUE_PHYSICAL_ATTACK: 
			return bound(value, 0, 999);
		case VALUE_MAGIC_ATTACK:
			return bound(value, 0, 999);
		case VALUE_PHYSICAL_DEFENSE: 
			return bound(value, 0, 999);
		case VALUE_ARCANE_DEFENSE:
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
        case DAMAGE_ARCANE:
            return "Arcane";
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
        case ELEMENT_ARCANE:
            return "Arcane";
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

string toStringJob(JobType val)
{
	switch (val) {
        case JOB_NONE:
            return "None";
        case JOB_FIGHTER:
            return "Fighter";
        case JOB_SCOUT:
            return "Scout";
        case JOB_ACOLYTE:
            return "Acolyte";
        case JOB_MAGE:
            return "Mage";
        case JOB_WARRIOR:
            return "Warrior";
        case JOB_KNIGHT:
            return "Knight";
        case JOB_BARBARIAN:
            return "Barbarian";
        case JOB_ROGUE:
            return "Rogue";
        case JOB_HUNTER:
            return "Hunter";
        case JOB_ARCHER:
            return "Archer";
        case JOB_BARD:
            return "Bard";
        case JOB_PRIEST:
            return "Priest";
        case JOB_PUGILIST:
            return "Pugilist";
        case JOB_WIZARD:
            return "Wizard";
        case JOB_ENCHANTER:
            return "Enchanter";
        case JOB_DRUID:
            return "Druid";
        case JOB_TREE_OF_LIFE:
            return "TreeOfLife";
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

#include "pch.h"

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

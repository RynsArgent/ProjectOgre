#ifndef __PCH_H__
#define __PCH_H__

#include <iostream>

using namespace std;

enum ElementType { ELEMENT_NONE, ELEMENT_PHYSICAL, ELEMENT_FIRE, ELEMENT_WATER, ELEMENT_EARTH, ELEMENT_ICE, ELEMENT_LIGHTNING };
enum AbilityType { ABILITY_NONE, ABILITY_ATTACK_MELEE, ABILITY_ATTACK_RANGE, ABILITY_SPECIAL };
enum AbilityAction { ACTION_STANDARD, ACTION_RESPONSE };
enum DamageRating { DAMAGE_NONE, DAMAGE_MINOR, DAMAGE_LOW, DAMAGE_MEDIUM, DAMAGE_HIGH, DAMAGE_MASSIVE };
enum DamageType { DAMAGE_TYPELESS, DAMAGE_PHYSICAL, DAMAGE_FIRE, DAMAGE_WATER, DAMAGE_EARTH, DAMAGE_ICE, DAMAGE_LIGHTNING, DAMAGE_HEALING };
enum StatusBenefit { NEUTRAL, BUFF, DEBUFF };
enum TargetGroup { TARGET_ENEMIES, TARGET_ALLIES, TARGET_BOTH };
enum TargetType { TARGET_CONFUSED, TARGET_CHARMED, TARGET_RANDOM, TARGET_WEAKEST, TARGET_STRONGEST };

enum Skill { NO_STANDARD_SKILL, NO_RESPONSE_SKILL, HUNDRED_BLADES, BLOCK, STRIKE, TAUNT, BATTLE_SHOUT, SHOOT, FOREWARN, SCOPE, TANGLE_TRAP };
#define NUMBER_OF_SKILLS 10

string toStringAT(AbilityType val); 
string toStringDT(DamageType val);
string toStringET(ElementType val);

class Character;
class Unit;
class Action;
struct Damage;
class Effect;
class Status;
struct Targeter;
class Ability;
struct GridPoint;
class Formation;
class Group;
class Battle;



#endif

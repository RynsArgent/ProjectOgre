#ifndef __PCH_H__
#define __PCH_H__

#include <iostream>

using namespace std;

enum AttributeType { 
	VALUE_HEALTH, 
	VALUE_SPEED, 
	VALUE_DAMAGE,
	VALUE_PHYSICAL_ATTACK, 
	VALUE_MAGIC_ATTACK, 
	VALUE_PHYSICAL_DEFENSE,
	VALUE_ARCANE_DEFENSE,
	VALUE_FIRE_DEFENSE, 
	VALUE_WATER_DEFENSE, 
	VALUE_EARTH_DEFENSE, 
	VALUE_ICE_DEFENSE, 
	VALUE_LIGHTNING_DEFENSE,
	VALUE_SIZE
};
enum JobType { JOB_NONE, JOB_FIGHTER, JOB_SCOUT, JOB_ACOLYTE, JOB_MAGE, JOB_WARRIOR, JOB_KNIGHT };
enum ElementType { ELEMENT_NONE, ELEMENT_PHYSICAL, ELEMENT_ARCANE, ELEMENT_FIRE, ELEMENT_WATER, ELEMENT_EARTH, ELEMENT_ICE, ELEMENT_LIGHTNING };
enum ActionType { ACTION_NONE, ABILITY_STANDARD, EFFECT_TRIGGER };
enum AbilityType { ABILITY_NONE, ABILITY_ATTACK_MELEE, ABILITY_ATTACK_RANGE, ABILITY_SPECIAL };
enum DamageRating { DAMAGE_NONE, DAMAGE_MINIMAL, DAMAGE_MINOR, DAMAGE_LOW, DAMAGE_MEDIUM, DAMAGE_HIGH, DAMAGE_MASSIVE, DAMAGE_COLOSSAL, DAMAGE_UNSTOPPABLE };
enum DamageType { DAMAGE_TYPELESS, DAMAGE_PHYSICAL, DAMAGE_ARCANE, DAMAGE_FIRE, DAMAGE_WATER, DAMAGE_EARTH, DAMAGE_ICE, DAMAGE_LIGHTNING, DAMAGE_HEALING };
enum StatusBenefit { NEUTRAL, BUFF, DEBUFF };
enum StatusMatch { STATUS_UNMATCHABLE, STATUS_SELFMATCHABLE, STATUS_ALLMATCHABLE };
enum StatusCategory { STATUS_UNCATEGORIZED, STATUS_DOT, STATUS_CC, STATUS_STAT };
enum TargetGroup { TARGET_ENEMIES, TARGET_ALLIES, TARGET_BOTH };
enum TargetType { TARGET_CONFUSED, TARGET_CHARMED, TARGET_RANDOM, TARGET_WEAKEST, TARGET_STRONGEST, TARGET_LEADER, TARGET_HEALING, TARGET_MOST_BUFFS, TARGET_MOST_DEBUFFS };

enum Skill { 
	NO_STANDARD_SKILL, 
	HUNDRED_BLADES, BLOCK, STRIKE, TAUNT, BATTLE_SHOUT, 
	SHOOT, HASTE, SCOPE, TANGLE_TRAP, 
	MEND, HEAL, CLEANSE, REGENERATION, BLIND,
	BARRIER, POLYMORPH, ARCANE_BOLT, FIREBALL, WATER_JET, ACID_DART, FROST_SHARD, LIGHTNING_BOLT,
	SLASH, PROVOKE, DEMORALIZING_SHOUT, CHARGE, RALLY, CHALLENGE, FLURRY };
enum SkillSet { SPELL1 };
Skill retrieveSkill(SkillSet set, ElementType element);

int bound(int value, int min, int max);
int bound(int value, AttributeType type);
string toStringAT(AbilityType val);
string toStringDT(DamageType val);
string toStringET(ElementType val);
string toStringJob(JobType val);
string toStringInt(int val);
string toStringDouble(double val);

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

// Forward declarations of all classes
class Character;
class Job;
class Unit;
class Action;
struct Event;
struct Damage;
class Effect;
class Status;
class StatusGroup;
struct Targeter;
class Ability;
struct GridPoint;
class Formation;
class Group;
class Battle;

#endif

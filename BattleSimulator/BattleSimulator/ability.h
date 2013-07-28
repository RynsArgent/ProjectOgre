#ifndef __ABILITY_H__
#define __ABILITY_H__

#include "pch.h"

#include "unit.h"
#include "battle.h"

// Base class for abilities
class Ability
{
protected:
	AbilityType type; // Defines the category an ability belongs to
	bool respondable; // Determines whether this ability can be followed with a counterattack
	int cost; // Potentially can be AP cost
public:
	Ability(AbilityType type = ABILITY_NONE, bool respondable = false, int cost = 0)
		: type(type), respondable(respondable), cost(cost)
	{}
	
	// Most abilities will have their logic implemented in this function
	virtual void action(Unit* caster, Battle* battle) = 0;

	~Ability() {}
};

class NoSkill : public Ability
{
protected:
	static const AbilityType TYPE = ABILITY_NONE;
	static const bool RESPONDABLE = false;
	static const int COST = 0;
public:
	NoSkill() : Ability(TYPE, RESPONDABLE, COST) {}
	virtual void action(Unit* caster, Battle* battle) {}
	~NoSkill() {}
};

class HundredBlades : public Ability
{
protected:
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool RESPONDABLE = true;
	static const int COST = 1;
public:
	HundredBlades() : Ability(TYPE, RESPONDABLE, COST) {}
	virtual void action(Unit* caster, Battle* battle);
	~HundredBlades() {}
};

class Block : public Ability
{
protected:
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool RESPONDABLE = false;
	static const int COST = 1;
public:
	Block() : Ability(TYPE, RESPONDABLE, COST) {}
	virtual void action(Unit* caster, Battle* battle);
	~Block() {}
};

class Strike : public Ability
{
protected:
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool RESPONDABLE = true;
	static const int COST = 1;
public:
	Strike() : Ability(TYPE, RESPONDABLE, COST) {}
	virtual void action(Unit* caster, Battle* battle);
	~Strike() {}
};

class Taunt : public Ability
{
protected:
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool RESPONDABLE = false;
	static const int COST = 1;
public:
	Taunt() : Ability(TYPE, RESPONDABLE, COST) {}
	virtual void action(Unit* caster, Battle* battle);
	~Taunt() {}
};

class BattleShout : public Ability
{
protected:
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool RESPONDABLE = false;
	static const int COST = 1;
public:
	BattleShout() : Ability(TYPE, RESPONDABLE, COST) {}
	virtual void action(Unit* caster, Battle* battle);
	~BattleShout() {}
};

// SCOUT

class Shoot : public Ability
{
protected:
	static const AbilityType TYPE = ABILITY_ATTACK_RANGE;
	static const bool RESPONDABLE = true;
	static const int COST = 1;
public:
	Shoot() : Ability(TYPE, RESPONDABLE, COST) {}
	virtual void action(Unit* caster, Battle* battle);
	~Shoot() {}
};

#endif
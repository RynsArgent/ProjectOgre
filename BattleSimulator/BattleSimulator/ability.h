#ifndef __ABILITY_H__
#define __ABILITY_H__

#include "pch.h"

#include "unit.h"
#include "battle.h"

// Base class for abilities
class Ability
{
protected:
	const static AbilityAction ACT = ACTION_STANDARD;
	AbilityAction act; // Action type, standard/response skill
	AbilityType type; // Defines the category an ability belongs to
	bool respondable; // Determines whether this ability can be followed with a counterattack
	bool interruptible; // Determines whether an ability can be cancelled
	int cost; // Can perhaps later be AP cost
public:
	Ability(AbilityAction act, AbilityType type, bool respondable, bool interruptible, int cost)
		: act(act), type(type), respondable(respondable), interruptible(interruptible), cost(cost)
	{}

	// Most abilities will have their logic implemented in this function
	virtual void action(Unit* current, Unit* previous, Battle* battle) = 0;

	AbilityAction getActionType() const { return act; }
	AbilityType getAbilityType() const { return type; }

	~Ability() {}
};

class NoStandardSkill : public Ability
{
protected:
	const static AbilityAction ACT = ACTION_STANDARD;
	static const AbilityType TYPE = ABILITY_NONE;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = false;
	static const int COST = 0;
public:
	NoStandardSkill() : Ability(ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Unit* previous, Battle* battle) {}
	~NoStandardSkill() {}
};
class NoResponseSkill : public Ability
{
protected:
	const static AbilityAction ACT = ACTION_RESPONSE;
	static const AbilityType TYPE = ABILITY_NONE;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = false;
	static const int COST = 0;
public:
	NoResponseSkill() : Ability(ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Unit* previous, Battle* battle) {}
	~NoResponseSkill() {}
};

class HundredBlades : public Ability
{
protected:
	const static AbilityAction ACT = ACTION_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	HundredBlades() : Ability(ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Unit* previous, Battle* battle);
	~HundredBlades() {}
};

class Block : public Ability
{
protected:
	const static AbilityAction ACT = ACTION_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Block() : Ability(ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Unit* previous, Battle* battle);
	~Block() {}
};

class Strike : public Ability
{
protected:
	const static AbilityAction ACT = ACTION_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Strike() : Ability(ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Unit* previous, Battle* battle);
	~Strike() {}
};

class Taunt : public Ability
{
protected:
	const static AbilityAction ACT = ACTION_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Taunt() : Ability(ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Unit* previous, Battle* battle);
	~Taunt() {}
};

class BattleShout : public Ability
{
protected:
	const static AbilityAction ACT = ACTION_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	BattleShout() : Ability(ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Unit* previous, Battle* battle);
	~BattleShout() {}
};

// SCOUT

class Shoot : public Ability
{
protected:
	const static AbilityAction ACT = ACTION_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_RANGE;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Shoot() : Ability(ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Unit* previous, Battle* battle);
	~Shoot() {}
};

#endif
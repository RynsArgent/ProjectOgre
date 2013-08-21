#ifndef __ABILITY_H__
#define __ABILITY_H__

#include "pch.h"

#include "action.h"
#include "event.h"
#include <vector>

// Base class for abilities
class Ability : public Action
{
protected:
    string name;
	bool basic; // Tells whether this ability is programmed to be set as a response ability (counterattack)
	bool respondable; // Determines whether this ability can be followed with a counterattack
	bool interruptible; // Determines whether an ability can be cancelled
	int cost; // Can perhaps later be AP cost
    
	// Variables that depend on the unit during the execution of an ability
	bool cancelled;
	vector<Targeter*> targeters;
	vector<Event*> events;
public:
	static Ability* getAbility(Skill skill);
	static Skill selectSkill(Unit* unit);
	static bool isAbleToRespond(Ability* previous, Ability* current);
    
	Ability(const string & name, ActionType act, AbilityType type, bool basic, bool respondable, bool interruptible, int cost)
    : Action(name, act, type), respondable(respondable), basic(basic), interruptible(interruptible), cost(cost), cancelled(false), targeters(), events()
	{
	}
    
	// Most abilities will have their logic implemented in this function
	virtual void action(Ability* previous, Unit* current, Battle* battle) {
        this->source = current;
        this->battle = battle;
		Event* event = new Event(this);
    };
    
    bool isRespondable() const { return respondable; }
    bool isInterruptible() const { return interruptible; }
    bool isCancelled() const { return cancelled; }
    void setCancelled(bool value) { cancelled = value; }
	vector<Targeter*> getTargeters() const { return targeters; }
	void setTargeters(const vector<Targeter*> & value) { targeters = value; }
	Targeter* retrieveFirstPrimaryTargeter() const;

	// This is used to check whether the unit is stunned, sleep, fleeing, ect.
	// Because if the unit is, the ability is probably cancelled
    bool checkpoint(Unit* current);
    
    virtual void print() const;
    
	~Ability();
};

class NoSkill : public Ability
{
protected:
	static const ActionType ACT = ACTION_NONE;
	static const AbilityType TYPE = ABILITY_NONE;
	static const bool BASIC = true;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	NoSkill() : Ability("No Skill", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* previous, Unit* current, Battle* battle) {};
	~NoSkill() {}
};

class HundredBlades : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	HundredBlades() : Ability("Hundred Blades", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~HundredBlades() {}
};

class Block : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Block() : Ability("Block", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Block() {}
};

class Strike : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = true;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Strike() : Ability("Strike", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Strike() {}
};

class Taunt : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Taunt() : Ability("Taunt", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Taunt() {}
};

class BattleShout : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	BattleShout() : Ability("Battle Shout", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~BattleShout() {}
};

// SCOUT

class Shoot : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_RANGE;
	static const bool BASIC = true;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Shoot() : Ability("Shoot", ACT, TYPE, BASIC,RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Shoot() {}
};

class Heal : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = true;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Heal() : Ability("Heal", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Heal() {}
};

class Cleanse : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Cleanse() : Ability("Cleanse", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Cleanse() {}
};

#endif

#ifndef __ABILITY_H__
#define __ABILITY_H__

#include "pch.h"

#include "action.h"
#include <vector>

// Base class for abilities
class Ability : public Action
{
protected:
    string name;
	AbilityAction act; // Action type, standard/response skill
	AbilityType type; // Defines the category an ability belongs to
	bool respondable; // Determines whether this ability can be followed with a counterattack
	bool interruptible; // Determines whether an ability can be cancelled
	int cost; // Can perhaps later be AP cost
    
	// Variables that depend on the unit during the execution of an ability
	bool cancelled;
	Targeter* targeter;
	vector<Applier*> appliers;
public:
	static Ability* getAbility(Skill skill);
    
	Ability(const string & name, AbilityAction act, AbilityType type, bool respondable, bool interruptible, int cost)
    : Action(), name(name), act(act), type(type), respondable(respondable), interruptible(interruptible), cost(cost), cancelled(false), targeter(NULL), appliers()
	{
	}
    
	// Most abilities will have their logic implemented in this function
	virtual void action(Unit* current, Battle* battle) {
        this->source = current;
        this->battle = battle;
    };
    
    string getName() const { return name; }
    bool isRespondable() const { return respondable; }
    bool isInterruptible() const { return interruptible; }
    bool isCancelled() const { return cancelled; }
    void setCancelled(bool value) { cancelled = value; }
	Targeter* getTargeter() const { return targeter; }
	void setTargeter(Targeter* value) { targeter = value; }
	vector<Applier*> getAppliers() { return appliers; }
	void setAppliers(const vector<Applier*> & value) { appliers = value; }

    bool checkpoint(Unit* current);
    
	AbilityAction getActionType() const { return act; }
	AbilityType getAbilityType() const { return type; }
    
    virtual void print() const;
    
	~Ability() 
	{
		if (targeter) delete targeter;
		for (int i = 0; i < appliers.size(); ++i)
			delete appliers[i];
	}
};

class NoStandardSkill : public Ability
{
protected:
	const static AbilityAction ACT = ACTION_STANDARD;
	static const AbilityType TYPE = ABILITY_NONE;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	NoStandardSkill() : Ability("No Standard Skill", ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Battle* battle) {};
	~NoStandardSkill() {}
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
	HundredBlades() : Ability("Hundred Blades", ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Battle* battle);
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
	Block() : Ability("Block", ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Battle* battle);
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
	Strike() : Ability("Strike", ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Battle* battle);
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
	Taunt() : Ability("Taunt", ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Battle* battle);
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
	BattleShout() : Ability("Battle Shout", ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Battle* battle);
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
	Shoot() : Ability("Shoot", ACT, TYPE, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Unit* current, Battle* battle);
	~Shoot() {}
};

#endif

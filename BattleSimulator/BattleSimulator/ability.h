#ifndef __ABILITY_H__
#define __ABILITY_H__

#include "pch.h"

#include "action.h"
#include <fstream>
#include <vector>

// To add an ability: 
// -go to pch.h and add the ability to the Skill enum
// -go to ability.h and create the new derived class for the Skill
// -go to ability.cpp and update the toStringSkill (NO NEED TO ATM)
// -go to ability.cpp and update the getAbility to instantiate the new object for skill
// -go to ability.cpp and write the logic in the execute function

// Base class for abilities
class Ability : public Action
{
protected:
	bool basic; // Tells whether this ability is programmed to be set as a response ability (counterattack)
	bool respondable; // Determines whether this ability can be followed with a counterattack
	bool interruptible; // Determines whether an ability can be cancelled
	int cost; // Can perhaps later be AP cost
    
	// Variables that depend on the unit during the execution of an ability
	bool cancelled;
	vector<Targeter*> targeters;
	vector<Event*> events;
public:
	static string toStringSkill(Skill skill);
	static Ability* getAbility(Skill skill);
	static Skill selectSkill(Unit* unit);
	static bool isAbleToRespond(Ability* previous, Ability* current);
    
	Ability(const string & name, ActionType act, AbilityType type, bool basic, bool respondable, bool interruptible, int cost)
    : Action(name, act, type), respondable(respondable), basic(basic), interruptible(interruptible), cost(cost), cancelled(false), targeters(), events()
	{
	}
    
	// Most abilities will have their logic implemented in this function
	virtual void action(Ability* previous, Unit* current, Battle* battle);
    
    bool isRespondable() const { return respondable; }
    bool isInterruptible() const { return interruptible; }
    bool isCancelled() const { return cancelled; }
    void setCancelled(bool value) { cancelled = value; }
	vector<Targeter*> getTargeters() const { return targeters; }
	void setTargeters(const vector<Targeter*> & value) { targeters = value; }

	// This is used to check whether the unit is stunned, sleep, fleeing, ect.
	// Because if the unit is, the ability is probably cancelled
    bool checkpoint(Unit* current);
    
    virtual void print(ostream& out) const;
    
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
	virtual void action(Ability* previous, Unit* current, Battle* battle) {
	}
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
	Shoot() : Ability("Shoot", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Shoot() {}
};

class Haste : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Haste() : Ability("Haste", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Haste() {}
};

class Scope : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Scope() : Ability("Scope", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Scope() {}
};

class TangleTrap : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	TangleTrap() : Ability("Tangle Trap", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~TangleTrap() {}
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

class Regeneration : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = true;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Regeneration() : Ability("Regeneration", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Regeneration() {}
};

class Blind : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = true;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Blind() : Ability("Blind", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Blind() {}
};

class Barrier : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Barrier() : Ability("Barrier", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Barrier() {}
};

class Polymorph : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Polymorph() : Ability("Polymorph", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Polymorph() {}
};

class Fireball : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Fireball() : Ability("Fireball", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Fireball() {}
};

class WaterJet : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	WaterJet() : Ability("Water Jet", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~WaterJet() {}
};

class AcidDart : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	AcidDart() : Ability("Acid Dart", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~AcidDart() {}
};

class FrostShard : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	FrostShard() : Ability("Frost Shard", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~FrostShard() {}
};

class LightningBolt : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	LightningBolt() : Ability("Lightning Bolt", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~LightningBolt() {}
};

class Slash : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Slash() : Ability("Slash", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Slash() {}
};

class Provoke : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Provoke() : Ability("Provoke", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Provoke() {}
};

class DemoralizingShout : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	DemoralizingShout() : Ability("Demoralizing Shout", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~DemoralizingShout() {}
};

#endif

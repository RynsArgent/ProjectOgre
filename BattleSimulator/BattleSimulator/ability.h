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
public:
	static string toStringSkill(Skill skill);
	static Ability* getAbility(Skill skill);
	static Skill selectSkill(Unit* unit);
	static bool isAbleToRespond(Battle* battle, Ability* previous, Unit* prevSource, Ability* current, Unit* curSource);
    
	Ability(const string & name, ActionType act, AbilityType type, bool basic, bool respondable, bool interruptible, int cost)
    : Action(name, act, type), respondable(respondable), basic(basic), interruptible(interruptible), cost(cost), cancelled(false)
	{
	}
    
	// Most abilities will have their logic implemented in this function
	virtual void action(Ability* previous, Unit* current, Battle* battle);
    
    bool isRespondable() const { return respondable; }
    void setRespondable(bool value) { respondable = value; }
    bool isInterruptible() const { return interruptible; }
    void setInterruptible(bool value) { interruptible = value; }
    bool isCancelled() const { return cancelled; }
    void setCancelled(bool value) { cancelled = value; }

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

class Mend : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = true;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Mend() : Ability("Mend", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Mend() {}
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

class Flash : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = true;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Flash() : Ability("Flash", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Flash() {}
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

class ArcaneBolt : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MAGIC;
	static const bool BASIC = true;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	ArcaneBolt() : Ability("Arcane Bolt", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~ArcaneBolt() {}
};

class Fireball : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MAGIC;
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
	static const AbilityType TYPE = ABILITY_ATTACK_MAGIC;
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
	static const AbilityType TYPE = ABILITY_ATTACK_MAGIC;
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
	static const AbilityType TYPE = ABILITY_ATTACK_MAGIC;
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
	static const AbilityType TYPE = ABILITY_ATTACK_MAGIC;
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

class Charge : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Charge() : Ability("Charge", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Charge() {}
};

class Rally : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Rally() : Ability("Rally", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Rally() {}
};

class Challenge : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Challenge() : Ability("Challenge", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Challenge() {}
};

class Flurry : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Flurry() : Ability("Flurry", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Flurry() {}
};

class PowerAttack : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	PowerAttack() : Ability("Power Attack", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~PowerAttack() {}
};

class Lasso : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_RANGE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Lasso() : Ability("Lasso", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Lasso() {}
};

class Piercethrough : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Piercethrough() : Ability("Piercethrough", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Piercethrough() {}
};

class Feint : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Feint() : Ability("Feint", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Feint() {}
};

class VenomousStrike : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	VenomousStrike() : Ability("Venomous Strike", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~VenomousStrike() {}
};

class CripplingShot : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_RANGE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	CripplingShot() : Ability("Crippling Shot", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~CripplingShot() {}
};

class ConfuseTrap : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	ConfuseTrap() : Ability("Confuse Trap", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~ConfuseTrap() {}
};

class HuntersMark : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_RANGE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	HuntersMark() : Ability("Hunter's Mark", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~HuntersMark() {}
};

class TranquilizingShot : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_RANGE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	TranquilizingShot() : Ability("Tranquilizing Shot", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~TranquilizingShot() {}
};

class CharmTrap : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	CharmTrap() : Ability("Charm Trap", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~CharmTrap() {}
};

class QuickNock : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	QuickNock() : Ability("Quick Nock", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~QuickNock() {}
};

class RapidShot : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_RANGE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	RapidShot() : Ability("Rapid Shot", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~RapidShot() {}
};

class Volley : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_RANGE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Volley() : Ability("Volley", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Volley() {}
};

class Snipe : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_RANGE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Snipe() : Ability("Snipe", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Snipe() {}
};

class Reformation : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = true;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Reformation() : Ability("Reformation", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Reformation() {}
};

class Lullaby : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Lullaby() : Ability("Lullaby", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Lullaby() {}
};

class BalladOfHeroes : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	BalladOfHeroes() : Ability("Ballad Of Heroes", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~BalladOfHeroes() {}
};

class RequiemOfWar : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	RequiemOfWar() : Ability("Requiem of War", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~RequiemOfWar() {}
};

class Banish : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MAGIC;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Banish() : Ability("Banish", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Banish() {}
};

class GreaterHeal : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	GreaterHeal() : Ability("Greater Heal", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~GreaterHeal() {}
};

class Raise : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_SPECIAL;
	static const bool BASIC = false;
	static const bool RESPONDABLE = false;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Raise() : Ability("Raise", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Raise() {}
};

class LightningKicks : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	LightningKicks() : Ability("Lightning Kicks", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~LightningKicks() {}
};

class HurricaneKick : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	HurricaneKick() : Ability("Hurricane Kick", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~HurricaneKick() {}
};

class Roundhouse : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	Roundhouse() : Ability("Roundhouse", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~Roundhouse() {}
};

class LegSweep : public Ability
{
protected:
	static const ActionType ACT = ABILITY_STANDARD;
	static const AbilityType TYPE = ABILITY_ATTACK_MELEE;
	static const bool BASIC = false;
	static const bool RESPONDABLE = true;
	static const bool INTERRUPTIBLE = true;
	static const int COST = 1;
public:
	LegSweep() : Ability("Leg Sweep", ACT, TYPE, BASIC, RESPONDABLE, INTERRUPTIBLE, COST) {}
	virtual void action(Ability* previous, Unit* current, Battle* battle);
	~LegSweep() {}
};

#endif

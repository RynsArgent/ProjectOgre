#ifndef __STATUS_H__
#define __STATUS_H__

#include "pch.h"

#include "action.h"
#include "event.h"
#include "unit.h"
#include <fstream>

// All Status Effects share this result object when merging data
// The value variables are ambiguous and can be used for anything 
// and are specified based on the derived class
struct StatusInstance
{
	int timer;
	int stacks;

	StatusInstance() : timer(0), stacks(0) {}
};

class Status
{
protected:
	Effect* effect;
	string subname;
	Unit* target;
	StatusBenefit benefit;
	StatusMatch match;

	int numStacks;
	int maxStacks;
    bool dispellable;

	bool timed;
	int timer;

	// Used for efficient memory usage
	bool clean; 
	Status* merger;
public:
	Status(Effect* effect, const string & subname, Unit* target = NULL, StatusBenefit benefit = NEUTRAL, StatusMatch match = STATUS_UNMATCHABLE, bool dispellable = true, bool timed = false, int timer = 0, int numStacks = 0, int maxStacks = 20)
		: effect(effect), subname(subname), target(target), benefit(benefit), match(match), 
		numStacks(numStacks), maxStacks(maxStacks), dispellable(dispellable), timed(false), timer(timer), clean(false), merger(NULL)
	{
	}

	Effect* getEffect() const {
		return effect;
	}

	string getSubname() const {
		return subname;
	}

	StatusBenefit getBenefit() const {
		return benefit;
	}
	
	Unit* getTarget() const {
		return target;
	}
	
	void setNumStacks(int value) {
		numStacks = value;
	}

	int getNumStacks() const {
		return numStacks;
	}
	
	void setMaxStacks(int value) {
		maxStacks = value;
	}

	int getMaxStacks() const {
		return maxStacks;
	}

	void setDispellable(bool value) {
		dispellable = value;
	}

	bool isDispellable() const {
		return dispellable;
	}

	void setTimed(bool value1, int value2) {
		timed = value1;
		timer = value2;
	}

	void setTimed(bool value) {
		timed = value;
	}   

	bool isTimed() const {
		return timed;
	}

	void setTimer(int value) {
		timer = value;
	}

	int getTimer() const {
		return timer;
	}
    
	void setMerger(Status* value) {
		merger = value;
	}

	Status* getMerger() const {
		return merger;
	}

	bool needsCleaning() const {
		return clean;
	}
	
	// Adds to numStacks and returns the actual number added (due to a max limit),
	// NOTE: the number returned is assumed that this object is added its stacks to the original
	int addStacks(int stacks) {
		int add = numStacks;
		if (stacks + add > maxStacks)
			add = maxStacks - stacks;
		stacks += add;
		numStacks = stacks;
		return add;
	}

	// Base function that at its most basic updates the timer, Status Effects will extend
	// the functions they need
	virtual bool hasExpired() const;
	
	// Checks to see if two status effects can be merged together. 
	// Assumption: The two status effects have the same target
	bool canMergeWith(Status* rhs) const;

	// This merges by "Effect trigger unit" Statuses, If different
	// Poison trigger Status are applied for example, this is not used.
	//
	// Note: Instead of stacking, resetting the buff can be done by not using
	//		StatusInstances
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult) = 0;

	// This deals with Status Effects that have effects that do something at the moment of creation or disappearance
	// (i.e. Doom III, Unit dies after it expires; would have to check timer == 0 in case of it activity on a dispel)
	// (i.e. Attribute modifiers, modifies stats and then restores them on expiration, specifically targeting Unit variables)
	//
	// Note: In order to support merging (stacking), onSpawn() must be implemented being called in the merging process as well
	//		onKill() must be implemented not being called in the merging process and can handle 2+ stacks in the final end process.
	virtual void onSpawn();
	virtual void onKill();
    
	// This deals with Status Effects that occur every round. 
	// (i.e. Poison/Burn/Bleed)
	virtual void onRound();
    
	// This deals with events that can have a roller's chance to succeed or fail
	// (i.e. Blind will affect the chances of success for an attack)
	virtual void onPrePerformHit(Event* event);

	// This deals with events that have already determined a roller's chance of success or fail
	// (i.e. Convert a miss for an attack to an auto-hit)
	virtual void onPostPerformHit(Event* event);
	
	// This deals with events that can have a target be victim to a chance of success or fail
	// (i.e. Evasion increases chance of failure for an attack to succeed on self)
	virtual void onPreReactHit(Event* event);
	
	// This deals with events that have already a target be victim to a chance of success or fail
	// (i.e. Convert a successful hit to an auto-miss)
	virtual void onPostReactHit(Event* event);

	// This deals with the triggers after damage is applied. 
	// (i.e. Extra damage against beasts)
	virtual void onPreApplyDamage(Damage* applier);
    
	// This deals with the triggers before damage is about to be applied. 
	// (i.e. Damage dealt heals attacker)
	virtual void onPostApplyDamage(Damage* applier);
    
	// This deals with the triggers before damage is about to be applied. 
	// (i.e. Damage Prevention)
	virtual void onPreReceiveDamage(Damage* applier);
    
	// This deals with the triggers after damage is applied. 
	// (i.e. Heal half of damage taken)
	virtual void onPostReceiveDamage(Damage* applier);

	// This deals with the triggers when candidate units are found.
    // Targets are not exactly chosen yet.
    // (i.e. Confused target also adds all ally units as candidates and set to TARGET_RANDOM)
    // (i.e. Taunted unit adds taunters in candidate list to the priority list)
	virtual void onPreFindTarget(Targeter* system);
    
    // This deals with the triggers when chosen units are selected from
    // the candidate list
    // (i.e. Stun chosen target moment it is selected)
    virtual void onPostFindTarget(Targeter* system);
    
    // This deals with the triggers when a unit becomes a candidate target.
    // (i.e. Bard removes itself from the candidate list cause it does jack)
	virtual void onPreBecomeTarget(Targeter* system);
    
	// This deals with the triggers when a Unit is selected as a chosen target
	// (i.e. Bard removes itself as a target for the first time cause it's so friendly)
	virtual void onPostBecomeTarget(Targeter* system);
    
    // This deals with executing the status effects that need to be checked after triggers.
    // This is mostly for status effects that render a unit unable to act.
    // (i.e. cancel the ability if a unit is stunned (beginning, midway, end)
    virtual void onCheckpoint(Ability* ability);

	// This deals with selecting which ability to use
	// (i.e. Charmed and Confused casters will use their basic ability)
    virtual void onSelectAbility(Unit* caster);
    
	~Status() {}
	
	friend class Effect;
};

class StatusStun : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;
public:
    StatusStun(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Stun", target, BENEFIT, MATCH, DISPELLABLE, TIMED, stacks, MAX_STACKS, MAX_STACKS)
    {}
    
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onCheckpoint(Ability* ability);
    virtual void onSelectAbility(Unit* caster);
	
    ~StatusStun() {}
};

class StatusSleep : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;
public:
    StatusSleep(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Sleep", target, BENEFIT, MATCH, DISPELLABLE, TIMED, stacks, MAX_STACKS, MAX_STACKS)
    {}
    
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onPostReceiveDamage(Damage* applier);
	virtual void onCheckpoint(Ability* ability);
    virtual void onSelectAbility(Unit* caster);
    
    ~StatusSleep() {}
};

class StatusFlee : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;
public:
    StatusFlee(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Flee", target, BENEFIT, MATCH, DISPELLABLE, TIMED, stacks, MAX_STACKS, MAX_STACKS)
    {}
    
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onSpawn();
	virtual void onCheckpoint(Ability* ability);
    virtual void onKill();
    virtual void onSelectAbility(Unit* caster);

    ~StatusFlee() {}
};

class StatusConfusion : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;    
public:
    StatusConfusion(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Confusion", target, BENEFIT, MATCH, DISPELLABLE, TIMED, stacks, MAX_STACKS, MAX_STACKS)
    {}
    
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onPreFindTarget(Targeter* system);
    virtual void onSelectAbility(Unit* caster);

    ~StatusConfusion() {}
};

class StatusCharm : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;
public:
    StatusCharm(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Charm", target, BENEFIT, MATCH, DISPELLABLE, TIMED, stacks, MAX_STACKS, MAX_STACKS)
    {}
    
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onPostReceiveDamage(Damage* applier);
	virtual void onPreFindTarget(Targeter* system);
    virtual void onSelectAbility(Unit* caster);

    ~StatusCharm() {}
};

class StatusPoison : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 5;
	static const bool DISPELLABLE = true;

	static const int TIMER = 3;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusPoison(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Poison", target, BENEFIT, MATCH, DISPELLABLE, TIMED, TIMER, stacks, MAX_STACKS), amount(AMOUNT)
	{}
	
	// Helper Functions
	void applyTimedDamage();
	
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onRound();

	~StatusPoison() {}
};

class StatusBleed : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 5;
	static const bool DISPELLABLE = true;
	
	static const int TIMER = 3;
	static const int AMOUNT = 20;

	int amount;
public:
	StatusBleed(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Bleed", target, BENEFIT, MATCH, DISPELLABLE, TIMED, TIMER, stacks, MAX_STACKS), amount(AMOUNT)
	{}
	
	// Helper Functions
	void applyTimedDamage();
	
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onRound();
    
	~StatusBleed() {}
};

class StatusBurn : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 5;
	static const bool DISPELLABLE = true;
	
	static const int TIMER = 3;
	static const int AMOUNT = 30;

	int amount;
public:
	StatusBurn(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Burn", target, BENEFIT, MATCH, DISPELLABLE, TIMED, TIMER, stacks, MAX_STACKS), amount(AMOUNT)
	{}
	
	// Helper Functions
	void applyTimedDamage();
	
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onRound();
    
	~StatusBurn() {}
};

class StatusRegeneration : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 5;
	static const bool DISPELLABLE = true;
	
	static const int TIMER = 3;
public:
	StatusRegeneration(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Regeneration", target, BENEFIT, MATCH, DISPELLABLE, TIMED, TIMER, stacks, MAX_STACKS)
	{}
	
	// Helper Functions
	void applyTimedHeal();
	
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onRound();
    
	~StatusRegeneration() {}
};

class StatusBlind : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;
public:
	StatusBlind(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Blind", target, BENEFIT, MATCH, DISPELLABLE, TIMED, stacks, MAX_STACKS, MAX_STACKS)
	{}
	
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onPrePerformHit(Event* event);

	~StatusBlind() {}
};

class StatusPolymorph : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;
public:
    StatusPolymorph(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Polymorph", target, BENEFIT, MATCH, DISPELLABLE, TIMED, stacks, MAX_STACKS, MAX_STACKS)
    {}
    
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onPostReceiveDamage(Damage* applier);
	virtual void onCheckpoint(Ability* ability);
    virtual void onSelectAbility(Unit* caster);
    
    ~StatusPolymorph() {}
};

class StatusMortality : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 20;
	static const bool DISPELLABLE = true;

	static const int TIMER = 3;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusMortality(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Mortality", target, BENEFIT, MATCH, DISPELLABLE, TIMED, stacks, MAX_STACKS, MAX_STACKS), amount(AMOUNT)
	{}

	// Helper Functions
	void modifyAttributes(int dvalue);

	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onSpawn();
	virtual void onKill();

	~StatusMortality() {}
};

class StatusBlock : public Status
{
private:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;

	static const int TIMER = 1;
	static const bool LIMITED = true;
	static const int AMOUNT = 30;

	bool limited; // If set, amount will drop on use and expire at 0. Otherwise, amount will not drop
	int amount;
public:
	StatusBlock(Effect* effect, Unit* target)
        : Status(effect, "Block", target, BENEFIT, MATCH, DISPELLABLE, TIMED, TIMER, MAX_STACKS, MAX_STACKS), limited(LIMITED), amount(AMOUNT)
	{}
	
	// Helper Functions
	virtual bool hasExpired() const;
	void applyDamagePrevention(Damage* applier);
	
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onPreReceiveDamage(Damage* applier);

	~StatusBlock() {}
};

class StatusTaunt : public Status
{
protected:
	static const StatusBenefit BENEFIT = NEUTRAL;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = false;

	static const int TIMER = 1;

	Unit* focus; 
public:
	StatusTaunt(Effect* effect, Unit* target, Unit* focus)
        : Status(effect, "Taunt", target, BENEFIT, MATCH, DISPELLABLE, TIMED, TIMER, MAX_STACKS, MAX_STACKS), focus(focus)
	{
	}
	
	// Helper Functions
	void addToPriorityList(Targeter* system) const;
	
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onPreFindTarget(Targeter* system);

	~StatusTaunt();
};

class StatusBattleShout : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;

	static const int TIMER = 1;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusBattleShout(Effect* effect, Unit* target, int stacks)
        : Status(effect, "BattleShout", target, BENEFIT, MATCH, DISPELLABLE, TIMED, TIMER, stacks, MAX_STACKS), amount(AMOUNT)
	{
	}

	// Helper Functions
	void modifyAttributes(int dvalue);
	
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onSpawn();
	virtual void onKill();

	~StatusBattleShout() {}
};

class StatusBarrier : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;

	static const int TIMER = 1;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusBarrier(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Barrier", target, BENEFIT, MATCH, DISPELLABLE, TIMED, TIMER, stacks, MAX_STACKS), amount(AMOUNT)
	{
	}
	
	// Helper Functions
	void modifyAttributes(int dvalue);
	
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onSpawn();
	virtual void onKill();

	~StatusBarrier() {}
};

class StatusHaste : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;

	static const int TIMER = 3;
	static const int AMOUNT = 1;

	int amount;
public:
	StatusHaste(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Haste", target, BENEFIT, MATCH, DISPELLABLE, TIMED, TIMER, stacks, MAX_STACKS), amount(AMOUNT)
	{
	}
	
	// Helper Functions
	void modifyAttributes(int dvalue);
	
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onSpawn();
	virtual void onKill();

	~StatusHaste() {}
};

class StatusChill : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;

	static const int TIMER = 3;
	static const int AMOUNT = 1;

	int amount;
public:
	StatusChill(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Chill", target, BENEFIT, MATCH, DISPELLABLE, TIMED, TIMER, stacks, MAX_STACKS), amount(AMOUNT)
	{
	}
	
	// Helper Functions
	void modifyAttributes(int dvalue);
	
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onSpawn();
	virtual void onKill();

	~StatusChill() {}
};

class StatusScope : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;

	static const int TIMER = 1;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusScope(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Scope", target, BENEFIT, MATCH, DISPELLABLE, TIMED, TIMER, stacks, MAX_STACKS), amount(AMOUNT)
	{}

	// Helper Functions
	
	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onPreApplyDamage(Damage* applier);

	~StatusScope() {}
};

class StatusTangleTrap : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const bool TIMED = true;
	static const int MAX_STACKS = 1;
	static const bool DISPELLABLE = true;

	static const int TIMER = 1;
public:
	StatusTangleTrap(Effect* effect, Unit* target)
        : Status(effect, "TangleTrap", target, BENEFIT, MATCH, DISPELLABLE, TIMED, TIMER, MAX_STACKS, MAX_STACKS)
	{}

	// Main Function
	virtual StatusInstance getMergeResult() const;
	virtual void onMerge(const StatusInstance & mergeResult);
	virtual void onPostBecomeTarget(Targeter* system);

	~StatusTangleTrap() {}
};

// Keeps track of ongoing Status Effects, also allows multiple status effects to be associated under one name
class Effect : public Action
{
private:
	vector<Status*> status;

	Unit* trigger; // Unit that processes Effects on its turn
public:
	Effect(Unit* source = NULL, Battle* battle = NULL, const string & name = "", Unit* trigger = NULL)
		: Action(name, EFFECT_TRIGGER, ABILITY_SPECIAL, source, battle), status(), trigger(trigger)
	{
	}

	bool isExpired() const {
		return status.size() <= 0;
	}
    
    Unit* getTrigger() const {
        return trigger;
    }
    
	void addStatus(Status* stat) {
		status.push_back(stat);
	}

	// Each ongoing status effect is processed every round starting from the origin.
	// For example, most effects that are timed will be updated here.
	void processRound() {
		for (int i = 0; i < status.size(); ++i) {
			// Process the Status effect
			status[i]->onRound();

			// Sets the clean flag and on End effects
			if (!status[i]->needsCleaning() && status[i]->hasExpired())
			{
				status[i]->onKill();
			}
		}
	}
	
	// Cleans up any expired status effects, removing links and trigger onKill
	void cleanStatus() 
	{
		// Clean up all expired status effects
		vector<Status*> nstatus(status.size());
		int c = 0;
		for (int i = 0; i < status.size(); ++i) {
			if (!status[i]->needsCleaning()) {
				nstatus[c] = status[i];
				++c;
			}
			else {
				delete status[i];
			}
		}
		nstatus.resize(c);
		status = nstatus;
	}

	/*
	Status* findMatchingStatus(Status* value)
	{
		for (int i = 0; i < status.size(); ++i) {
			if (status[i]->getSubname() == subname && 
				status[i]->getTarget() == target && 
				!status[i]->hasExpired())
				return status[i];
		}
		return NULL;
	}
	
	vector<Effect*> findEffects(const string & value)	
	{
		vector<Effect*> ret;
		for (int i = 0; i < trigger->getCurrentEffects().size(); ++i) 
			if (trigger->getCurrentEffects()[i]->getName() == value &&
				!trigger->getCurrentEffects()[i]->isExpired())
				ret.push_back(trigger->getCurrentEffects()[i]);
		return ret;
	}
	
	void merge(Effect* old) 
	{
		// Inefficient, but manageable for now
		for (int i = 0; i < status.size(); ++i)
		{
			Status* keep = status[i];
			Status* replace = old->findMatchingStatus(keep->getSubname(), keep->getTarget());
			if (replace != NULL) merge(keep, replace);
		}
		old->cleanStatus();
	}
	*/

	void merge(Status* dominant, Status* recessive) {
		StatusInstance res = recessive->getMergeResult();

		// Do not do onKill(...), for the onMerge should handle it
		dominant->onMerge(res);
		recessive->Status::onKill(); // and Unlink
		dominant->Status::onSpawn(); // Link 
	}


	void applyEffect()
	{
		for (int i = 0; i < status.size(); ++i)
			status[i]->setMerger(status[i]->getTarget()->getMatchingStatus(status[i]));
		
		for (int i = 0; i < status.size(); ++i)
		{
			if (status[i]->getMerger() != NULL)
			{
				merge(status[i], status[i]->getMerger());
			}
			else
				status[i]->onSpawn();
		}
		/*
		for (int i = 0; i < status.size(); ++i) 
			status[i]->onSpawn();
	
		// There can be more than one effect of the same name, 
		// try to merge with all the old ones
		// and update to the new one
		vector<Effect*> matches = findEffects(name);
		for (int i = 0; i < matches.size(); ++i) {
			merge(matches[i]);
		}
		*/

		if (trigger != NULL) trigger->currentEffects.push_back(this);
	}
    
	virtual void print(ostream& out) const;

	~Effect() {}
};	

#endif

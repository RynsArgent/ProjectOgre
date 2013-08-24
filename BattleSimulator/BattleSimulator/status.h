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
struct StatusMergeResult
{
	int timer;
	int val1;
	int val2;
	int val3;

	StatusMergeResult() : timer(0), val1(0), val2(0), val3(0) {}
};

class Status
{
protected:
	Effect* effect;
	string subname;
	StatusBenefit benefit;
	Unit* target;
	
    // unimplemented
    bool dispellable;

	bool timed;
	int timer;

	bool clean; // Used for efficient memory cleaning
public:
	Status(Effect* effect, const string & subname, StatusBenefit benefit = NEUTRAL, Unit* target = NULL)
		: effect(effect), subname(subname), benefit(benefit), target(target),
		dispellable(true), timed(false), timer(0), clean(false)
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
    
	bool needsCleaning() const {
		return clean;
	}

	// Base function that at its most basic updates the timer, Status Effects will extend
	// the functions they need
	virtual bool hasExpired() const;
	
	// This merges by "Effect trigger unit" Statuses, If different
	// Poison trigger Status are applied for example, this is not used.
	//
	// Note: Instead of stacking, resetting the buff can be done by not using
	//		StatusMergeResults
	virtual StatusMergeResult getMergeResult() const = 0;
	virtual void onMerge(const StatusMergeResult & mergeResult) = 0;

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
    
public:
    StatusStun(Effect* effect, const string & subname, Unit* target)
        : Status(effect, subname, DEBUFF, target)
    {}
    
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onCheckpoint(Ability* ability);
    virtual void onSelectAbility(Unit* caster);
	
    ~StatusStun() {}
};


class StatusSleep : public Status
{
private:
    
public:
    StatusSleep(Effect* effect, const string & subname, Unit* target)
		: Status(effect, subname, DEBUFF, target)
    {}
    
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onPostReceiveDamage(Damage* applier);
	virtual void onCheckpoint(Ability* ability);
    virtual void onSelectAbility(Unit* caster);
    
    ~StatusSleep() {}
};

class StatusFlee : public Status
{
private:
    
public:
    StatusFlee(Effect* effect, const string & subname, Unit* target)
		: Status(effect, subname, DEBUFF, target)
    {}
    
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onSpawn();
	virtual void onCheckpoint(Ability* ability);
    virtual void onKill();
    virtual void onSelectAbility(Unit* caster);

    ~StatusFlee() {}
};

class StatusConfusion : public Status
{
private:
    
public:
    StatusConfusion(Effect* effect, const string & subname, Unit* target)
		: Status(effect, subname, DEBUFF, target)
    {}
    
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onPreFindTarget(Targeter* system);
    virtual void onSelectAbility(Unit* caster);

    ~StatusConfusion() {}
};

class StatusCharm : public Status
{
private:
    
public:
    StatusCharm(Effect* effect, const string & subname, Unit* target)
		: Status(effect, subname, DEBUFF, target)
    {}
    
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onPostReceiveDamage(Damage* applier);
	virtual void onPreFindTarget(Targeter* system);
    virtual void onSelectAbility(Unit* caster);

    ~StatusCharm() {}
};

class StatusPoison : public Status
{
protected:
	int amount;
public:
	StatusPoison(Effect* effect, const string & subname, Unit* target, int amount)
		: Status(effect, subname, DEBUFF, target), amount(amount)
	{}
	
	// Helper Functions
	void applyTimedDamage();
	
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onRound();

	// Set Status Specific Variables
	void setAmount(int value) { amount = value; }
    
	~StatusPoison() {}
};

class StatusBlind : public Status
{
protected:
public:
	StatusBlind(Effect* effect, const string & subname, Unit* target)
		: Status(effect, subname, DEBUFF, target)
	{}
	
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onPrePerformHit(Event* event);

	~StatusBlind() {}
};

class StatusMortality : public Status
{
protected:
	int amount;
public:
	StatusMortality(Effect* effect, const string & subname, Unit* target, int amount)
		: Status(effect, subname, DEBUFF, target), amount(amount)
	{}

	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onSpawn();
	virtual void onKill();
	
	// Set Status Specific Variables
	void setAmount(int value) { amount = value; }

	~StatusMortality() {}
};

class StatusBlock : public Status
{
private:
	bool limited; // If set, amount will drop on use and expire at 0. Otherwise, amount will not drop
	int amount;
public:
	StatusBlock(Effect* effect, const string & subname, Unit* target, bool limited, int amount)
		: Status(effect, subname, BUFF, target), limited(limited), amount(amount)
	{}
	
	// Helper Functions
	virtual bool hasExpired() const;
	void applyDamagePrevention(Damage* applier);
	
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onPreReceiveDamage(Damage* applier);

	// Set Status Specific Variables
	void setLimited(bool value) { limited = value; }
	void setAmount(int value) { amount = value; }

	~StatusBlock() {}
};

class StatusTaunt : public Status
{
protected:
	Unit* focus; 
public:
	StatusTaunt(Effect* effect, const string & subname, Unit* target, Unit* focus)
		: Status(effect, subname, NEUTRAL, target), focus(focus)
	{
		dispellable = false;
	}
	
	// Helper Functions
	void addToPriorityList(Targeter* system) const;
	
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onPreFindTarget(Targeter* system);

	// Set Status Specific Variables
	void setFocus(Unit* value) { focus = value; }

	~StatusTaunt();
};

class StatusBattleShout : public Status
{
protected:
	int amount;
public:
	StatusBattleShout(Effect* effect, const string & subname, Unit* target, int amount)
		: Status(effect, subname, BUFF, target), amount(amount)
	{}

	// Helper Functions
	
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onSpawn();
	virtual void onKill();

	// Set Status Specific Variables
	void setAmount(int value) { amount = value; }

	~StatusBattleShout() {}
};

class StatusHaste : public Status
{
protected:
	int amount;
public:
	StatusHaste(Effect* effect, const string & subname, Unit* target, int amount)
		: Status(effect, subname, BUFF, target), amount(amount)
	{}

	// Helper Functions
	
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onSpawn();
	virtual void onKill();

	// Set Status Specific Variables
	void setAmount(int value) { amount = value; }

	~StatusHaste() {}
};

class StatusScope : public Status
{
protected:
	int amount;
public:
	StatusScope(Effect* effect, const string & subname, Unit* target, int amount)
		: Status(effect, subname, BUFF, target), amount(amount)
	{}

	// Helper Functions
	
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onPreApplyDamage(Damage* applier);

	// Set Status Specific Variables
	void setAmount(int value) { amount = value; }

	~StatusScope() {}
};

class StatusTangleTrap : public Status
{
protected:
public:
	StatusTangleTrap(Effect* effect, const string & subname, Unit* target)
		: Status(effect, subname, BUFF, target)
	{}

	// Helper Functions
	
	// Main Function
	virtual StatusMergeResult getMergeResult() const;
	virtual void onMerge(const StatusMergeResult & mergeResult);
	virtual void onPostBecomeTarget(Targeter* system);

	// Set Status Specific Variables

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
		cleanStatus();
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

	Status* findStatus(const string & subname, Unit* target)
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

	void merge(Status* dominant, Status* recessive) {
		StatusMergeResult res = recessive->getMergeResult();

		// Do not do onKill(...), for the merged buff should cancel it,
		// however, we do have to unlink it
		dominant->onMerge(res);
		recessive->Status::onKill();
	}

	void merge(Effect* old) 
	{
		// Inefficient, but manageable for now
		for (int i = 0; i < status.size(); ++i)
		{
			Status* keep = status[i];
			Status* replace = old->findStatus(keep->getSubname(), keep->getTarget());
			if (replace != NULL) merge(keep, replace);
		}
		old->cleanStatus();
	}

	void applyEffect()
	{
		for (int i = 0; i < status.size(); ++i) 
			status[i]->onSpawn();
	
		// There can be more than one effect of the same name, 
		// try to merge with all the old ones
		// and update to the new one
		vector<Effect*> matches = findEffects(name);
		for (int i = 0; i < matches.size(); ++i) {
			merge(matches[i]);
		}

		if (trigger != NULL) trigger->currentEffects.push_back(this);
	}
    
	virtual void print(ostream& out) const;

	~Effect() {}
};	

#endif

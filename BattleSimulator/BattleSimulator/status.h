#ifndef __STATUS_H__
#define __STATUS_H__

#include "pch.h"

#include <algorithm>
#include "unit.h"

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
	// (i.e. Doom III, Unit dies after it expires)
	// (i.e. Attribute modifiers, modifies stats and then restores them on expiration, specifically targeting Unit variables)
	//
	// Note: In order to support merging (stacking), onSpawn() must be implemented being called in the merging process as well
	//		onKill() must be implemented not being called in the merging process and handling 2 or more stacks in the final end process.
	virtual void onSpawn();
	virtual void onKill();
    
	// This deals with Status Effects that occur every round lengthwise. (i.e. Poison/Burn/Bleed)
	virtual void onRound();
    
	// This deals with the triggers before damage is about to be applied. (i.e. Damage Prevention)
	virtual void onPreApplyDamage(Damage* applier);
    
	// This deals with the triggers after damage is applied. (i.e. Extra damage against beasts)
	virtual void onPostApplyDamage(Damage* applier);
    
	// This deals with the triggers before damage is about to be applied. (i.e. Damage dealt heals attacker)
	virtual void onPreReceiveDamage(Damage* applier);
    
	// This deals with the triggers after damage is applied. (i.e. Heal half of damage taken)
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
    // (i.e. cancel the ability if a unit is stunned (beginning, midway, end)
    // This is mostly for status effects that render a unit unable to act.
    virtual void onCheckpoint(Ability* ability);

	// This deals with selecting which ability to use
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
	{}
	
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

/*
class StatusAttackResponse : public Status
{
protected:
	static const StatusType TYPE = STATUS_ABILITY_RESPONSE;
	
	Skill skill;
	bool preemptive; // If set, respond to the enemy ability before it applies to you
	bool limited; // If set, will expire based on number of uses
	int amount;
public:
	StatusAttackResponse(const string & name, StatusBenefit benefit, Unit* target, Skill skill, bool preemptive, bool limited, int amount)
		: Status(name, benefit, TYPE, target), skill(skill), preemptive(preemptive), limited(limited), amount(amount)
	{ onSpawn(); }
	
	// Functions to use Status Effect
	virtual bool hasExpired() const;
	void applyAbility(Unit* caster, Battle* battle);

	// Set Status Specific Variables
	void setSkill(Skill value) { skill = value; }
	void setPreemptive(bool value) { preemptive = value; }
	void setLimited(bool value) { limited = value; }
	void setAmount(int value) { amount = value; }

	~StatusAttackResponse() {}
};
*/

// Keeps track of ongoing Status Effects, also allows multiple status effects to be associated under one name
class Effect
{
private:
	string name;
    Battle* battle;
	vector<Status*> status;

	Unit* origin;
	Unit* trigger; // Unit that processes Effects on its turn
    
	bool clean; // Used for efficient cleaning
public:
	Effect(const string & name = "", Unit* origin = NULL, Unit* trigger = NULL, Battle* battle = NULL)
    : name(name), battle(battle), status(), origin(origin), trigger(trigger), clean(false)
	{
	}
    
	bool isExpired() const {
		return clean;
	}

	string getName() const {
		return name;
	}
    
    Unit* getOrigin() const {
        return origin;
    }

    Unit* getTrigger() const {
        return trigger;
    }
    
    Battle* getBattle() const {
        return battle;
    }
    
	void addStatus(Status* stat) {
		status.push_back(stat);
	}

	// Each ongoing status effect is processed every round starting from the origin.
	// For examples, most effects are timed and will be updated here.
	void processRound() {
		for (int i = 0; i < status.size(); ++i) {
			// Process the Status effect
			status[i]->onRound();
            
			// Flag certain status effects to be cleaned at the end if it is expired
			if (status[i]->hasExpired())
			{
				// Sets the clean flag and on End effects
				status[i]->onKill();
				
				// Remove Status buff/debuff link from target
				Unit* target = status[i]->getTarget();
				if (target != NULL) {
					target->currentStatus.erase(find(target->currentStatus.begin(), target->currentStatus.end(), status[i]));
				}
			}
		}
        
		// Clean up all expired status effects
		vector<Status*> nstatus(status.size());
		int c = 0;
		for (int i = 0; i < status.size(); ++i) {
			if (!status[i]->needsCleaning()) {
				nstatus[c] = status[i];
				++c;
			}
			else delete status[i];
		}
		nstatus.resize(c);
		status = nstatus;
        
		// If there are no more ongoing status effects, set container's clean flag
		if (status.size() <= 0)
			end();
	}
	
	Status* findStatus(const string & value)
	{
		for (int i = 0; i < status.size(); ++i) 
			if (status[i]->getSubname() == value && !status[i]->hasExpired())
				return status[i];
		return NULL;
	}
	
	Effect* findEffect(const string & value)
	{
		for (int i = 0; i < trigger->getCurrentEffects().size(); ++i) 
			if (trigger->getCurrentEffects()[i]->getName() == value &&
				!trigger->getCurrentEffects()[i]->isExpired())
				return trigger->getCurrentEffects()[i];
		return NULL;
	}

	void merge(Status* dominant, Status* recessive) {
		StatusMergeResult res = recessive->getMergeResult();

		// Do not do onKill(...), for the merged buff should cancel it
		dominant->onMerge(res);
	}

	void merge(Effect* old) 
	{
		// Inefficient, but manageable for now
		for (int i = 0; i < status.size(); ++i)
		{
			Status* keep = status[i];
			Status* replace = old->findStatus(keep->getSubname());
			if (replace != NULL) merge(keep, replace);
		}

		// Remove all non-matching and matching effects. This works based on the assumption that the effect
		// will always share the same set of status effects.
		for (int i = 0; i < old->status.size(); ++i) {
			Status* recessive = old->status[i];
			// Remove Status buff/debuff link from target
			Unit* target = recessive->getTarget();
			if (target != NULL) {
				target->currentStatus.erase(find(target->currentStatus.begin(), target->currentStatus.end(), recessive));
			}
			delete recessive;
		}
		old->status.clear();
		old->end();
	}

	void applyEffect()
	{
		for (int i = 0; i < status.size(); ++i)
			status[i]->onSpawn();
	
		Effect* match = findEffect(name);
		if (match != NULL)
			merge(match);

		if (trigger != NULL) trigger->currentEffects.push_back(this);
	}

	bool needsCleaning() const {
		return clean;
	}
    
	void end() {
		clean = true;
	}
    
	~Effect() {}
};	

#endif

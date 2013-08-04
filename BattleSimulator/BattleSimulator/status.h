#ifndef __STATUS_H__
#define __STATUS_H__

#include "pch.h"

#include <algorithm>
#include "unit.h"

class Status
{
protected:
	Effect* effect;
	string name;
	StatusBenefit benefit;
	Unit* target;
	
	// unremovable, stacwwwwwwkable, and linked are unimplemented
	bool unremovable;
	bool stackable; 

	bool timed;
	int timer;
	bool linked; // For when a buff/debuff has multiple status effects, if one gets removed, if linked, it removed the other linked effects

	bool clean; // Used for efficient memory cleaning
public:
	Status(Effect* effect, const string & name, StatusBenefit benefit = NEUTRAL, Unit* target = NULL)
		: effect(effect), name(name), benefit(benefit), target(target),
		unremovable(false), stackable(false), timed(false), timer(0), linked(false), clean(false)
	{
		if (target != NULL) target->currentStatus.push_back(this);
	}

	Effect* getEffect() const {
		return effect;
	}

	string getName() const {
		return name;
	}

	StatusBenefit getBenefit() const {
		return benefit;
	}
	
	Unit* getTarget() const {
		return target;
	}
	
	void setUnremovable(bool value) {
		unremovable = value;
	}

	bool isUnremovable() const {
		return unremovable;
	}

	void setStackable(bool value) {
		stackable = value;
	}

	bool isStackable() const {
		return stackable;
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
	
	void setLinked(bool value) {
		linked = value;
	}

	int isLinked() const {
		return linked;
	}

	bool needsCleaning() const {
		return clean;
	}

	// Base function that at its most basic updates the timer, Status Effects will extend
	// the functions they need
	virtual bool hasExpired() const;

	// This deals with Status Effects that have effects that do something at the moment of creation or disappearance
	// (i.e. Doom III, Unit dies after it expires)
	// (i.e. Attribute modifiers, modifies stats and then restores them on expiration, specifically targeting Unit variables)
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
	// This deals with the triggers before a chosen targets are selected
	// (i.e. Targeter is forced to deal with a prioritized taunting target before any other candidates)
	virtual void onPreTarget(Targeter* system);
	// This deals with that triggers when a Unit was just selected as a target (primary or secondary)
	// (i.e. Bard removes itself as a target for the first time cause it's so friendly)
	virtual void onPostTarget(Targeter* system);

	~Status() {}
	
	friend class Effect;
};

class StatusBlock : public Status
{
private:
	bool limited; // If set, amount will drop on use and expire at 0. Otherwise, amount will not drop
	int amount;
public:
	StatusBlock(Effect* effect, const string & name, Unit* target, bool limited, int amount)
		: Status(effect, name, BUFF, target), limited(limited), amount(amount)
	{}
	
	// Helper Functions
	virtual bool hasExpired() const;
	void applyDamagePrevention(Damage* applier);
	
	// Main Function
	virtual void onPreReceiveDamage(Damage* applier);
	
	// Set Status Specific Variables
	void setLimited(bool value) { limited = value; }
	void setAmount(int value) { amount = value; }

	~StatusBlock() {}
};

class StatusPoison : public Status
{
protected:
	int amount;
public:
	StatusPoison(Effect* effect, const string & name, Unit* target, int amount)
		: Status(effect, name, DEBUFF, target), amount(amount)
	{}
	
	// Helper Functions
	void applyTimedDamage();
	
	// Main Function
	virtual void onRound();
	
	// Set Status Specific Variables
	void setAmount(int value) { amount = value; }

	~StatusPoison() {}
};

class StatusTaunt : public Status
{
protected:
	Unit* focus; 
public:
	StatusTaunt(Effect* effect, const string & name, Unit* target, Unit* focus)
		: Status(effect, name, NEUTRAL, target), focus(focus)
	{}
	
	// Helper Functions
	void addToPriorityList(Targeter* system) const;
	
	// Main Function
	virtual void onPreTarget(Targeter* system);

	// Set Status Specific Variables
	void setFocus(Unit* value) { focus = value; }

	~StatusTaunt();
};

class StatusBattleShout : public Status
{
protected:
	int amount;
public:
	StatusBattleShout(Effect* effect, const string & name, Unit* target, int amount)
		: Status(effect, name, BUFF, target), amount(amount)
	{}

	// Helper Functions
	
	// Main Function
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
	{}
	
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
	Unit* origin;

	vector<Status*> status;

	bool clean; // Used for efficient memory cleaning
public:
	Effect(const string & name = "", Unit* origin = NULL) 
		: name(name), origin(origin), status(), clean(false)
	{
		if (origin != NULL) origin->currentEffects.push_back(this);
	}

	string getName() const {
		return name;
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

	bool needsCleaning() const {
		return clean;
	}

	void end() {
		clean = true;
	}

	~Effect() {}
};

#endif

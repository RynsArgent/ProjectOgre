#ifndef __STATUS_H__
#define __STATUS_H__

#include "pch.h"

#include <algorithm>
#include "unit.h"

class Status
{
protected:
	string name;
	StatusType type;
	StatusBenefit benefit;
	Unit* target;
	
	// unremovable, stackable, and linked are unimplemented
	bool unremovable;
	bool stackable; 

	bool timed;
	int timer;
	bool linked; // For when a buff/debuff has multiple status effects, if one gets removed, if linked, it removed the other linked effects

	bool clean; // Used for efficient memory cleaning
public:
	Status(const string & name = "", StatusBenefit benefit = NEUTRAL, StatusType type = STATUS_NONE, Unit* target = NULL)
		: name(name), benefit(benefit), type(type), target(target),
		unremovable(false), stackable(false), timed(false), timer(0), linked(false), clean(false)
	{
		if (target != NULL) target->currentStatus.push_back(this);
	}

	string getName() const {
		return name;
	}

	StatusBenefit getBenefit() const {
		return benefit;
	}
	
	StatusType getType() const {
		return type;
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

	// Base function that at its most basic updates the timer
	virtual bool hasExpired() const;
	virtual void init();
	virtual void processRound();
	virtual void end();

	~Status() {}

	friend class Effect;
};

class StatusDamagePrevention : public Status
{
protected:
	static const StatusType TYPE = STATUS_DAMAGE_PREVENTION;

	int amount;
	vector<DamageType> preventedTypes;
public:
	StatusDamagePrevention(const string & name, StatusBenefit benefit, Unit* target, int amount, const vector<DamageType> & preventedTypes)
		: Status(name, benefit, TYPE, target), amount(amount), preventedTypes(preventedTypes)
	{}
	
	// Functions to use Status Effect
	virtual bool hasExpired() const;
	int applyDamagePrevention(int appliedDamage, vector<DamageType> appliedTypes);
	
	// Set Status Specific Variables
	void setAmount(int value) { amount = value; }
	void setPreventedTypes(const vector<DamageType> & value) { preventedTypes = value; }

	~StatusDamagePrevention() {}
};

class StatusDamageOverTime : public Status
{
protected:
	static const StatusType TYPE = STATUS_DAMAGE_OVER_TIME;

	int amount;
	vector<DamageType> damageTypes;
public:
	StatusDamageOverTime(const string & name, StatusBenefit benefit, Unit* target, int amount, const vector<DamageType> & damageTypes)
		: Status(name, benefit, TYPE, target), amount(amount), damageTypes(damageTypes)
	{}
	
	// Functions to use Status Effect
	virtual void processRound();
	void applyTimedDamage();
	
	// Set Status Specific Variables
	void setAmount(int value) { amount = value; }
	void setDamageTypes(const vector<DamageType> & value) { damageTypes = value; }

	~StatusDamageOverTime() {}
};

class StatusTaunt : public Status
{
protected:
	static const StatusType TYPE = STATUS_TAUNT;

	Unit* focus; 
public:
	StatusTaunt(const string & name, StatusBenefit benefit, Unit* target, Unit* focus)
		: Status(name, benefit, TYPE, target), focus(focus)
	{}
	
	// Functions to use Status Effect
	Unit* getFocus() const { return focus; }
	
	// Set Status Specific Variables
	void setFocus(Unit* value) { focus = value; }

	~StatusTaunt();
};

class StatusAttackBonus : public Status
{
protected:
	static const StatusType TYPE = STATUS_ATTACK_BONUS;

	int amount;
public:
	StatusAttackBonus(const string & name, StatusBenefit benefit, Unit* target, int amount)
		: Status(name, benefit, TYPE, target), amount(amount)
	{}

	// Functions to use Status Effect
	virtual void init();
	virtual void end();
	
	// Set Status Specific Variables
	void setAmount(int value) { amount = value; }

	~StatusAttackBonus() {}
};

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
			status[i]->processRound();

			// Flag certain status effects to be cleaned at the end if it is expired
			if (status[i]->hasExpired())
			{
				// Sets the clean flag and on End effects
				status[i]->end();
				
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
};

#endif

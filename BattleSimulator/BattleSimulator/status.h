#pragma once

#include "pch.h"

#include <algorithm>
#include "unit.h"

enum StatusType { STATUS_NONE, STATUS_DAMAGE_PREVENTION, STATUS_DAMAGE_OVER_TIME, STATUS_TAUNT, STATUS_ATTACK_BONUS };
enum StatusBenefit { NEUTRAL, BUFF, DEBUFF };

class Status
{
protected:
	string name;
	StatusType type;
	StatusBenefit benefit;
	Unit* target;
	
	bool stackable; // unused
	bool timed;
	int timer;
	bool linked; // unused

	bool clean;
public:
	Status(const string & name = "", StatusBenefit benefit = NEUTRAL, StatusType type = STATUS_NONE, Unit* target = nullptr)
		: name(name), benefit(benefit), type(type), target(target),
		stackable(false), timed(false), timer(0), linked(false), clean(false)
	{
		if (target != nullptr) target->currentStatus.push_back(this);
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

	void setStackable(bool value) {
		stackable = true;
	}

	bool isStackable() const {
		return stackable;
	}

	void setTimed(bool value) {
		timed = true;
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

// Keeps track of ongoing Status Effects
class Effect
{
private:
	string name;
	Unit* origin;

	vector<Status*> status;

	bool clean;
public:
	Effect(const string & name = "", Unit* origin = nullptr) 
		: name(name), origin(origin), status(), clean(false)
	{
		if (origin != nullptr) origin->currentEffects.push_back(this);
	}

	string getName() const {
		return name;
	}

	void addStatus(Status* stat) {
		status.push_back(stat);
	}

	void processRound() {
		for (int i = 0; i < status.size(); ++i) {
			status[i]->processRound();
			if (status[i]->hasExpired())
			{
				status[i]->end();
				
				Unit* target = status[i]->getTarget();
				if (target != nullptr) {
					target->currentStatus.erase(find(target->currentStatus.begin(), target->currentStatus.end(), status[i]));
				}
			}
		}

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

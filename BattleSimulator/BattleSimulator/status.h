#pragma once

#include "unit.h"

enum StatusType { STATUS_PREVENTION, STATUS_DAMAGE_OVER_TIME };
enum StatusBenefit { NEUTRAL, BUFF, DEBUFF };

class Status
{
protected:
	string name;
	bool stackable;
	int stacks;

	bool timed;
	int timer;

	StatusBenefit benefit;
	StatusType type;
public:
	Status();
	
	bool hasExpired() const {
		return (timed && timer <= 0);
	}

	bool hasStacks() const {
		return (stackable && stacks > 0);
	}

	virtual void processTurn(Unit* target, int roundLength);

	virtual void updateAtCall(Unit* target) = 0;

	~Status() {}
};

class StatusDamagePrevention : public Status
{
protected:
	static const StatusType TYPE = STATUS_PREVENTION;

	int preventionAmount;
	vector<DamageType> types;
public:
	StatusDamagePrevention();

	virtual void updateAtCall(Unit* target);

	~StatusDamagePrevention() {}
};

class StatusDamageOverTime : public Status
{
protected:
	static const StatusType TYPE = STATUS_DAMAGE_OVER_TIME;

	int damageAmount;
	DamageType damageType;
public:
	StatusDamageOverTime();
	
	virtual void processTurn(Unit* target, int roundLength);
	virtual void updateAtCall(Unit* target);

	~StatusDamageOverTime() {}
};
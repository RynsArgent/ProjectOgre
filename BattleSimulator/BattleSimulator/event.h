#ifndef __EVENT_H__
#define __EVENT_H__

#include "pch.h"

#include <fstream>
#include <string>

// Tracks a result of an Action.
struct Event
{
	static const int AUTO_HIT_CHANCE = 100;
	static const int MELEE_HIT_CHANCE = 100;
	static const int RANGE_HIT_CHANCE = 100;
	static const int HEAL_HIT_CHANCE = 100;
	static const int BUFF_HIT_CHANCE = 100;
	static const int DEBUFF_HIT_CHANCE = 50;
	static const int UNBUFF_HIT_CHANCE = 100;
	static const int CLEANSE_HIT_CHANCE = 100;

    Action* ref;
    	
	int chance;
	bool success;

	string desc;

	Event(Action* ref = NULL, int chance = 100);

    // Apply Event abilities if the odds were a success on a target Unit
	void determineSuccess();
	void determineSuccess(Unit* target);

	virtual void apply();
    virtual void print(ostream& out) const;
    
	~Event();
};

struct EventCauseDamage : public Event
{
	Damage* damage;

	EventCauseDamage(Action* ref = NULL, int chance = 100, Damage* damage = NULL)
		: Event(ref, chance), damage(damage)
	{}
	
	virtual void apply();
    virtual void print(ostream& out) const;

	~EventCauseDamage();
};

struct EventCauseStatus : public Event
{
	Status* status;

	EventCauseStatus(Action* ref = NULL, int chance = 100, Status* status = NULL)
		: Event(ref, chance), status(status)
	{}
	
	virtual void apply();
    virtual void print(ostream& out) const;

	~EventCauseStatus();
};

struct EventRemoveStatus : public Event
{
	Unit* target;
	StatusBenefit removingType;

	Status* removedResult;
	
	EventRemoveStatus(Action* ref = NULL, int chance = 100, Unit* target = NULL, StatusBenefit removingType = DEBUFF)
		: Event(ref, chance), target(target), removingType(removingType), removedResult(NULL)
	{}

	virtual void apply();
    virtual void print(ostream& out) const;
	
	~EventRemoveStatus();
};

#endif

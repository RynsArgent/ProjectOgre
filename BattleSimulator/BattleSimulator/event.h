#ifndef __EVENT_H__
#define __EVENT_H__

#include "pch.h"

// Tracks a result of an Action.
struct Event
{
    Action* ref;
    	
	int chance;
	bool success;

	Event(Action* ref = NULL, int chance = 100);

    // Apply Event abilities if the odds were a success on a target Unit
	void determineSuccess(Unit* target);

	virtual void apply();
    virtual void print() const;
    
	~Event();
};

struct EventCauseDamage : public Event
{
	Damage* damage;

	EventCauseDamage(Action* ref = NULL, int chance = 100, Damage* damage = NULL)
		: Event(ref, chance), damage(damage)
	{}
	
	virtual void apply();
    virtual void print() const;
	
	~EventCauseDamage();
};

struct EventCauseStatus : public Event
{
	Status* status;

	EventCauseStatus(Action* ref = NULL, int chance = 100, Status* status = NULL)
		: Event(ref, chance), status(status)
	{}
	
	virtual void apply();
    virtual void print() const;

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
    virtual void print() const;
	
	~EventRemoveStatus();
};

#endif
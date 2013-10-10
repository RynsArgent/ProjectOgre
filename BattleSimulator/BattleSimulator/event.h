#ifndef __EVENT_H__
#define __EVENT_H__

#include "pch.h"

#include "gridpoint.h"

#include <fstream>
#include <string>

// Tracks a result of an Action.
// NOTE: We may need to add more Events such as Status Triggers
struct Event
{
	static const int AUTO_HIT_CHANCE = 100;
	static const int MELEE_HIT_CHANCE = 100;
	static const int RANGE_HIT_CHANCE = 100;
	static const int MAGIC_HIT_CHANCE = 100;
	static const int HEAL_HIT_CHANCE = 100;
	static const int BUFF_HIT_CHANCE = 100;
	static const int DEBUFF_HIT_CHANCE = 100;
	static const int UNBUFF_HIT_CHANCE = 100;
	static const int CLEANSE_HIT_CHANCE = 100;
	static const int KNOCKBACK_HIT_CHANCE = 100;

    Action* ref;
    string name;

	int chance;
	bool success;

	bool hiddenSource;

	string desc;

	Event(Action* ref, const string & name, int chance = 100, bool hiddenSource = false);

    // Apply Event abilities if the odds were a success on a target Unit
	void determineSuccess();
	void determineSuccess(Unit* target);

	virtual void apply(Battle* battle);
    virtual void print(ostream& out) const;
    
	~Event();
};

struct EventCauseDamage : public Event
{
	Damage* damage;

	EventCauseDamage(Action* ref, const string & name, int chance = 100, Damage* damage = NULL, bool hiddenSource = false)
		: Event(ref, name, chance, hiddenSource), damage(damage)
	{}
	
	virtual void apply(Battle* battle);
    virtual void print(ostream& out) const;

	~EventCauseDamage();
};

struct EventCauseStatus : public Event
{
	Status* status;

	EventCauseStatus(Action* ref, const string & name, int chance = 100, Status* status = NULL, bool hiddenSource = false)
		: Event(ref, name, chance, hiddenSource), status(status)
	{}
	
	virtual void apply(Battle* battle);
    virtual void print(ostream& out) const;

	~EventCauseStatus();
};

struct EventRemoveStatus : public Event
{
	Unit* target;
	StatusBenefit removingType;

	StatusGroup* removedResult;
	
	EventRemoveStatus(Action* ref, const string & name, int chance = 100, Unit* target = NULL, StatusBenefit removingType = DEBUFF, bool hiddenSource = false)
		: Event(ref, name, chance, hiddenSource), target(target), removingType(removingType), removedResult(NULL)
	{}

	virtual void apply(Battle* battle);
    virtual void print(ostream& out) const;
	
	~EventRemoveStatus();
};

struct EventReposition : public Event
{
	Unit* target;
	GridPoint destination;

	EventReposition(Action* ref, const string & name, int chance = 100, Unit* target = NULL, const GridPoint & destination = GridPoint(), bool hiddenSource = false)
		: Event(ref, name, chance, hiddenSource), target(target), destination(destination)
	{}
	
	virtual void apply(Battle* battle);
    virtual void print(ostream& out) const;
	
	~EventReposition();
};

#endif

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
	static const int AUTO_HIT_CHANCE = 101; // 101 used so that even blind effects will not reduce it
	static const int MELEE_HIT_CHANCE = 100;
	static const int RANGE_HIT_CHANCE = 100;
	static const int MAGIC_HIT_CHANCE = 100;
	static const int HEAL_HIT_CHANCE = 100;
	static const int BUFF_HIT_CHANCE = 100;
	static const int DEBUFF_HIT_CHANCE = 100;
	static const int UNBUFF_HIT_CHANCE = 100;
	static const int CLEANSE_HIT_CHANCE = 100;
	static const int DISPEL_HIT_CHANCE = 100;
	static const int REPOSITION_HIT_CHANCE = 100;
	static const int RAISE_HIT_CHANCE = 100;

    Action* ref;
    string name;
	EventType type;

	int chance;
	bool success;

	bool hiddenSource;

	string desc;

	Event(Action* ref, const string & name, EventType type = EVENT_NONE, int chance = 100, bool hiddenSource = false);

    // Apply Event abilities if the odds were a success on a target Unit
	bool isAutomaticSuccess() const;
	bool isAutomaticFailure() const;
	void determineSuccess();

	virtual void apply(Battle* battle);
    virtual void print(ostream& out) const;
    
	virtual ~Event();
};

struct EventAttack : public Event
{
	static const EventType TYPE = EVENT_ATTACK;

	AbilityType attack; // This is separate from Ability Class for special cases (look at Feint)
	Unit* target;
	bool indirect; // This means the attacker or source is not directly focused on this target (splash damage for example)
	
	EventAttack(Action* ref, const string & name, int chance = 100, AbilityType attack = ABILITY_NONE, Unit* target = NULL, bool indirect = false, bool hiddenSource = false)
		: Event(ref, name, TYPE, chance, hiddenSource), attack(attack), indirect(indirect), target(target)
	{}
	
	void determineSuccess(Unit* target);

	virtual void apply(Battle* battle);
    virtual void print(ostream& out) const;

	~EventAttack();
};

struct EventCauseDamage : public Event
{
	static const EventType TYPE = EVENT_CAUSE_DAMAGE;

	Damage* damage;

	EventCauseDamage(Action* ref, const string & name, int chance = 100, Damage* damage = NULL, bool hiddenSource = false)
		: Event(ref, name, TYPE, chance, hiddenSource), damage(damage)
	{}
	
	virtual void apply(Battle* battle);
    virtual void print(ostream& out) const;

	~EventCauseDamage();
};

struct EventCauseStatus : public Event
{
	static const EventType TYPE = EVENT_CAUSE_STATUS;

	Status* status;

	EventCauseStatus(Action* ref, const string & name, int chance = 100, Status* status = NULL, bool hiddenSource = false)
		: Event(ref, name, TYPE, chance, hiddenSource), status(status)
	{}
	
	virtual void apply(Battle* battle);
    virtual void print(ostream& out) const;

	~EventCauseStatus();
};

struct EventRemoveStatus : public Event
{
	static const EventType TYPE = EVENT_REMOVE_STATUS;

	Unit* target;
	StatusBenefit removingType;

	StatusGroup* removedResult;
	
	// Doesn't have something to remove, but a category BUFF DEBUFF or NEUTRAL
	EventRemoveStatus(Action* ref, const string & name, int chance = 100, Unit* target = NULL, StatusBenefit removingType = DEBUFF, bool hiddenSource = false)
		: Event(ref, name, TYPE, chance, hiddenSource), target(target), removingType(removingType), removedResult(NULL)
	{}
	
	// Know what to remove
	EventRemoveStatus(Action* ref, const string & name, int chance = 100, Unit* target = NULL, StatusGroup* status = NULL, bool hiddenSource = false)
		: Event(ref, name, TYPE, chance, hiddenSource), target(target), removingType(NEUTRAL), removedResult(status)
	{}


	virtual void apply(Battle* battle);
    virtual void print(ostream& out) const;
	
	~EventRemoveStatus();
};

struct EventReposition : public Event
{
	static const EventType TYPE = EVENT_REPOSITION;

	Unit* target;
	GridPoint destination;

	EventReposition(Action* ref, const string & name, int chance = 100, Unit* target = NULL, const GridPoint & destination = GridPoint(), bool hiddenSource = false)
		: Event(ref, name, TYPE, chance, hiddenSource), target(target), destination(destination)
	{}
	
	virtual void apply(Battle* battle);
    virtual void print(ostream& out) const;
	
	~EventReposition();
};

struct EventRaise : public Event
{
	static const EventType TYPE = EVENT_RAISE;

	Unit* target;
	GridPoint destination;

	EventRaise(Action* ref, const string & name, int chance = 100, Unit* target = NULL, const GridPoint & destination = GridPoint(), bool hiddenSource = false)
		: Event(ref, name, TYPE, chance, hiddenSource), target(target), destination(destination)
	{}
	
	virtual void apply(Battle* battle);
    virtual void print(ostream& out) const;
	
	~EventRaise();
};

#endif

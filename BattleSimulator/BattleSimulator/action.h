#ifndef __ACTION_H__
#define __ACTION_H__

#include "pch.h"

#include <vector>

// Base class for battle events
class Action
{
protected:
	string name;
    
    ActionType action; // Defines how the action is triggered
	AbilityType type; // Defines the category an ability belongs to
    
    Unit* source;
    Battle* battle;
    
	vector<Event*> events;
	vector<Targeter*> targeters;
public:
    Action()
		: name(""), action(ACTION_NONE), type(ABILITY_NONE), source(NULL), battle(NULL), events(), targeters()
	{}
    Action(const string & name, ActionType action, AbilityType type, Unit* source = NULL, Battle* battle = NULL)
        : name(name), action(action), type(type), source(source), battle(battle), events(), targeters()
	{}
    
    string getName() const { return name; }
	ActionType getAction() const { return action; }
	AbilityType getAbilityType() const { return type; }
	Unit* getSource() const { return source; }
	Battle* getBattle() const { return battle; }
	vector<Event*> getEvents() { return events; }
	vector<Targeter*> getTargeters() { return targeters; }
    
    void addEvent(Event* evt);
    void addTargeter(Targeter* targeter);

    virtual void print(ostream& out) const = 0;
    
    virtual ~Action();
};

#endif

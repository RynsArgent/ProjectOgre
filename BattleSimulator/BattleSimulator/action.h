#ifndef __ACTION_H__
#define __ACTION_H__

#include "pch.h"

#include <vector>

// Base class for battle events
class Action
{
protected:
	string name;
    
    ActionType action;
    
    Unit* source;
    Battle* battle;
    
	vector<Event*> events;
public:
    Action()
		: name(""), action(ACTION_NONE), source(NULL), battle(NULL), events()
	{}
    Action(const string & name, ActionType action, Unit* source = NULL, Battle* battle = NULL)
        : name(name), action(action), source(source), battle(battle), events()
	{}
    
    string getName() const { return name; }
	ActionType getAction() const { return action; }
	Unit* getSource() const { return source; }
	Battle* getBattle() const { return battle; }
	vector<Event*> getEvents() { return events; }
    
    void addEvent(Event* event);

    virtual void print() const = 0;
    
    ~Action();
};

#endif

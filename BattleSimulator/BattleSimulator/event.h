#ifndef __EVENT_H__
#define __EVENT_H__

#include "pch.h"

// Tracks a result of an Action.
struct Event
{
    Action* ref;
    
	Damage* damage;
	Status* status;
	
	int chance;

	bool success;

	Event(Action* ref = NULL, Damage* damage = NULL, Status* status = NULL, int chance = 100);

    // Apply Event abilities if the odds were a success
	void apply();

    void print() const;
    
	~Event();
};

#endif
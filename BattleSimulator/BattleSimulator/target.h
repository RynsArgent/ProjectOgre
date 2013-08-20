#ifndef __TARGET_H__
#define __TARGET_H__

#include "pch.h"

#include <algorithm>
#include <vector>
#include <cstdlib>

#include "unit.h"

// Simple object that contains potential targets
struct Targeter
{	
	Action* ref;

	vector<Unit*> base;
	vector<Unit*> candidates;
	vector<int> priorities; // This is main motivation for Taunt effects, they are indices referencing candidates
	vector<Unit*> chosen;
    
    TargetGroup group;
	TargetType method; // Method of selecting candidates
	bool primary; // Determines whether a primary target exists for this system for counterattacks
	
	// Dynamic variables
	bool provoked; // If set to true, that means the Action has processed long enough (before completion or cancellation)
					// for the primary unit to respond

	Targeter(Action* ref, const vector<Unit*> & candidates, TargetGroup group, TargetType method, bool primary = true)
		: ref(ref), base(candidates), candidates(candidates), priorities(), chosen(), group(group), method(method), primary(primary), provoked(false)
	{
    }
            
	Unit* getPrimary() const {
		if (!primary) return NULL;
		if (chosen.size() > 0)
			return chosen[0];
		return NULL;
	}

	// The static functions below are common in abilities and are provided for code reuse. (Search for front targets, back targets, ect)
	static vector<Unit*> searchForFrontTargets(Unit* current, Battle* battle, Group* allyGroup, Group* enemyGroup, int startingAdjacencyRange, int rowRange);
	
	void set(int n = 1);
    
    void print() const;

	~Targeter() {}
};

#endif
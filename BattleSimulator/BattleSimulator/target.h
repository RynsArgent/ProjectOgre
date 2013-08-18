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
    Unit* source;
    Battle* battle;

	vector<Unit*> base;
	vector<Unit*> candidates;
	vector<int> priorities; // This is main motivation for Taunt effects, they are indices referencing candidates
	vector<Unit*> chosen;
    
    TargetGroup group;
	TargetType method; // Method of selecting candidates
	bool primary; // Determines whether a primary target exists for this system

	Targeter(Unit* source, Battle* battle, const vector<Unit*> & candidates, TargetGroup group, TargetType method, bool primary = true)
		: source(source), battle(battle), base(candidates), candidates(candidates), priorities(), chosen(), group(group), method(method), primary(primary)
	{
    }
            
	Unit* getSource() const { return source; }
	Battle* getBattle() const { return battle; }

	// The static functions below are common in abilities and are provided for code reuse. (Search for front targets, back targets, ect)
	static vector<Unit*> searchForFrontTargets(Unit* current, Battle* battle, Group* allyGroup, Group* enemyGroup, int startingAdjacencyRange, int rowRange);
	
	void set(int n = 1);
    
    void print() const;

	~Targeter() {}
};

#endif
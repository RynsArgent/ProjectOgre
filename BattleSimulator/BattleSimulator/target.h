#ifndef __TARGET_H__
#define __TARGET_H__

#include "pch.h"

#include <algorithm>
#include <vector>
#include <cstdlib>

// Simple object that contains potential targets
struct Targeter
{
	Unit* caster;
	
	vector<Unit*> candidates;
	vector<int> priorities; // This is main motivation for Taunt effects, they are indices referencing candidates
	vector<Unit*> chosen;

	TargetType method; // Method of selecting candidates
	TargetBenefit benefit; // Determines whether this targeting system is good or bad
	bool primary; // Determines whether a primary target exists for this system

	Targeter(Unit* caster, const vector<Unit*> & candidates, TargetType method, TargetBenefit benefit = TARGET_UNKNOWN, bool primary = true)
		: caster(caster), candidates(candidates), priorities(), chosen(), method(method), benefit(benefit), primary(primary)
	{}
	
	// The static functions below are common in abilities and are provided for code reuse. (Search for front targets, back targets, ect)
	static vector<Unit*> searchForFrontTargets(Unit* current, Unit* previous, Battle* battle, Group* allyGroup, Group* enemyGroup, int startingAdjacencyRange, int rowRange);
	
	void set(int n = 1);

	~Targeter() {}
};

#endif
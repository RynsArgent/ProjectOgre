#ifndef __DAMAGE_H__
#define __DAMAGE_H__

#include "pch.h"

#include "unit.h"
#include "ability.h"
#include "status.h"

struct Damage
{
	// Reference to the ability or status effect that causes this damage. Useful for damage prevention effects
	// that or more specific than the standard damage reductions (i.e. Only prevent ranged damage)
	Ability* ability;
	Status* status;

	int amount;
	DamageType type;

	Damage(Ability* aref, int amount, DamageType type = DAMAGE_NONE) : ability(aref), status(NULL), amount(amount), type(type) {}
	Damage(Status* sref, int amount, DamageType type = DAMAGE_NONE) : ability(NULL), status(sref), amount(amount), type(type) {}

	int apply(Unit* target);
};

int findNumMatching(const vector<DamageType> & types1, const vector<DamageType> & types2);

#endif
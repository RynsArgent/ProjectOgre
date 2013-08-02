#ifndef __DAMAGE_H__
#define __DAMAGE_H__

#include "pch.h"

#include "unit.h"
#include "ability.h"
#include "status.h"

struct DamageNode
{
	int amount;
	DamageType type;

	DamageNode* next;
	
	DamageNode(int amount, DamageType type = DAMAGE_NONE, DamageNode* next = NULL)
		: amount(amount), type(type), next(next)
	{}

	int apply(Unit* target);

	void clean() {
		if (next) { 
			next->clean();
			delete next;
		}
		next = NULL;
	}

	~DamageNode() {}
};

struct Damage
{
	// Reference to the ability or status effect that causes this damage. Useful for damage prevention effects
	// that or more specific than the standard damage reductions (i.e. Only prevent ranged damage)
	Ability* ability;
	Status* status;

	DamageNode* head;
	DamageNode* tail;

	int final; // Final calculated damage

	Damage(Ability* aref, int amount, DamageType type = DAMAGE_NONE) 
		: ability(aref), status(NULL), head(new DamageNode(amount, type)), tail(head), final(0)
	{
	}

	Damage(Status* sref, int amount, DamageType type = DAMAGE_NONE)
		: ability(NULL), status(sref), head(new DamageNode(amount, type)), tail(head), final(0)
	{
	}

	void add(int amount, DamageType type = DAMAGE_NONE) {
		tail->next = new DamageNode(amount, type);
		tail = tail->next;
	}

	int apply(Unit* target);

	void clean() {
		head->clean();
		delete head; head = NULL;
		tail = NULL;
	}

	~Damage() {
		clean();
	}
};

int findNumMatching(const vector<DamageType> & types1, const vector<DamageType> & types2);

#endif
#ifndef __DAMAGE_H__
#define __DAMAGE_H__

#include "pch.h"

#include "unit.h"
#include "ability.h"
#include "status.h"

struct DamageNode
{
	int start;
	int amount;
	int final;
	DamageType type;

	DamageNode* next;
	
	DamageNode(int amount, DamageType type = DAMAGE_TYPELESS, DamageNode* next = NULL)
		: start(amount), amount(amount), final(0), type(type), next(next)
	{}

	void modify(Unit* applier, Unit* target);
	int apply(Unit* applier, Unit* target);

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

	Unit* applier;
	Unit* target;

	DamageNode* head;
	DamageNode* tail;
	
	int start; // Final calculated damage
	int final; // Final calculated damage

	Damage(Ability* aref, Unit* applier, Unit* target, int amount, DamageType type = DAMAGE_TYPELESS) 
		: ability(aref), status(NULL), applier(applier), target(target), head(new DamageNode(amount, type)), tail(head), start(amount), final(0)
	{
	}

	Damage(Status* sref, Unit* applier, Unit* target, int amount, DamageType type = DAMAGE_TYPELESS)
		: ability(NULL), status(sref), applier(applier), target(target), head(new DamageNode(amount, type)), tail(head), start(amount), final(0)
	{
	}

	void add(int amount, DamageType type = DAMAGE_TYPELESS) {
		tail->next = new DamageNode(amount, type);
		tail = tail->next;
		start += amount;
	}

	void apply();

	void clean() {
		head->clean();
		delete head; head = NULL;
		tail = NULL;
	}

	~Damage() {
		clean();
	}

	static int findNumMatching(const vector<DamageType> & types1, const vector<DamageType> & types2);
	static int getDamageValue(DamageRating rating, int base);
};


#endif
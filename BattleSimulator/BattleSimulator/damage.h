#ifndef __DAMAGE_H__
#define __DAMAGE_H__

#include "pch.h"

#include <vector>
#include "action.h"

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

	void modify(Unit* target);
	int apply(Unit* target);

	void clean() {
		if (next) { 
			next->clean();
			delete next;
		}
		next = NULL;
	}

    void print() const;
    
	~DamageNode() {}
};

struct Damage : public Action
{
	// Reference to the ability or status effect that causes this damage. Useful for damage prevention effects
	// that or more specific than the standard damage reductions (i.e. Only prevent ranged damage)
	Ability* ability;
	Status* status;

    Unit* target;
    
	DamageNode* head;
	DamageNode* tail;
	
	int start; // Final calculated damage
	int final; // Final calculated damage

	Damage(Ability* aref, Unit* target, int amount, DamageType type = DAMAGE_TYPELESS) 
		: Action() ,ability(aref), status(NULL), target(target), head(new DamageNode(amount, type)), tail(head), start(amount), final(0)
	{
        init();
    }

	Damage(Status* sref, Unit* target, int amount, DamageType type = DAMAGE_TYPELESS)
		: Action(), ability(NULL), status(sref), target(target), head(new DamageNode(amount, type)), tail(head), start(amount), final(0)
	{
        init();
	}

    void init();
    
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

    virtual void print() const;
    
	~Damage() {
		clean();
	}

	static int findNumMatching(const vector<DamageType> & types1, const vector<DamageType> & types2);
	static int getDamageValue(DamageRating rating, int base);
};


#endif
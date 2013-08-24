#ifndef __DAMAGE_H__
#define __DAMAGE_H__

#include "pch.h"

#include <fstream>
#include <vector>

struct DamageNode
{
	int start;
	int amount;
	int final;
	DamageRating rating;
	DamageType type;

	DamageNode* next;
	
	DamageNode(int amount, DamageRating rating, DamageType type = DAMAGE_TYPELESS, DamageNode* next = NULL);

	void modify(Unit* target);
	int apply(Unit* target);

	void clean() {
		if (next) { 
			next->clean();
			delete next;
		}
		next = NULL;
	}

    void print(ostream& out) const;
    
	~DamageNode() {}
};

struct Damage
{
	// Reference to the ability or effect that causes this damage. Useful for damage prevention effects
	// that or more specific than the standard damage reductions (i.e. Only prevent ranged damage)
	Action* action;

    Unit* target;
    
	DamageNode* head;
	DamageNode* tail;
	int size;
	
	int start; // Final calculated damage
	int final; // Final calculated damage

	Damage(Action* aref, Unit* target, int amount, DamageRating rating, DamageType type = DAMAGE_TYPELESS) 
		: action(aref), target(target), head(new DamageNode(amount, rating, type)), tail(head), size(1), start(amount), final(0)
	{
    }
    
	void add(int amount, DamageRating rating, DamageType type = DAMAGE_TYPELESS) {
		tail->next = new DamageNode(amount, rating, type);
		tail = tail->next;
		start += amount;
		++size;
	}

	void apply();

	void clean() {
		head->clean();
		delete head; head = NULL;
		tail = NULL;
	}

    void print(ostream& out) const;
    
	~Damage() {
		clean();
	}

	static int findNumMatching(const vector<DamageType> & types1, const vector<DamageType> & types2);
	static int getDamageValue(DamageRating rating, int base);
};


#endif
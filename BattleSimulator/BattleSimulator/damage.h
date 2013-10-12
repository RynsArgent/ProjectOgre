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
	bool pierce; // ignore damage reduction

	DamageNode* next;
	
	DamageNode(int amount, DamageRating rating, DamageType type = DAMAGE_TYPELESS, bool pierce = false, DamageNode* next = NULL);

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
	
	int start; // initial damage
	int final; // Final calculated damage

	Damage(Action* aref, Unit* target, int amount, DamageRating rating, DamageType type = DAMAGE_TYPELESS, bool pierce = false) 
		: action(aref), target(target), head(new DamageNode(amount, rating, type, pierce)), tail(head), size(1), start(amount), final(0)
	{
    }
    
	void add(int amount, DamageRating rating, DamageType type = DAMAGE_TYPELESS, bool pierce = false) {
		tail->next = new DamageNode(amount, rating, type, pierce);
		tail = tail->next;
		start += amount;
		++size;
	}

	void apply(Battle* battle);

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
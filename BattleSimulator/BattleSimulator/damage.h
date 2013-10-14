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
		: action(aref), target(target), head(NULL), tail(NULL), size(0), start(0), final(0)
	{
		add(amount, rating, type, pierce);
    }
    
	void add(int amount, DamageRating rating, DamageType type = DAMAGE_TYPELESS, bool pierce = false) {
		DamageNode* n = NULL;
		if (type == DAMAGE_HOLY) {
			const int NUM_DAMAGE_TYPES = 3;
			int dividedAmount = amount / 3;
			int dividedRemainder = amount % NUM_DAMAGE_TYPES;
			n = new DamageNode(dividedAmount + (dividedRemainder == 1), rating, DAMAGE_WATER, pierce, // Water
				new DamageNode(dividedAmount + (dividedRemainder == 2), rating, DAMAGE_LIGHTNING, pierce, // Lighting
				new DamageNode(dividedAmount, rating, DAMAGE_EARTH, pierce, NULL))); // Earth
				size += 3;
		} else if (type == DAMAGE_SHADOW) {
			const int NUM_DAMAGE_TYPES = 3;
			int dividedAmount = amount / 3;
			int dividedRemainder = amount % NUM_DAMAGE_TYPES;
			n = new DamageNode(dividedAmount + (dividedRemainder == 1), rating, DAMAGE_FIRE, pierce, // Fire
				new DamageNode(dividedAmount + (dividedRemainder == 2), rating, DAMAGE_ICE, pierce, // Ice
				new DamageNode(dividedAmount, rating, DAMAGE_EARTH, pierce, NULL))); // Earth
				size += 3;
		} else {
			n = new DamageNode(amount, rating, type, pierce);
			++size;
		}

		if (head == NULL) {
			head = n;
			tail = n;
		} else {
			tail->next = n;
			tail = tail->next;
		}
		start += amount;
	}

	void apply(Battle* battle);

	void clean() {
		head->clean();
		delete head; head = NULL;
		tail = NULL;
	}

	bool containsDamageType(DamageType type) const {
		for (DamageNode* n = head; n != NULL; n = n->next)
			if (n->type == type)
				return true;
		return false;
	}

    void print(ostream& out) const;
    
	~Damage() {
		clean();
	}

	static int findNumMatching(const vector<DamageType> & types1, const vector<DamageType> & types2);
	static int getDamageValue(DamageRating rating, int base);
};


#endif
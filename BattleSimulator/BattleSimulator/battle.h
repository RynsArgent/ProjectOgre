#ifndef __BATTLE_H__
#define __BATTLE_H__

#include "pch.h"

#include <vector>

class Battle
{
private:
	Group* group1;
	Group* group2;
	
	int roundNumber;
	int turnIndex;
	vector<Unit*> unitOrder;

	Unit* mainUnit;
	Unit* respondUnit;
	Ability* mainAbility;
	Ability* respondAbility;
	vector<Event*> eventStack;

	bool isOver;
public:
	Battle(Group *group1, Group *group2);

	int roundLength() const {
		return unitOrder.size();
	}

	Group* getAllyGroup(int gid) const;
	Group* getEnemyGroup(int gid) const;

	bool isBattleOver() const;
	void initializeUnits();
	void newUnitOrder();
	void executeTurn();
	void cleanupTurn();
    void addToEventStack(Event* event);
    
	void simulate();
	
	void print() const;

	~Battle() {}

	friend class Ability;
	friend class Renderer;
};

#endif

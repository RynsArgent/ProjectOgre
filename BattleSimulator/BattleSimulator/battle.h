#ifndef __BATTLE_H__
#define __BATTLE_H__

#include "pch.h"

#include "gridpoint.h"
#include "group.h"

class Battle
{
private:
	Group* group1;
	Group* group2;
	
	int roundNumber;
	int turnIndex;
	vector<Unit*> unitOrder;
	vector<Action*> actionStack;

	bool isOver;
public:
	Battle(Group *group1, Group *group2);

	int roundLength() const {
		return unitOrder.size();
	}

	Group* getAllyGroup(int gid) const {
		if (gid == group1->getGrid())
			return group1;
		else
			return group2;
	}

	Group* getEnemyGroup(int gid) const {
		if (gid != group1->getGrid())
			return group1;
		else
			return group2;
	}

	bool isBattleOver() const;
	void initializeUnits();
	void newUnitOrder();
	void executeTurn();
    void addToActionStack(Action* action);
    
	void simulate();
	
	void print() const;

	~Battle() {}

	friend class Ability;
};

#endif

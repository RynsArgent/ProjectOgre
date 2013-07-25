#pragma once

#include "pch.h"

#include "group.h"

class Battle
{
private:
	Group* group1;
	Group* group2;
	
	int roundNumber;
	int turnIndex;
	vector<Unit*> unitOrder;

	bool isOver;
public:
	Battle(Group *group1, Group *group2);

	int roundLength() const {
		return unitOrder.size();
	}

	Group* getAllyGroup(int gid) const {
		if (gid == group1->getGid())
			return group1;
		else
			return group2;
	}

	Group* getEnemyGroup(int gid) const {
		if (gid != group1->getGid())
			return group1;
		else
			return group2;
	}

	bool isBattleOver() const;
	void initializeUnits();
	void newUnitOrder();
	void executeTurn();

	void simulate();
	
	void print() const;


	~Battle() {}

	friend class Ability;
};

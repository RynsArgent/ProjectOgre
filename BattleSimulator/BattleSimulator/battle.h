#pragma once

#include "group.h"
#include "status.h"

#include <map>

class Battle
{
private:
	Group* group1;
	Group* group2;
	
	vector<Unit*> unitOrder;
	map<Unit*, vector<Status*> > statusEffects;
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

	void initializeUnits();
	void newUnitOrder();
	void executeTurn();

	~Battle() {}

	friend class Ability;
};

#ifndef __BATTLE_H__
#define __BATTLE_H__

#include "pch.h"

#include <vector>

class Battle
{
private:
	int seed;
	Group* group1;
	Group* group2;
	
	int roundNumber;
	int turnIndex;
	vector<Unit*> unitOrder;

	Unit* globaltrigger;
	Unit* mainUnit;
	Unit* respondUnit;
	Ability* mainAbility;
	Ability* respondAbility;

	vector<Event*> eventStack;
	vector<StatusGroup*> cleanup;

	bool isOver;
public:
	Battle(int seed, Group *group1, Group *group2);

	int roundLength() const {
		return unitOrder.size();
	}
	
	int getSeed() const { return seed; }
	int getRoundNumber() const { return roundNumber; }
	Unit* getGlobalTrigger() const { return globaltrigger; }
	Unit* getMainUnit() const { return mainUnit; }
	Unit* getRespondUnit() const { return respondUnit; }
	vector<Event*> getEventStack() const { return eventStack; }

	Group* getAllyGroup(int gid) const;
	Group* getEnemyGroup(int gid) const;

	bool isBattleOver() const;
	void initializeUnits();
	void newUnitOrder();
	void executeTurn();
	void cleanupTurn();
    void addToEventStack(Event* value);
    void addToCleanup(StatusGroup* value);
	int getWinner() const;
    
	void simulate();
	
	void preprint() const;
	void postprint() const;

	~Battle() {}

	friend class Ability;
	friend class Renderer;
};

#endif

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

    // This can be attached with Status and Effects without an attachment
    // to a real existing unit. This allows for timers to run turn by turn
    // and not by round by round.
	Unit* globalTrigger;
    
	Unit* mainUnit; // Current unit's turn
	Unit* respondUnit; // The unit responding to current unit
	Ability* mainAbility;
	Ability* respondAbility;
	
	vector<Event*> eventStack;
	vector<Ability*> abilStack; // For cleanup
	vector<StatusGroup*> cleanup;

	bool isOver;
public:
	Battle(int seed, Group *group1, Group *group2);

	int roundLength() const {
		return unitOrder.size();
	}
	
	int getSeed() const { return seed; }
	int getRoundNumber() const { return roundNumber; }
	Unit* getGlobalTrigger() const { return globalTrigger; }
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
    void addToAbilStack(Ability* value);
    void addToCleanup(StatusGroup* value);
	int getWinner() const;
    
	void simulate();
	
	void preprint() const;
	void postprint() const;

	~Battle();

	friend class Ability;
	friend struct Renderer;
};

#endif

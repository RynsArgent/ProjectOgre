#include "battle.h"

#include <algorithm>
#include <cstdlib>
#include "status.h"
#include "ability.h"
#include "target.h"
#include "unit.h"
#include "gridpoint.h"
#include "group.h"

// Used to determine unit order, faster units are sorted to the front of the list
bool compareSpeed(Unit* lhs, Unit* rhs) {
	return bound(lhs->getCurrentSpeed(), VALUE_SPEED) > bound(rhs->getCurrentSpeed(), VALUE_SPEED) || 
		(bound(lhs->getCurrentSpeed(), VALUE_SPEED) == bound(rhs->getCurrentSpeed(), VALUE_SPEED) &&
		 lhs->getCurrentInitiative() > rhs->getCurrentInitiative());
}

Battle::Battle(Group* group1, Group* group2)
	: group1(group1), group2(group2), roundNumber(0), turnIndex(-1), unitOrder(), 
	mainUnit(NULL), respondUnit(NULL), mainAbility(NULL), respondAbility(NULL),
	eventStack(), isOver(false)
{
	group1->turnToFace(FACING_FORWARD);
	group2->turnToFace(FACING_FORWARD);

	// Set up unit order
	initializeUnits();
}

bool Battle::isBattleOver() const
{
	return isOver;
}

Group* Battle::getAllyGroup(int gid) const {
	if (gid == group1->getGrid())
		return group1;
	else
		return group2;
}

Group* Battle::getEnemyGroup(int gid) const {
	if (gid != group1->getGrid())
		return group1;
	else
		return group2;
}


void Battle::initializeUnits()
{
	vector<Unit*> units1 = group1->allyUnits();
	vector<Unit*> units2 = group2->allyUnits();
	for (int i = 0; i < units1.size(); ++i)
		unitOrder.push_back(units1[i]);		
	for (int i = 0; i < units2.size(); ++i)
		unitOrder.push_back(units2[i]);
	
	newUnitOrder();
	sort(unitOrder.begin(), unitOrder.end(), compareSpeed);
}

void Battle::newUnitOrder()
{
	// Set a random initiative so that units with same speed are dealt with
	for (int i = 0; i < unitOrder.size(); ++i)
		unitOrder[i]->setCurrentInitiative(rand());
}

void Battle::executeTurn()
{
	if (isBattleOver())
		return;
	// Determine whether a new round started
	if (turnIndex < 0 || turnIndex >= unitOrder.size())
	{
		turnIndex = 0;
		++roundNumber;
	}

	// Retrieve the next unit in the turn list
	mainUnit = unitOrder[turnIndex];
	
	// Process unit ongoing effects
	mainUnit->processEffects();

    mainAbility = NULL;
	// Perform the unit ability based on its position
	if (mainUnit && mainUnit->isAvailable())
	{
		mainUnit->setCurrentSkill(NO_STANDARD_SKILL);
		mainUnit->setCurrentTier(2);

		// Activate any status effects that occur on preparing for abilities
		for (int i = 0; i < mainUnit->getCurrentStatus().size(); ++i) {
			mainUnit->getCurrentStatus()[i]->onSelectAbility(mainUnit);
		}

		mainUnit->setCurrentSkill(Ability::selectSkill(mainUnit));

		// Execute the ability
		mainAbility = Ability::getAbility(mainUnit->getCurrentSkill());
		mainAbility->action(NULL, mainUnit, this);
	}

	// Response ability
	respondAbility = NULL;
	if (mainAbility != NULL)
	{
		Targeter* mainTargeter = mainAbility->retrieveFirstPrimaryTargeter();
		if (mainTargeter != NULL)
		{
			respondUnit = mainTargeter->getPrimary();
			if (respondUnit != NULL && mainTargeter->provoked)
			{
				respondUnit->setCurrentSkill(NO_STANDARD_SKILL);
				respondUnit->setCurrentTier(1);

				// Activate any status effects that occur on preparing for abilities
				for (int i = 0; i < respondUnit->getCurrentStatus().size(); ++i) {
					respondUnit->getCurrentStatus()[i]->onSelectAbility(respondUnit);
				}
				
				respondUnit->setCurrentSkill(Ability::selectSkill(respondUnit));

				respondAbility = Ability::getAbility(respondUnit->getCurrentSkill());
				if (Ability::isAbleToRespond(mainAbility, respondAbility))
					respondAbility->action(mainAbility, respondUnit, this);
			}
		}
	}

	// Clean up any units that have died
	group1->cleanDead();
	group2->cleanDead();
	// Determine whether an end result has occurred
	if (!group1->groupIsAvailable() || !group2->groupIsAvailable())
		isOver = true;
	
    print();
    
	// Increment to the next turn
	++turnIndex;
	
	// Will need to sort based on only units that have not moved yet, especially when units can start changing speeds
	sort(unitOrder.begin() + turnIndex, unitOrder.end(), compareSpeed);
}

void Battle::cleanupTurn()
{
    // Clean up turn data
	for (int i = 0; i < eventStack.size(); ++i)
		delete eventStack[i];
	eventStack.clear();
    if (mainAbility) {
		delete mainAbility;
		mainAbility = NULL;
	}
	if (respondAbility) {
		delete respondAbility;
		respondAbility = NULL;
	}
	// Clean up ended effects, this must be after deleted event stacks because the event stack
	// references effect names
	if (mainUnit) mainUnit->cleanEffects();
}

void Battle::addToEventStack(Event* event) {
    eventStack.push_back(event);
}

void Battle::simulate()
{
	while (!isBattleOver())
	{
		executeTurn();
	}
}

void Battle::print() const
{
	cout << "Round Number: " << roundNumber << endl;
    
	cout << "--------------------------------------------------" << endl;

	cout << "Battle Map: " << endl;
	group2->printGroup(true);
	cout << endl;
	group1->printGroup(false);
	
	cout << "--------------------------------------------------" << endl;
	
    for (int i = 0; i < eventStack.size(); ++i)
        eventStack[i]->print(cout);
	
	cout << "--------------------------------------------------" << endl;

	for (int i = 0; i < unitOrder.size(); ++i)
	{
		if (i == turnIndex)
			cout << "*** ";
		unitOrder[i]->print();
	}
	if (isBattleOver())
	{
		if (group1->groupIsAvailable())
			cout << "Group 1 Won!" << endl;
		else if (group2->groupIsAvailable())
			cout << "Group 2 Won!" << endl;
		else
			cout << "Tie!" << endl;
	}
	cout << endl << endl << endl;
}

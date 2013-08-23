#include "battle.h"

#include <algorithm>
#include <cstdlib>
#include "status.h"
#include "ability.h"
#include "target.h"

// Used to determine unit order, faster units are sorted to the front of the list
bool compareSpeed(Unit* lhs, Unit* rhs) {
	return bound(lhs->getCurrentSpeed(), VALUE_SPEED) > bound(rhs->getCurrentSpeed(), VALUE_SPEED) || 
		(bound(lhs->getCurrentSpeed(), VALUE_SPEED) == bound(rhs->getCurrentSpeed(), VALUE_SPEED) &&
		 lhs->getCurrentInitiative() > rhs->getCurrentInitiative());
}

Battle::Battle(Group* group1, Group* group2)
	: group1(group1), group2(group2), roundNumber(0), turnIndex(-1), unitOrder(), eventStack(), isOver(false)
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
	Unit* unit1 = unitOrder[turnIndex];
	
	// Process unit ongoing effects
	unit1->processEffects();

    Ability* mainAbility = NULL;
	// Perform the unit ability based on its position
	if (unit1 && unit1->isAvailable())
	{
		unit1->setCurrentSkill(NO_STANDARD_SKILL);
		unit1->setCurrentTier(2);

		// Activate any status effects that occur on preparing for abilities
		for (int i = 0; i < unit1->getCurrentStatus().size(); ++i) {
			unit1->getCurrentStatus()[i]->onSelectAbility(unit1);
		}

		unit1->setCurrentSkill(Ability::selectSkill(unit1));

		// Execute the ability
		mainAbility = Ability::getAbility(unit1->getCurrentSkill());
		mainAbility->action(NULL, unit1, this);
	}

	// Response ability
	Ability* respondAbility = NULL;
	if (mainAbility != NULL)
	{
		Targeter* mainTargeter = mainAbility->retrieveFirstPrimaryTargeter();
		if (mainTargeter != NULL)
		{
			Unit* unit2 = mainTargeter->getPrimary();
			if (unit2 != NULL && mainTargeter->provoked)
			{
				unit2->setCurrentSkill(NO_STANDARD_SKILL);
				unit2->setCurrentTier(1);

				// Activate any status effects that occur on preparing for abilities
				for (int i = 0; i < unit2->getCurrentStatus().size(); ++i) {
					unit2->getCurrentStatus()[i]->onSelectAbility(unit2);
				}
				
				unit2->setCurrentSkill(Ability::selectSkill(unit2));

				respondAbility = Ability::getAbility(unit2->getCurrentSkill());
				if (Ability::isAbleToRespond(mainAbility, respondAbility))
					respondAbility->action(mainAbility, unit2, this);
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

    // Clean up turn data
	for (int i = 0; i < eventStack.size(); ++i)
		delete eventStack[i];
	eventStack.clear();
    if (mainAbility) delete mainAbility;
	if (respondAbility) delete respondAbility;
	// Clean up ended effects, this must be after deleted event stacks because the event stack
	// references effect names
	unit1->cleanEffects();
    
	// Increment to the next turn
	++turnIndex;
	
	// Will need to sort based on only units that have not moved yet, especially when units can start changing speeds
	sort(unitOrder.begin() + turnIndex, unitOrder.end(), compareSpeed);
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
        eventStack[i]->print();
	
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

#include "battle.h"

#include <algorithm>
#include <cstdlib>
#include "status.h"
#include "ability.h"

// Used to determine unit order, faster units are sorted to the front of the list
bool compareSpeed(Unit* lhs, Unit* rhs) {
	return lhs->getCurrentSpeed() > rhs->getCurrentSpeed() || 
		(lhs->getCurrentSpeed() == rhs->getCurrentSpeed() && lhs->getCurrentInitiative() > rhs->getCurrentInitiative());
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
	Unit* unit = unitOrder[turnIndex];
	
	// Process unit ongoing effects
	unit->processEffects();

    Ability* ability = NULL;
	// Perform the unit ability based on its position
	if (unit->isAvailable())
	{
		unit->setCurrentSkill(NO_STANDARD_SKILL);

		// Activate any status effects that occur on preparing for abilities
		for (int i = 0; i < unit->getCurrentStatus().size(); ++i) {
			unit->getCurrentStatus()[i]->onSelectAbility(unit);
		}

		// Assign the associated row ability of the current unit if no
		// ability is forced yet.
		if (unit->getCurrentSkill() == NO_STANDARD_SKILL) {
			int row = unit->getGridY();
			switch (row)
			{
			case 2:
				unit->setCurrentSkill(unit->getFrontSkill());
				break;
			case 1:
				unit->setCurrentSkill(unit->getMidSkill());
				break;
			case 0:
				unit->setCurrentSkill(unit->getBackSkill());
				break;
			}
		}

		// Execute the ability
		ability = Ability::getAbility(unit->getCurrentSkill());
		ability->action(unit, this);
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
    if (ability) delete ability;
	// Clean up ended effects, this must be after deleted event stacks because the event stack
	// references effect names
	unit->cleanEffects();
    
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
	
	if (turnIndex >= 0 && turnIndex < unitOrder.size())
	{
		Unit* unit = unitOrder[turnIndex];
		cout << unit->getName() << "'s turn";
		if (unit->isDead())
			cout << " *dead*";
		cout << endl; 
	}
	if (eventStack.size() <= 0 && roundNumber > 0)
		return;
	
    for (int i = 0; i < eventStack.size(); ++i)
        eventStack[i]->print();
    
	cout << "Battle Map: " << endl;
	group2->printGroup(true);
	cout << endl;
	group1->printGroup(false);
	
	for (int i = 0; i < unitOrder.size(); ++i)
		unitOrder[i]->print();
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

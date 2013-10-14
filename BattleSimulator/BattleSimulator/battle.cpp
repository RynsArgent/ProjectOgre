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

Battle::Battle(int seed, Group* group1, Group* group2)
	: seed(seed), group1(group1), group2(group2), roundNumber(0), turnIndex(-1), unitOrder(), 
	globalTrigger(NULL), mainUnit(NULL), respondUnit(NULL), mainAbility(NULL), respondAbility(NULL),
	eventStack(), abilStack(), cleanup(), isOver(false)
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
	globalTrigger = new Unit(NULL, -1, -1, -1);
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
		for (int i = 0; i < unitOrder.size(); ++i)
			unitOrder[i]->setDone(false);
		turnIndex = 0;
		++roundNumber;
	}

	// Retrieve the next unit in the turn list
	mainUnit = unitOrder[turnIndex];

	// Process unit beginning effects
	mainUnit->processBeginEffects();
	globalTrigger->processBeginEffects();

	if (mainUnit->isAvailable())
	{
		mainUnit->setCurrentSkill(NO_STANDARD_SKILL);
		mainUnit->setCurrentTier(2);
		// Activate any status effects that occur on preparing for abilities
		mainUnit->activateOnSelectAbility(mainUnit);
		globalTrigger->activateOnSelectAbility(mainUnit);
		mainUnit->setCurrentSkill(Ability::selectSkill(mainUnit));
	}

    mainAbility = NULL;
	// Perform the unit ability based on its position
	if (mainUnit->isAvailable())
	{
		// Execute the ability
		mainAbility = Ability::getAbility(mainUnit->getCurrentSkill());
		mainAbility->action(NULL, mainUnit, this);
	}

	// Response ability
	respondAbility = NULL;
	if (mainAbility != NULL)
	{
		// Look for the first able primary responder who can counter.
		//Targeter* mainTargeter = mainAbility->retrieveFirstPrimaryTargeter();
		bool responded = false;
		vector<Targeter*> targeters = mainAbility->getTargeters();
		for (int i = 0; i < targeters.size() && !responded; ++i)
		{
			Targeter* targeter = targeters[i];
			vector<Unit*> primaries = targeter->getPrimaries();
			for (int j = 0; j < primaries.size(); ++j)
			{
				respondUnit = primaries[j];
				if (respondUnit->isAvailable() && targeter->provoked)
				{
					respondUnit->setCurrentSkill(NO_STANDARD_SKILL);
					respondUnit->setCurrentTier(1);
					// Activate any status effects that occur on preparing for abilities
					respondUnit->activateOnSelectAbility(respondUnit);
					globalTrigger->activateOnSelectAbility(respondUnit);
					respondUnit->setCurrentSkill(Ability::selectSkill(respondUnit));

					respondAbility = Ability::getAbility(respondUnit->getCurrentSkill());
                    
                    // Check if the responding ability can respond to the previous.
					if (Ability::isAbleToRespond(this, mainAbility, mainUnit, respondAbility, respondUnit) && respondAbility->getAbilityType() != ABILITY_NONE) {
						respondAbility->action(mainAbility, respondUnit, this);
						responded = true;
						break; // found ability to counter used to counter, no need to look further
					}
					else {
						delete respondAbility;
						respondAbility = NULL;
					}
				}
			}
		}
	}
	
	// Process unit ending effects
	mainUnit->processEndEffects();
	globalTrigger->processEndEffects();

	// Clean up any units that have died
	group1->cleanDead();
	group2->cleanDead();
	// Determine whether an end result has occurred
	if (!group1->groupIsAvailable() || !group2->groupIsAvailable())
		isOver = true;
	
	// Will need to sort based on only units that have not moved yet, especially when units can start changing speeds
	sort(unitOrder.begin() + turnIndex, unitOrder.end(), compareSpeed);

	// Increment to the next turn
	mainUnit->setDone(true);
	++turnIndex;
}

void Battle::cleanupTurn()
{
    // Clean up turn data
	for (int i = 0; i < eventStack.size(); ++i)
		delete eventStack[i];
	eventStack.clear();
	for (int i = 0; i < abilStack.size(); ++i)
		delete abilStack[i];
	abilStack.clear();
	for (int i = 0; i < cleanup.size(); ++i)
		delete cleanup[i];
	cleanup.clear();

//    if (mainAbility) {
//		delete mainAbility;
//		mainAbility = NULL;
//	}
//	if (respondAbility) {
//		delete respondAbility;
//		respondAbility = NULL;
//	}
	mainAbility = NULL;
	respondAbility = NULL;
	// Clean up ended effects, this must be after deleted event stacks because the event stack
	// references effect names
	if (mainUnit) mainUnit->cleanEffects();
	if (globalTrigger) globalTrigger->cleanEffects();
}

void Battle::addToEventStack(Event* value) {
    eventStack.push_back(value);
}

void Battle::addToAbilStack(Ability* value) {
	abilStack.push_back(value);
}

void Battle::addToCleanup(StatusGroup* value) {
    cleanup.push_back(value);
}

void Battle::simulate(bool print)
{
	while (!isBattleOver() && roundNumber <= 100)
	{
		if (print)
			preprint();
		cleanupTurn();
		executeTurn();
		if (print)
			postprint();
	}
}

void Battle::preprint() const
{
	cout << "Round Number: " << roundNumber << endl;
    
	cout << "--------------------------------------------------" << endl;

	cout << "Battle Map: " << endl;
	group2->printGroup(true);
	cout << endl;
	group1->printGroup(false);
}

void Battle::postprint() const
{
	cout << "--------------------------------------------------" << endl;
	
	for (int i = 0; i < unitOrder.size(); ++i)
	{
		int prevTurn = turnIndex - 1;
		if (prevTurn < 0)
			prevTurn = unitOrder.size() - 1;
		if (i == prevTurn && turnIndex >= 0)
			cout << "*** ";
		unitOrder[i]->print();
	}

	cout << "--------------------------------------------------" << endl;
	
    for (int i = 0; i < eventStack.size(); ++i)
        eventStack[i]->print(cout);
	
	if (isBattleOver())
	{
		switch (getWinner())
		{
		case 0:
			cout << "Tie!" << endl;
			break;
		case 1:
			cout << "Group 1 Won!" << endl;
			break;
		case 2:
			cout << "Group 2 Won!" << endl;
			break;
		}
	}
	cout << endl << endl << endl;
}

int Battle::getWinner() const
{
	if (group1->groupIsAvailable())
		return 1;
	else if (group2->groupIsAvailable())
		return 2;
	else
		return 0;
}

Battle::~Battle()
{
	cleanupTurn();
	delete globalTrigger;
}
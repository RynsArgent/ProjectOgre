#include "battle.h"

#include <algorithm>
#include <cstdlib>
#include "ability.h"

// Load only abilities that are used for each battle, functions below are associated to loading and accessing abilities
Ability* abilities[NUMBER_OF_SKILLS];

void initAbilityList()
{
	for (int i = 0; i < NUMBER_OF_SKILLS; ++i)
		abilities[i] = NULL;
}

void setAbility(Skill skill)
{
	switch (skill)
	{
	case NO_STANDARD_SKILL:
		abilities[skill] = new NoStandardSkill();
		break;
	case NO_RESPONSE_SKILL:
		abilities[skill] = new NoResponseSkill();
		break;
	case HUNDRED_BLADES:
		abilities[skill] = new HundredBlades();
		break;
	case BLOCK:
		abilities[skill] = new Block();
		break;
	case STRIKE:
		abilities[skill] = new Strike();
		break;
	case TAUNT:
		abilities[skill] = new Taunt();
		break;
	case BATTLE_SHOUT:
		abilities[skill] = new BattleShout();
		break;
	case SHOOT:
		abilities[skill] = new Shoot();
		break;
	default:
		abilities[skill] = new NoStandardSkill();
		break;
	}
}

Ability* getAbility(Skill skill)
{
	if (abilities[skill] == NULL)
		setAbility(skill);
	
	return abilities[skill];
}

//

// Used to determine unit order, faster units are sorted to the front of the list
bool compareSpeed(Unit* lhs, Unit* rhs) {
	return lhs->getCurrentSpeed() > rhs->getCurrentSpeed() || 
		(lhs->getCurrentSpeed() == rhs->getCurrentSpeed() && lhs->getCurrentInitiative() > rhs->getCurrentInitiative());
}

Battle::Battle(Group* group1, Group* group2)
	: group1(group1), group2(group2), roundNumber(0), turnIndex(-1), unitOrder(), isOver(false)
{
	// Clear the ability array to null values
	initAbilityList();

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

	// Retrieve the next unit in the 
	Unit* unit = unitOrder[turnIndex];
	
	// Process unit ongoing effects
	unit->processEffects();
	unit->cleanEffects();

	// Perform the unit ability based on its position
	if (unit->isAvailable())
	{
		int row = unit->getGridY();

		Ability* ability = NULL;
		switch (row)
		{
		case 2:
			ability = getAbility(unit->getFrontSkill());
			break;
		case 1:
			ability = getAbility(unit->getMidSkill());
			break;
		case 0:
			ability = getAbility(unit->getBackSkill());
			break;
		default:
			ability = getAbility(NO_STANDARD_SKILL);
			break;
		}
		ability->action(unit, NULL, this);
	}

	// Clean up any units that have died
	group1->cleanDead();
	group2->cleanDead();

	// Increment to the next turn
	++turnIndex;

	// Determine whether an end result has occurred
	if (!group1->groupIsAvailable() || !group2->groupIsAvailable())
		isOver = true;
	
	// Will need to sort based on only units that have not moved yet, especially when units can start changing speeds
	sort(unitOrder.begin(), unitOrder.end(), compareSpeed);
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

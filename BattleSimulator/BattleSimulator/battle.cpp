#include "battle.h"

#include <algorithm>
#include <cstdlib>
#include "ability.h"

Ability* abilities[NUMBER_OF_SKILLS];

void initAbilityList()
{
	for (int i = 0; i < NUMBER_OF_SKILLS; ++i)
		abilities[i] = nullptr;
}

void setAbility(Skill skill)
{
	switch (skill)
	{
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
	default:
		abilities[skill] = new NoSkill();
		break;
	}
}

Ability* getAbility(Skill skill)
{
	if (abilities[skill] == nullptr)
		setAbility(skill);
	
	return abilities[skill];
}

bool compareSpeed(Unit* lhs, Unit* rhs)
{
	return lhs->getCurrentSpeed() < rhs->getCurrentSpeed() || 
		(lhs->getCurrentSpeed() == rhs->getCurrentSpeed() && lhs->getCurrentInitiative() < rhs->getCurrentInitiative());
}

Battle::Battle(Group* group1, Group* group2)
	: group1(group1), group2(group2), roundNumber(0), turnIndex(-1), unitOrder(), isOver(false)
{
	initAbilityList();

	group1->turnToFace(FACING_FORWARD);
	group2->turnToFace(FACING_FORWARD);

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
}

void Battle::newUnitOrder()
{
	// Set a random initiative so that units with same speed are dealt with
	for (int i = 0; i < unitOrder.size(); ++i)
		unitOrder[i]->setCurrentInitiative(rand());

	sort(unitOrder.begin(), unitOrder.end(), compareSpeed);
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

		newUnitOrder();
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

		Ability* ability = nullptr;
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
			ability = getAbility(NO_SKILL);
			break;
		}
		ability->action(unit, this);
	}

	// Clean up any units that have died
	group1->cleanDead();
	group2->cleanDead();

	// Increment to the next turn
	++turnIndex;

	// Determine whether an end result has occurred
	if (!group1->groupIsAvailable() || !group2->groupIsAvailable())
		isOver = true;
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
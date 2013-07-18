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
	if (abilities[skill] != nullptr)
		setAbility(skill);
	
	return abilities[skill];
}

bool compareSpeed(Unit* lhs, Unit* rhs)
{
	return lhs->getCurrentSpeed() < rhs->getCurrentSpeed() || 
		(lhs->getCurrentSpeed() == rhs->getCurrentSpeed() && lhs->getCurrentInitiative() < rhs->getCurrentInitiative());
}

Battle::Battle(Group* group1, Group* group2)
	: group1(group1), group2(group2), unitOrder(), statusEffects()
{
	initAbilityList();

	group1->turnToFace(FACING_FORWARD);
	group2->turnToFace(FACING_FORWARD);

	initializeUnits();
}

void Battle::initializeUnits()
{
	vector<Unit*> units1 = group1->theseUnits();
	vector<Unit*> units2 = group2->theseUnits();
	for (int i = 0; i < units1.size(); ++i)
		unitOrder.push_back(units1[i]);
	for (int i = 0; i < units2.size(); ++i)
		unitOrder.push_back(units2[i]);
	
	for (int u = 0; u < unitOrder.size(); ++u)
	{
		Unit* unit = unitOrder[u];
		statusEffects[unit] = vector<Status*>();
	}
}

void Battle::newUnitOrder()
{
	for (int i = 0; i < unitOrder.size(); ++i)
		unitOrder[i]->setCurrentInitiative(rand());

	sort(unitOrder.begin(), unitOrder.end(), compareSpeed);
}

void Battle::executeTurn()
{
	newUnitOrder();

	for (int i = 0; i < unitOrder.size(); ++i)
	{
		Unit* unit = unitOrder[i];
		int row = unit->getGridY();

		Ability* ability = NULL;
		switch (row)
		{
		case 0:
			ability = getAbility(unit->getBackSkill());
			break;
		case 1:
			ability = getAbility(unit->getMidSkill());
			break;
		case 2:
			ability = getAbility(unit->getFrontSkill());
			break;
		default:
			ability = getAbility(NO_SKILL);
			break;
		}
		ability->action(unit, this);
	}
}
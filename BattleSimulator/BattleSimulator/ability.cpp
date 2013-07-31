#include "ability.h"

#include <algorithm>
#include <cstdlib>
#include "target.h"
#include "damage.h"

// Load only abilities that are used for each battle, functions below are associated to loading and accessing abilities
// The following are definitions to define the ability list
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

// The following are definitions of specific abilities

void HundredBlades::action(Unit* current, Unit* previous, Battle* battle)
{
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	int numTimes = rand() % 3 + 1;
	for (int i = 0; i < numTimes; ++i)
	{
		int rowRange = 1;
		int initialColumnRange = 1;
		vector<Unit*> targets = Targeter::searchForFrontTargets(current, previous, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);
	
		if (targets.size() > 0)
		{
			Targeter* system = new Targeter(current, targets, TARGET_RANDOM, TARGET_UNSAFE, true);
			system->set(1);

			if (system->chosen.size() > 0) {
				Unit* target = system->chosen[0];
				Damage* damage = new Damage(this, current->getCurrentPhysicalAttack(), DAMAGE_PHYSICAL);
				damage->apply(target);
				delete damage;
				cout << current->getName() << " uses Hundred Blades on " << target->getName() << endl;
			}
			delete system;
		}
		else
			cout << current->getName() << " cannot use Hundred Blades" << endl;
	}
}

void Block::action(Unit* current, Unit* previous, Battle* battle)
{
	Group* allyGroup = battle->getAllyGroup(current->getGrid());

	int minx = 0;
	int maxx = allyGroup->getWidth() - 1;
	int miny = min(allyGroup->getHeight() - 1, current->getGridY() + 1);
	int maxy = allyGroup->getHeight() - 1;
	
	vector<Unit*> targets = allyGroup->allyUnitsAt(minx, maxx, miny, maxy);
	
	if (targets.size() > 0)
	{
		Targeter* system = new Targeter(current, targets, TARGET_RANDOM, TARGET_SAFE, true);
		system->set(1);

		if (system->chosen.size() > 0) {
			Unit* target = system->chosen[0];

			Effect *effect = new Effect("Block", current);
			Status *status = new StatusBlock(target, true, 3);
			status->setTimed(true, 1);
			status->onSpawn();
			effect->addStatus(status);
			cout << current->getName() << " uses Block on " << target->getName() << endl;
		}
		delete system;
	}
	else
		cout << current->getName() << " cannot use Block" << endl;
}

void Strike::action(Unit* current, Unit* previous, Battle* battle)
{
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
	
	int rowRange = 1;
	int initialColumnRange = 1;
	vector<Unit*> targets = Targeter::searchForFrontTargets(current, previous, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);
	
	if (targets.size() > 0)
	{
		Targeter* system = new Targeter(current, targets, TARGET_RANDOM, TARGET_UNSAFE, true);
		system->set(1);

		if (system->chosen.size() > 0) {
			Unit* target = system->chosen[0];
			Damage* damage = new Damage(this, current->getCurrentPhysicalAttack(), DAMAGE_PHYSICAL);
			damage->apply(target);
			delete damage;
		
			cout << current->getName() << " uses Strike on " << target->getName() << endl;
		}
		delete system;
	}
	else
		cout << current->getName() << " cannot use Strike" << endl;
}

void Taunt::action(Unit* current, Unit* previous, Battle* battle)
{
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
	
	vector<Unit*> targets = enemyGroup->allyUnits();
	Effect *effect = new Effect("Taunt", current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status *status = new StatusTaunt(targets[i], current);
		status->setTimed(true, 1);
		status->onSpawn();
		effect->addStatus(status);
	}
	cout << current->getName() << " uses Taunt" << endl;
}

void BattleShout::action(Unit* current, Unit* previous, Battle* battle)
{
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	
	vector<Unit*> targets = allyGroup->allyUnits();
	Effect *effect = new Effect("Battle Shout", current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status *status = new StatusBattleShout(targets[i], 1);
		status->setTimed(true, 1);
		status->onSpawn();
		effect->addStatus(status);
	}
	cout << current->getName() << " uses Battle Shout" << endl;
}

///////// SCOUT

void Shoot::action(Unit* current, Unit* previous, Battle* battle)
{
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
	
	int rowRange = 2;
	int initialColumnRange = 1;
	vector<Unit*> targets = Targeter::searchForFrontTargets(current, previous, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);
	
	if (targets.size() > 0)
	{
		Targeter* system = new Targeter(current, targets, TARGET_RANDOM, TARGET_UNSAFE, true);
		system->set(1);
		
		if (system->chosen.size() > 0) {
			Unit* target = system->chosen[0];
			Damage* damage = new Damage(this, current->getCurrentPhysicalAttack(), DAMAGE_PHYSICAL);
			damage->apply(target);
			delete damage;
		
			cout << current->getName() << " uses Shoot on " << target->getName() << endl;
		}
		delete system;
	}
	else
		cout << current->getName() << " cannot use Shoot" << endl;
}

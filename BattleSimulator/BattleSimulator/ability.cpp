#include "ability.h"

#include <algorithm>
#include <cstdlib>
#include "target.h"
#include "damage.h"

Ability* Ability::getAbility(Skill skill)
{
	switch (skill)
	{
	case NO_STANDARD_SKILL:
		return new NoStandardSkill();
	case NO_RESPONSE_SKILL:
		return new NoResponseSkill();
	case HUNDRED_BLADES:
		return new HundredBlades();
	case BLOCK:
		return new Block();
	case STRIKE:
		return new Strike();
	case TAUNT:
		return new Taunt();
	case BATTLE_SHOUT:
		return new BattleShout();
	case SHOOT:
		return new Shoot();
	default:
		return new NoStandardSkill();
	}
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
				Damage* damage = new Damage(this, current, target, Damage::getDamageValue(DAMAGE_LOW, current->getCurrentPhysicalAttack()), DAMAGE_PHYSICAL);
				damage->apply();
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

			string name = "Block";
			Effect *effect = new Effect(name, current);
			Status *status = new StatusBlock(effect, name, target, true, 30);
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
			Damage* damage = new Damage(this, current, target, Damage::getDamageValue(DAMAGE_MEDIUM, current->getCurrentPhysicalAttack()), DAMAGE_PHYSICAL);
			damage->apply();
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
	string name = "Taunt";
	Effect *effect = new Effect(name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status *status = new StatusTaunt(effect, name, targets[i], current);
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
	string name = "Battle Shout";
	Effect *effect = new Effect(name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status *status = new StatusBattleShout(effect, name, targets[i], 10);
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
			Damage* damage = new Damage(this, current, target, Damage::getDamageValue(DAMAGE_MEDIUM, current->getCurrentPhysicalAttack()), DAMAGE_PHYSICAL);
			damage->apply();
			delete damage;
		
			cout << current->getName() << " uses Shoot on " << target->getName() << endl;
		}
		delete system;
	}
	else
		cout << current->getName() << " cannot use Shoot" << endl;
}

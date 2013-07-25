#include "ability.h"

#include <cstdlib>

Ability::Ability(int cost)
	: cost(cost)
{}

void HundredBlades::action(Unit* caster, Battle* battle) const
{
	Group* allyGroup = battle->getAllyGroup(caster->getGid());
	Group* enemyGroup = battle->getEnemyGroup(caster->getGid());

	int numTimes = rand() % 3 + 1;
	for (int i = 0; i < numTimes; ++i)
	{
		int range = 1;
		int xmin = caster->getGridX() - range;
		int xmax = caster->getGridX() + range;
		vector<Unit*> targets = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmin, xmax);
		while (targets.size() <= 0 && (xmin >= 0 || xmax < enemyGroup->getWidth() - 1))
		{
			++range;
			xmin = caster->getGridX() - range;
			xmax = caster->getGridX() + range;
			Unit* u1 = allyGroup->enemyUnitFurthestInFront(enemyGroup, xmin);
			Unit* u2 = allyGroup->enemyUnitFurthestInFront(enemyGroup, xmax);
			if (u1 != nullptr)
				targets.push_back(u1);
			if (u2 != nullptr)
				targets.push_back(u2);
		} 
	
		if (targets.size() > 0)
		{
			Unit* target = nullptr;
			int randIndex = rand() % targets.size();
			target = targets[randIndex];
		
			applyDamage(caster, target, DAMAGE_MELEE);
			cout << caster->getName() << " uses Hundred Blades on " << target->getName() << endl;
		}
		else
			cout << caster->getName() << " cannot use Hundred Blades" << endl;
	}
}

void Block::action(Unit* caster, Battle* battle) const
{
	Group* allyGroup = battle->getAllyGroup(caster->getGid());

	int minx = 0;
	int maxx = allyGroup->getWidth() - 1;
	int miny = min(allyGroup->getHeight() - 1, caster->getGridY() + 1);
	int maxy = allyGroup->getHeight() - 1;
	
	vector<Unit*> targets = allyGroup->allyUnitsAt(minx, maxx, miny, maxy);
	
	if (targets.size() > 0)
	{
		int targetIndex = rand() % targets.size();
		Unit* target = targets[targetIndex];

		string name = "Block";
		Effect *effect = new Effect(name, caster);
		vector<DamageType> types;
		types.push_back(DAMAGE_MELEE);
		types.push_back(DAMAGE_RANGE);
		Status *status = new StatusDamagePrevention(name, BUFF, target, 3, types);
		status->setTimed(true);
		status->setTimer(1);
		status->init();
		effect->addStatus(status);
		cout << caster->getName() << " uses Block on " << target->getName() << endl;
	}
	else
		cout << caster->getName() << " cannot use Block" << endl;
}

void Strike::action(Unit* caster, Battle* battle) const
{
	Group* allyGroup = battle->getAllyGroup(caster->getGid());
	Group* enemyGroup = battle->getEnemyGroup(caster->getGid());
	
	int range = 1;
	int xmin = caster->getGridX() - range;
	int xmax = caster->getGridX() + range;
	vector<Unit*> targets = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmin, xmax);
	while (targets.size() <= 0 && (xmin >= 0 || xmax < enemyGroup->getWidth() - 1))
	{
		++range;
		xmin = caster->getGridX() - range;
		xmax = caster->getGridX() + range;
		Unit* u1 = allyGroup->enemyUnitFurthestInFront(enemyGroup, xmin);
		Unit* u2 = allyGroup->enemyUnitFurthestInFront(enemyGroup, xmax);
		if (u1 != nullptr)
			targets.push_back(u1);
		if (u2 != nullptr)
			targets.push_back(u2);
	} 
	
	if (targets.size() > 0)
	{
		Unit* target = nullptr;
		int randIndex = rand() % targets.size();
		target = targets[randIndex];

		for (int i = caster->getCurrentStatus().size() - 1; i >= 0; --i)
		{
			Status* status = caster->getCurrentStatus()[i];
			if (status->getType() == STATUS_TAUNT)
			{
				Unit* focus = dynamic_cast<StatusTaunt*>(status)->getFocus();
			
				int xminmod = enemyGroup->getWidth() - xmax - 1;
				int xmaxmod = enemyGroup->getWidth() - xmin - 1;
				if (enemyGroup->allyUnitFurthestInFront(focus->getGridX()) == focus && // Front row reachability test
					enemyGroup->withinColumnRange(focus->getGridX(), xminmod, xmaxmod)) // Column range test
				{
					target = focus;
					break;
				}
			}
		}

		applyDamage(caster, target, DAMAGE_MELEE);
		cout << caster->getName() << " uses Strike on " << target->getName() << endl;
	}
	else
		cout << caster->getName() << " cannot use Strike" << endl;
}

void Taunt::action(Unit* caster, Battle* battle) const
{
	Group* enemyGroup = battle->getEnemyGroup(caster->getGid());
	
	vector<Unit*> targets = enemyGroup->allyUnits();
	string name = "Taunt";
	Effect *effect = new Effect(name, caster);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status *status = new StatusTaunt(name, NEUTRAL, targets[i], caster);
		status->setTimed(true);
		status->setTimer(1);
		status->init();
		effect->addStatus(status);
	}
	cout << caster->getName() << " uses Taunt" << endl;
}

void BattleShout::action(Unit* caster, Battle* battle) const
{
	Group* allyGroup = battle->getAllyGroup(caster->getGid());
	
	vector<Unit*> targets = allyGroup->allyUnits();
	string name = "Battle Shout";
	Effect *effect = new Effect(name, caster);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status *status = new StatusAttackBonus(name, BUFF, targets[i], 1);
		status->setTimed(true);
		status->setTimer(1);
		status->init();
		effect->addStatus(status);
	}
	cout << caster->getName() << " uses Battle Shout" << endl;
}

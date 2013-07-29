#include "ability.h"

#include <algorithm>
#include <cstdlib>
#include "damage.h"

bool compareLifePreferLess(Unit* lhs, Unit* rhs) {
	return lhs->getCurrentHealth() < rhs->getCurrentHealth();
}
bool compareLifePreferMore(Unit* lhs, Unit* rhs) {
	return lhs->getCurrentHealth() > rhs->getCurrentHealth();
}

void HundredBlades::action(Unit* current, Unit* previous, Battle* battle)
{
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	int numTimes = rand() % 3 + 1;
	for (int i = 0; i < numTimes; ++i)
	{
		int adjacencyRange = 1;
		int xmin = current->getGridX() - adjacencyRange;
		int xmax = current->getGridX() + adjacencyRange;
		vector<Unit*> targets = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmin, xmax);
		while (targets.size() <= 0 && (xmin >= 0 || xmax < enemyGroup->getWidth() - 1))
		{
			++adjacencyRange;
			xmin = current->getGridX() - adjacencyRange;
			xmax = current->getGridX() + adjacencyRange;
			vector<Unit*> uLeft = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmin, xmin);
			vector<Unit*> uRight = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmax, xmax);
			for (int i = 0; i < uLeft.size(); ++i)
				targets.push_back(uLeft[i]);
			for (int i = 0; i < uRight.size(); ++i)
				targets.push_back(uRight[i]);
		} 
	
		if (targets.size() > 0)
		{
			Unit* target = NULL;
			int randIndex = rand() % targets.size();
			target = targets[randIndex];

			Damage(this, current->getCurrentPhysicalAttack(), DAMAGE_PHYSICAL).apply(target);
			cout << current->getName() << " uses Hundred Blades on " << target->getName() << endl;
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
		int targetIndex = rand() % targets.size();
		Unit* target = targets[targetIndex];

		string name = "Block";
		Effect *effect = new Effect(name, current);
		vector<DamageType> types;
		types.push_back(DAMAGE_PHYSICAL);
		Status *status = new StatusDamagePrevention(name, BUFF, target, true, 3, types);
		status->setTimed(true);
		status->setTimer(1);
		status->init();
		effect->addStatus(status);
		cout << current->getName() << " uses Block on " << target->getName() << endl;
	}
	else
		cout << current->getName() << " cannot use Block" << endl;
}

void Strike::action(Unit* current, Unit* previous, Battle* battle)
{
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
	
	int adjacencyRange = 1;
	int xmin = current->getGridX() - adjacencyRange;
	int xmax = current->getGridX() + adjacencyRange;
	vector<Unit*> targets = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmin, xmax);
	while (targets.size() <= 0 && (xmin >= 0 || xmax < enemyGroup->getWidth() - 1))
	{
		++adjacencyRange;
		xmin = current->getGridX() - adjacencyRange;
		xmax = current->getGridX() + adjacencyRange;
		vector<Unit*> uLeft = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmin, xmin);
		vector<Unit*> uRight = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmax, xmax);
		for (int i = 0; i < uLeft.size(); ++i)
			targets.push_back(uLeft[i]);
		for (int i = 0; i < uRight.size(); ++i)
			targets.push_back(uRight[i]);
	} 
	
	if (targets.size() > 0)
	{
		Unit* target = NULL;
		switch (allyGroup->getTargetOrder())
		{
		case TARGET_RANDOM:
			{
				int randIndex = rand() % targets.size();
				target = targets[randIndex];
				break;
			}
		case TARGET_WEAKEST:
			{
				sort(targets.begin(), targets.end(), compareLifePreferLess);
				target = targets[0];
				break;
			}
		case TARGET_STRONGEST:
			{
				sort(targets.begin(), targets.end(), compareLifePreferMore);
				target = targets[0];
				break;
			}
		}

		for (int i = current->getCurrentStatus().size() - 1; i >= 0; --i)
		{
			Status* status = current->getCurrentStatus()[i];
			if (status->getType() == STATUS_TAUNT)
			{
				Unit* focus = dynamic_cast<StatusTaunt*>(status)->getFocus();
			
				vector<Unit*> uCol = enemyGroup->allyUnitsFurthestInFront(focus->getGridX(), focus->getGridX());
				int xminmod = enemyGroup->getWidth() - xmax - 1;
				int xmaxmod = enemyGroup->getWidth() - xmin - 1;
				if ((uCol.size() > 0 && find(uCol.begin(), uCol.end(), focus) != uCol.end()) && // Front row reachability test
					enemyGroup->withinColumnRange(focus->getGridX(), xminmod, xmaxmod)) // Column range test
				{
					target = focus;
					break;
				}
			}
		}
		
		Damage(this, current->getCurrentPhysicalAttack(), DAMAGE_PHYSICAL).apply(target);
		cout << current->getName() << " uses Strike on " << target->getName() << endl;
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
		Status *status = new StatusTaunt(name, NEUTRAL, targets[i], current);
		status->setTimed(true);
		status->setTimer(1);
		status->init();
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
		Status *status = new StatusAttackBonus(name, BUFF, targets[i], 1);
		status->setTimed(true);
		status->setTimer(1);
		status->init();
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
	int adjacencyRange = 1;
	int xmin = current->getGridX() - adjacencyRange;
	int xmax = current->getGridX() + adjacencyRange;
	vector<Unit*> targets = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmin, xmax, rowRange);
	while (targets.size() <= 0 && (xmin >= 0 || xmax < enemyGroup->getWidth() - 1))
	{
		++adjacencyRange;
		xmin = current->getGridX() - adjacencyRange;
		xmax = current->getGridX() + adjacencyRange;
		vector<Unit*> uLeft = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmin, xmin, rowRange);
		vector<Unit*> uRight = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmax, xmin, rowRange);
		for (int i = 0; i < uLeft.size(); ++i)
			targets.push_back(uLeft[i]);
		for (int i = 0; i < uRight.size(); ++i)
			targets.push_back(uRight[i]);
	} 
	
	if (targets.size() > 0)
	{
		Unit* target = NULL;
		switch (allyGroup->getTargetOrder())
		{
		case TARGET_RANDOM:
			{
				int randIndex = rand() % targets.size();
				target = targets[randIndex];
				break;
			}
		case TARGET_WEAKEST:
			{
				sort(targets.begin(), targets.end(), compareLifePreferLess);
				target = targets[0];
				break;
			}
		case TARGET_STRONGEST:
			{
				sort(targets.begin(), targets.end(), compareLifePreferMore);
				target = targets[0];
				break;
			}
		}

		for (int i = current->getCurrentStatus().size() - 1; i >= 0; --i)
		{
			Status* status = current->getCurrentStatus()[i];
			if (status->getType() == STATUS_TAUNT)
			{
				Unit* focus = dynamic_cast<StatusTaunt*>(status)->getFocus();
			
				vector<Unit*> uCol = enemyGroup->allyUnitsFurthestInFront(focus->getGridX(), focus->getGridX(), rowRange);
				int xminmod = enemyGroup->getWidth() - xmax - 1;
				int xmaxmod = enemyGroup->getWidth() - xmin - 1;
				if ((uCol.size() > 0 && find(uCol.begin(), uCol.end(), focus) != uCol.end()) && // Front row reachability test
					enemyGroup->withinColumnRange(focus->getGridX(), xminmod, xmaxmod)) // Column range test
				{
					target = focus;
					break;
				}
			}
		}
		
		Damage(this, current->getCurrentPhysicalAttack(), DAMAGE_PHYSICAL).apply(target);
		cout << current->getName() << " uses Shoot on " << target->getName() << endl;
	}
	else
		cout << current->getName() << " cannot use Shoot" << endl;
}

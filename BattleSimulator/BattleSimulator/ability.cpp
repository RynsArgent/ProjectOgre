#include "ability.h"


Ability::Ability(int cost)
	: cost(cost)
{}

void HundredBlades::action(Unit* caster, Battle* battle) const
{
	Group* allyGroup = battle->getAllyGroup(caster->getGid());
	Group* enemyGroup = battle->getEnemyGroup(caster->getGid());

	int minx = max(0, caster->getGridX() - 1);
	int maxx = min(enemyGroup->getWidth() - 1, caster->getGridX() + 1);

	int numTimes = rand() % 3 + 1;
	for (int i = 0; i < numTimes; ++i)
	{
		vector<Unit*> targets = enemyGroup->enemyUnitsFurthestInFront(enemyGroup, minx, maxx);
		int targetIndex = rand() % targets.size();

		applyDamage(caster, targets[targetIndex], DAMAGE_MELEE);
	}
}

void Block::action(Unit* caster, Battle* battle) const
{
}

void Strike::action(Unit* caster, Battle* battle) const
{
	Group* allyGroup = battle->getAllyGroup(caster->getGid());
	Group* enemyGroup = battle->getEnemyGroup(caster->getGid());
	
	int minx = max(0, caster->getGridX() - 1);
	int maxx = min(enemyGroup->getWidth() - 1, caster->getGridX() + 1);
	
	vector<Unit*> targets = enemyGroup->enemyUnitsFurthestInFront(enemyGroup, minx, maxx);
	int targetIndex = rand() % targets.size();

	applyDamage(caster, targets[targetIndex], DAMAGE_MELEE);
}

void Taunt::action(Unit* caster, Battle* battle) const
{
}

void BattleShout::action(Unit* caster, Battle* battle) const
{
}


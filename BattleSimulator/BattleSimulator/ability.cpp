#include "ability.h"

#include <algorithm>
#include <cstdlib>
#include "unit.h"
#include "target.h"
#include "damage.h"
#include "status.h"
#include "battle.h"

Ability* Ability::getAbility(Skill skill)
{
	switch (skill)
	{   
	case NO_STANDARD_SKILL:
		return new NoSkill();
    case NO_RESPONSE_SKILL:
        return new NoSkill();
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
		return new NoSkill();
	}
}

// Executed after certain triggers during the execution of a single ability,
// true means that the ability can continue as planned.
bool Ability::checkpoint(Unit* current)
{
    for (int i = 0; i < current->getCurrentStatus().size(); ++i)
        current->getCurrentStatus()[i]->onCheckpoint(this);
    return isCancelled();
}

void Ability::print() const
{
    cout << "Ability " << getName() << " from " << source->getName() << endl;
}

Ability::~Ability()
{
    if (targeter) delete targeter;
}

// The following are definitions of specific abilities

void HundredBlades::action(Unit* current, Battle* battle)
{
    Ability::action(current, battle);
    
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	int numTimes = rand() % 3 + 2;
	for (int i = 0; i < numTimes; ++i)
	{
        if (checkpoint(current)) return;
        
		int rowRange = 1;
		int initialColumnRange = 1;
		vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);
	
		if (targets.size() > 0)
		{
			targeter = new Targeter(current, battle, targets, TARGET_ENEMIES, TARGET_RANDOM, true);
			targeter->set(1);

            if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];

				Damage* damage = new Damage(this, target, Damage::getDamageValue(DAMAGE_LOW, current->getCurrentPhysicalAttack()), DAMAGE_PHYSICAL);
				
                Event* event = new Event(this, damage, NULL);
                event->apply();
			}
		}
	}
}

void Block::action(Unit* current, Battle* battle)
{
    Ability::action(current, battle);
    
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
    
    if (checkpoint(current)) return;
    
	int minx = 0;
	int maxx = allyGroup->getWidth() - 1;
	int miny = min(allyGroup->getHeight() - 1, current->getGridY() + 1);
	int maxy = allyGroup->getHeight() - 1;
	
	vector<Unit*> targets = allyGroup->allyUnitsAt(minx, maxx, miny, maxy);
	
	if (targets.size() > 0)
	{
		targeter = new Targeter(current, battle, targets, TARGET_ALLIES, TARGET_RANDOM, true);
		targeter->set(1);

        if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];

			string name = "Block";
			Effect* effect = new Effect(current, battle, name, current);
			
			Status* status = new StatusBlock(effect, name, target, true, 30);
			//Status* status = new StatusFlee(effect, "Flee", target);
			status->setTimed(true, 3);
			
			Event* event = new Event(this, NULL, status);
			event->apply();
			
			effect->applyEffect();
		}
	}
}

void Strike::action(Unit* current, Battle* battle)
{
    Ability::action(current, battle);
    
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
	
    if (checkpoint(current)) return;
    
	int rowRange = 1;
	int initialColumnRange = 1;
	vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);
	
	if (targets.size() > 0)
	{
		targeter = new Targeter(current, battle, targets, TARGET_ENEMIES, TARGET_RANDOM, true);
		targeter->set(1);

        if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];

			Damage* damage = new Damage(this, target, Damage::getDamageValue(DAMAGE_MEDIUM, current->getCurrentPhysicalAttack()), DAMAGE_PHYSICAL);
			
			Event* event = new Event(this, damage, NULL);
			event->apply();
		}
	}
}

void Taunt::action(Unit* current, Battle* battle)
{
    Ability::action(current, battle);
    
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
    
    if (checkpoint(current)) return;
	
	vector<Unit*> targets = enemyGroup->allyUnits();
	/*
	string name = "Taunt";
	Effect* effect = new Effect(name, current, current, battle);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusTaunt(effect, name, targets[i], current);
		status->setTimed(true, 1);
		
		Applier* applier = new Applier(NULL, status);
		applier->apply();
		appliers.push_back(applier);
	}
	effect->applyEffect();
	*/
	string name = "Poison";
	for (int i = 0; i < targets.size(); ++i)
	{
		Effect* effect = new Effect(current, battle, name, targets[i]);

		Status* status = new StatusPoison(effect, name, targets[i], 10);
		status->setTimed(true, 1);
		
        Event* event = new Event(this, NULL, status);
        event->apply();

		effect->applyEffect();
	}
}

void BattleShout::action(Unit* current, Battle* battle)
{
    Ability::action(current, battle);
    
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
    
    if (checkpoint(current)) return;
	
	vector<Unit*> targets = allyGroup->allyUnits();
	string name = "BattleShout";
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusBattleShout(effect, name, targets[i], 10);
		//Status* status = new StatusConfusion(effect, "Confusion", targets[i]);
		//Status* status = new StatusCharm(effect, "Charm", targets[i]);
		//Status* status = new StatusStun(effect, "Stun", targets[i]);
		//Status* status = new StatusSleep(effect, "Sleep", targets[i]);
		//Status* status = new StatusFlee(effect, "Flee", targets[i]);
		status->setTimed(true, 1);
		
        Event* event = new Event(this, NULL, status);
        event->apply();
	}
	effect->applyEffect();
}

///////// SCOUT

void Shoot::action(Unit* current, Battle* battle)
{
    Ability::action(current, battle);
    
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
    
    if (checkpoint(current)) return;
	
	int rowRange = 2;
	int initialColumnRange = 1;
	vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);
	
	if (targets.size() > 0)
	{
		targeter = new Targeter(current, battle, targets, TARGET_ENEMIES, TARGET_RANDOM, true);
		targeter->set(1);

        if (checkpoint(current)) return;
        
		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];

			Damage* damage = new Damage(this, target, Damage::getDamageValue(DAMAGE_MEDIUM, current->getCurrentPhysicalAttack()), DAMAGE_PHYSICAL);
			
			Event* event = new Event(this, damage, NULL);
			event->apply();
		}
	}
}

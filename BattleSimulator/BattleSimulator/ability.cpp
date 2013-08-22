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
	case HASTE:
		return new Haste();
	case SCOPE:
		return new Scope();
	case TANGLE_TRAP:
		return new TangleTrap();
	case HEAL:
		return new Heal();
	case CLEANSE:
		return new Cleanse();
	default:
		return new NoSkill();
	}
}

Skill Ability::selectSkill(Unit* unit)
{
	switch (unit->getCurrentTier())
	{
	case 0:
		// The lowest tier means no action
		return NO_STANDARD_SKILL;
	case 1:
		// Level 1 tier is a basic skill
		return unit->getBasicSkill();
	case 2:
		{
			// Assign the associated row ability of the current unit if no
			// ability is forced yet.
			int row = unit->getGridY();
			switch (row)
			{
			case 2:
				return unit->getFrontSkill();
			case 1:
				return unit->getMidSkill();
			case 0:
				return unit->getBackSkill();
			}
			return NO_STANDARD_SKILL;
		}
	}
	return NO_STANDARD_SKILL;
}

bool Ability::isAbleToRespond(Ability* previous, Ability* current)
{
	if (!previous->isRespondable())
		return false;
	switch (current->getAbilityType())
	{
	case ABILITY_ATTACK_MELEE:
		return previous->getAbilityType() == ABILITY_ATTACK_MELEE;
	case ABILITY_ATTACK_RANGE:
		return previous->getAbilityType() == ABILITY_ATTACK_RANGE;
	}
	return false;
}

Targeter* Ability::retrieveFirstPrimaryTargeter() const
{
	for (int i = 0; i < targeters.size(); ++i)
	{
		Unit* primary = targeters[i]->getPrimary();
		if (primary && primary->isAvailable())
			return targeters[i];
	}
	return NULL;
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
}

Ability::~Ability()
{
	for (int i = 0; i < targeters.size(); ++i)
		delete targeters[i];
}

// The following are definitions of specific abilities

void HundredBlades::action(Ability* previous, Unit* current, Battle* battle)
{
    Ability::action(previous, current, battle);

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	int numTimes = rand() % 3 + 1;
	for (int i = 0; i < numTimes; ++i)
	{
		if (checkpoint(current)) return;

		int rowRange = 1;
		int initialColumnRange = 1;
		vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);

		if (targets.size() > 0)
		{
			Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), true);
			targeter->set(1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
				targeter->provoked = true;

				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, 100, damage);
				log->apply();
			}
			targeters.push_back(targeter);
		}
	}
}

void Block::action(Ability* previous, Unit* current, Battle* battle)
{
    Ability::action(previous, current, battle);
    
    if (checkpoint(current)) return;
    
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
    
	int minx = 0;
	int maxx = allyGroup->getWidth() - 1;
	int miny = min(allyGroup->getHeight() - 1, current->getGridY() + 1);
	int maxy = allyGroup->getHeight() - 1;
	
	vector<Unit*> targets = allyGroup->allyUnitsAt(minx, maxx, miny, maxy);
	
	if (targets.size() > 0)
	{
		Targeter* targeter = new Targeter(this, targets, TARGET_ALLIES, TARGET_RANDOM, true);
		targeter->set(1);

        if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];

			string name = "Block";
			Effect* effect = new Effect(current, battle, name, current);
			
			Status* status = new StatusBlock(effect, name, target, true, 30);
			//Status* status = new StatusFlee(effect, "Flee", target);
			status->setTimed(true, 1);
			
			Event* log = new EventCauseStatus(this, 100, status);
			log->apply();

			effect->applyEffect();
		}
		targeters.push_back(targeter);
	}
}

void Strike::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;
	
	if (previous != NULL)
	{
		if (previous->getSource() != NULL && previous->getSource()->isAvailable())
		{
			Unit* target = previous->getSource();
			
			Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);
				
			Event* log = new EventCauseDamage(this, 100, damage);
			log->apply();
		}
	}
	else
	{
		Group* allyGroup = battle->getAllyGroup(current->getGrid());
		Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
    
		int rowRange = 1;
		int initialColumnRange = 1;
		vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);
	
		if (targets.size() > 0)
		{
			Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), true);
			targeter->set(1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
				targeter->provoked = true;

				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);
			
				Event* log = new EventCauseDamage(this, 100, damage);
				log->apply();
			}
			targeters.push_back(targeter);
		}
	}
}

void Taunt::action(Ability* previous, Unit* current, Battle* battle)
{
    Ability::action(previous, current, battle);
    
    if (checkpoint(current)) return;
    
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
	
	vector<Unit*> targets = enemyGroup->allyUnits();
	string name = "Taunt";
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusTaunt(effect, name, targets[i], current);
		status->setTimed(true, 1);
		
		Event* log = new EventCauseStatus(this, 100, status);
		log->apply();
	}
	effect->applyEffect();
	/*
	string name = "Blind";
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusBlind(effect, name, targets[i]);
		status->setTimed(true, 1);
		
		Event* log = new EventCauseStatus(this, 100, status);
		log->apply();
	}
	effect->applyEffect();
	string name = "Mortality";
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusMortality(effect, name, targets[i], 10);
		status->setTimed(true, 3);
		
		Event* log = new EventCauseStatus(this, 100, status);
		log->apply();
	}
	effect->applyEffect();
	string name = "Poison";
	for (int i = 0; i < targets.size(); ++i)
	{
		Effect* effect = new Effect(current, battle, name, targets[i]);

		Status* status = new StatusPoison(effect, name, targets[i], 10);
		status->setTimed(true, 1);
		
		Event* log = new EventCauseStatus(this, 100, status);
		log->apply();

		effect->applyEffect();
	}
	*/
}

void BattleShout::action(Ability* previous, Unit* current, Battle* battle)
{
    Ability::action(previous, current, battle);
    
    if (checkpoint(current)) return;
    
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	
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

		Event* log = new EventCauseStatus(this, 100, status);
		log->apply();
	}
	effect->applyEffect();
}

void Shoot::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	if (previous != NULL)
	{
		if (previous->getSource() != NULL && previous->getSource()->isAvailable() &&
			previous->getAbilityType() == ABILITY_ATTACK_RANGE)
		{
			Unit* target = previous->getSource();
			
			Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);
			
			Event* log = new EventCauseDamage(this, 100, damage);
			log->apply();
		}
	}
	else
	{
		Group* allyGroup = battle->getAllyGroup(current->getGrid());
		Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
	
		int rowRange = 2;
		int initialColumnRange = 1;
		vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);
	
		if (targets.size() > 0)
		{
			Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), true);
			targeter->set(1);

			if (checkpoint(current)) return;
        
			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
				targeter->provoked = true;
				
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);
			
				Event* log = new EventCauseDamage(this, 100, damage);
				log->apply();
			}
			targeters.push_back(targeter);
		}
	}
}

void Haste::action(Ability* previous, Unit* current, Battle* battle)
{
    Ability::action(previous, current, battle);
    
    if (checkpoint(current)) return;
    
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	
	vector<Unit*> targets = allyGroup->allyUnits();
	string name = "Haste";
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusHaste(effect, name, targets[i], 1);
		status->setTimed(true, 1);
		
		Event* log = new EventCauseStatus(this, 100, status);
		log->apply();
	}
	effect->applyEffect();
}

void Scope::action(Ability* previous, Unit* current, Battle* battle)
{
    Ability::action(previous, current, battle);
    
    if (checkpoint(current)) return;
    
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	
	vector<Unit*> targets = allyGroup->allyUnits();
	string name = "Scope";
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusScope(effect, name, targets[i], 10);
		status->setTimed(true, 1);
		
		Event* log = new EventCauseStatus(this, 100, status);
		log->apply();
	}
	effect->applyEffect();
}

void TangleTrap::action(Ability* previous, Unit* current, Battle* battle)
{   
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;
	
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	
	vector<Unit*> targets = allyGroup->allyUnitsFurthestInFront(0, allyGroup->getWidth() - 1);
	
	if (targets.size() > 0)
	{
		Targeter* targeter = new Targeter(this, targets, TARGET_ALLIES, TARGET_RANDOM, true);
		targeter->set(1);

		if (checkpoint(current)) return;
		
		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];

			string name = "TangleTrap";
			Effect* effect = new Effect(current, battle, name, current);
			
			Status* status = new StatusTangleTrap(effect, name, target);
			status->setTimed(true, 1);
			
			Event* log = new EventCauseStatus(this, 100, status);
			log->apply();

			effect->applyEffect();
		}
		targeters.push_back(targeter);
	}
}

void Heal::action(Ability* previous, Unit* current, Battle* battle)
{   
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	if (previous != NULL)
	{
		if (previous->getSource() != NULL && previous->getSource()->isAvailable())
		{
			Unit* target = current;

			Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_HEALING);
			
			Event* log = new EventCauseDamage(this, 100, damage);
			log->apply();
		}
	}
	else
	{
		Group* allyGroup = battle->getAllyGroup(current->getGrid());
	
		vector<Unit*> targets = allyGroup->allyUnits();
	
		if (targets.size() > 0)
		{
			Targeter* targeter = new Targeter(this, targets, TARGET_ALLIES, TARGET_WEAKEST, true);
			targeter->set(1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
			
				Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_HEALING);
			
				Event* log = new EventCauseDamage(this, 100, damage);
				log->apply();
			}
			targeters.push_back(targeter);
		}
	}
}

void Cleanse::action(Ability* previous, Unit* current, Battle* battle)
{   
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;
	
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	
	vector<Unit*> targets = allyGroup->allyUnits();

	if (targets.size() > 0)
	{
		Targeter* targeter = new Targeter(this, targets, TARGET_ALLIES, TARGET_MOST_DEBUFFS, true);
		targeter->set(1);

		if (checkpoint(current)) return;
		
		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];

			Event* log = new EventRemoveStatus(this, 100, target, DEBUFF);
			log->apply();
		}
		targeters.push_back(targeter);
	}
}

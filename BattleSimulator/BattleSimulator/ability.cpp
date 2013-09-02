#include "ability.h"

#include <algorithm>
#include <cstdlib>
#include "event.h"
#include "unit.h"
#include "target.h"
#include "damage.h"
#include "status.h"
#include "group.h"
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
	case REJUVENATE:
		return new Rejuvenate();
	case BLIND:
		return new Blind();
	case BARRIER:
		return new Barrier();
	case POLYMORPH:
		return new Polymorph();
	case FIREBALL:
		return new Fireball();
	case WATER_JET:
		return new WaterJet();
	case ACID_DART:
		return new AcidDart();
	case FROST_SHARD:
		return new FrostShard();
	case LIGHTNING_BOLT:
		return new LightningBolt();
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
	return true;
}

void Ability::action(Ability* previous, Unit* current, Battle* battle) 
{
	this->source = current;
	this->battle = battle;
	Event* event = new Event(this);
};

Targeter* Ability::retrieveFirstPrimaryTargeter() const
{
	for (int i = 0; i < targeters.size(); ++i)
	{
		Unit* primary = targeters[i]->getPrimary();
		if (primary && primary->isAvailable()) {
			return targeters[i];
		}
	}
	return NULL;
}

// Executed after certain triggers during the execution of a single ability,
// true means that the ability can continue as planned.
bool Ability::checkpoint(Unit* current)
{
	current->activateOnCheckpoint(this);
    return isCancelled();
}

void Ability::print(ostream& out) const
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

				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_LOW, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, Event::MELEE_HIT_CHANCE, damage);
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
			
			Status* status = new StatusBlock(effect, target);
			status->setTimed(true, 1);
			
			Event* log = new EventCauseStatus(this, Event::BUFF_HIT_CHANCE, status);
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
				
			Event* log = new EventCauseDamage(this, Event::MELEE_HIT_CHANCE, damage);
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
			
				Event* log = new EventCauseDamage(this, Event::MELEE_HIT_CHANCE, damage);
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
		Status* status = new StatusTaunt(effect, targets[i], current);
		
		Event* log = new EventCauseStatus(this, Event::DEBUFF_HIT_CHANCE, status);
		log->apply();
	}
	effect->applyEffect();
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
		Status* status = new StatusBattleShout(effect, targets[i], 1);
		status->setTimed(true, 1);

		Event* log = new EventCauseStatus(this, Event::BUFF_HIT_CHANCE, status);
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
			
			Event* log = new EventCauseDamage(this, Event::RANGE_HIT_CHANCE, damage);
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
			
				Event* log = new EventCauseDamage(this, Event::RANGE_HIT_CHANCE, damage);
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
		Status* status = new StatusHaste(effect, targets[i], 1);
		
		Event* log = new EventCauseStatus(this, Event::BUFF_HIT_CHANCE, status);
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
		Status* status = new StatusScope(effect, targets[i], 1);
		
		Event* log = new EventCauseStatus(this, Event::BUFF_HIT_CHANCE, status);
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
			
			Status* status = new StatusTangleTrap(effect, target);
			
			Event* log = new EventCauseStatus(this, Event::BUFF_HIT_CHANCE, status);
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
		if (previous->getSource() != NULL)
		{
			Unit* target = current;

			Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_HEALING);
			
			Event* log = new EventCauseDamage(this, Event::HEAL_HIT_CHANCE, damage);
			log->apply();
		}
	}
	else
	{
		Group* allyGroup = battle->getAllyGroup(current->getGrid());
	
		vector<Unit*> targets = allyGroup->allyUnits();
	
		if (targets.size() > 0)
		{
			Targeter* targeter = new Targeter(this, targets, TARGET_ALLIES, TARGET_HEALING, true);
			targeter->set(1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
			
				Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_HEALING);
			
				Event* log = new EventCauseDamage(this, Event::HEAL_HIT_CHANCE, damage);
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

			Event* log = new EventRemoveStatus(this, Event::CLEANSE_HIT_CHANCE, target, DEBUFF);
			log->apply();
		}
		targeters.push_back(targeter);
	}
}

void Rejuvenate::action(Ability* previous, Unit* current, Battle* battle)
{   
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	if (previous != NULL)
	{
		Unit* target = current;

		string name = "Regeneration";
		Effect* effect = new Effect(current, battle, name, target);
	
		Status* status = new StatusRegeneration(effect, target, 1);

		Event* log = new EventCauseStatus(this, Event::BUFF_HIT_CHANCE, status);
		log->apply();
			
		effect->applyEffect();
	}
	else
	{
		Group* allyGroup = battle->getAllyGroup(current->getGrid());
	
		vector<Unit*> targets = allyGroup->allyUnits();
	
		if (targets.size() > 0)
		{
			Targeter* targeter = new Targeter(this, targets, TARGET_ALLIES, TARGET_HEALING, true);
			targeter->set(1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];

				string name = "Regeneration";
				Effect* effect = new Effect(current, battle, name, target);
	
				Status* status = new StatusRegeneration(effect, target, 1);
	
				Event* log = new EventCauseStatus(this, Event::BUFF_HIT_CHANCE, status);
				log->apply();
			
				effect->applyEffect();
			}
			targeters.push_back(targeter);
		}
	}
}

void Blind::action(Ability* previous, Unit* current, Battle* battle)
{   
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	if (previous != NULL)
	{
		if (previous->getSource() != NULL && previous->getSource()->isAvailable())
		{
			Unit* target = previous->getSource();

			string name = "Blind";
			Effect* effect = new Effect(current, battle, name, target);
	
			Status* status = new StatusBlind(effect, target, 1);

			Event* log = new EventCauseStatus(this, Event::DEBUFF_HIT_CHANCE, status);
			log->apply();
			
			effect->applyEffect();
		}
	}
	else
	{
		Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
	
		vector<Unit*> targets = enemyGroup->allyUnits();
	
		if (targets.size() > 0)
		{
			Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, TARGET_RANDOM, true);
			targeter->set(1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];

				string name = "Blind";
				Effect* effect = new Effect(current, battle, name, target);
	
				Status* status = new StatusBlind(effect, target, 1);

				Event* log = new EventCauseStatus(this, Event::DEBUFF_HIT_CHANCE, status);
				log->apply();
			
				effect->applyEffect();
			}
			targeters.push_back(targeter);
		}
	}
}

void Barrier::action(Ability* previous, Unit* current, Battle* battle)
{
    Ability::action(previous, current, battle);
    
    if (checkpoint(current)) return;
    
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	
	vector<Unit*> targets = allyGroup->allyUnits();
	string name = "Barrier";
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusBarrier(effect, targets[i], 1);

		Event* log = new EventCauseStatus(this, Event::BUFF_HIT_CHANCE, status);
		log->apply();
	}
	effect->applyEffect();
}

void Polymorph::action(Ability* previous, Unit* current, Battle* battle)
{   
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
	
	vector<Unit*> targets = enemyGroup->allyUnits();
	
	if (targets.size() > 0)
	{
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, TARGET_RANDOM, true);
		targeter->set(1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];

			string name = "Polymorph";
			Effect* effect = new Effect(current, battle, name, target);

			Status* status = new StatusPolymorph(effect, target, 1);

			Event* log = new EventCauseStatus(this, Event::DEBUFF_HIT_CHANCE, status);
			log->apply();
			
			effect->applyEffect();
		}
		targeters.push_back(targeter);
	}
}

void Fireball::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

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
				
			Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_FIRE);
			
			Event* log = new EventCauseDamage(this, Event::MAGIC_HIT_CHANCE, damage);
			log->apply();
				
			if (damage->final > 0)
			{
				string name = "Burn";
				Effect* effect = new Effect(current, battle, name, target);

				Status* status = new StatusBurn(effect, target, 1);

				Event* log = new EventCauseStatus(this, Event::DEBUFF_HIT_CHANCE, status);
				log->apply();
			
				effect->applyEffect();
			}
		}
		targeters.push_back(targeter);
	}
}

void WaterJet::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

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
				
			Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_WATER);
			
			Event* log = new EventCauseDamage(this, Event::MAGIC_HIT_CHANCE, damage);
			log->apply();
				
			if (damage->final > 0)
			{
				string name = "Flee";
				Effect* effect = new Effect(current, battle, name, target);

				Status* status = new StatusFlee(effect, target, 1);

				Event* log = new EventCauseStatus(this, Event::DEBUFF_HIT_CHANCE, status);
				log->apply();
			
				effect->applyEffect();
			}
		}
		targeters.push_back(targeter);
	}
}

void AcidDart::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

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
				
			Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_EARTH);
			
			Event* log = new EventCauseDamage(this, Event::MAGIC_HIT_CHANCE, damage);
			log->apply();
				
			if (damage->final > 0)
			{
				string name = "Poison";
				Effect* effect = new Effect(current, battle, name, target);

				Status* status = new StatusPoison(effect, target, 1);

				Event* log = new EventCauseStatus(this, Event::DEBUFF_HIT_CHANCE, status);
				log->apply();
			
				effect->applyEffect();
			}
		}
		targeters.push_back(targeter);
	}
}

void FrostShard::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

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
				
			Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_ICE);
			
			Event* log = new EventCauseDamage(this, Event::MAGIC_HIT_CHANCE, damage);
			log->apply();
				
			if (damage->final > 0)
			{
				string name = "Chill";
				Effect* effect = new Effect(current, battle, name, target);

				Status* status = new StatusChill(effect, target, 1);

				Event* log = new EventCauseStatus(this, Event::DEBUFF_HIT_CHANCE, status);
				log->apply();
			
				effect->applyEffect();
			}
		}
		targeters.push_back(targeter);
	}
}

void LightningBolt::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

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
				
			Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_LIGHTNING);
			
			Event* log = new EventCauseDamage(this, Event::MAGIC_HIT_CHANCE, damage);
			log->apply();
				
			if (damage->final > 0)
			{
				string name = "Stun";
				Effect* effect = new Effect(current, battle, name, target);

				Status* status = new StatusStun(effect, target, 1);

				Event* log = new EventCauseStatus(this, Event::DEBUFF_HIT_CHANCE, status);
				log->apply();
			
				effect->applyEffect();
			}
		}
		targeters.push_back(targeter);
	}
}

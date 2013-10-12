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

string Ability::toStringSkill(Skill skill)
{
	Ability* tmp = getAbility(skill);
	string ret = tmp->getName();
	delete tmp;
	return ret;
}

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
	case MEND:
		return new Mend();
	case HEAL:
		return new Heal();
	case CLEANSE:
		return new Cleanse();
	case REGENERATION:
		return new Regeneration();
	case BLIND:
		return new Blind();
	case BARRIER:
		return new Barrier();
	case POLYMORPH:
		return new Polymorph();
	case ARCANE_BOLT:
		return new ArcaneBolt();
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
	case SLASH:
		return new Slash();
	case PROVOKE:
		return new Provoke();
	case DEMORALIZING_SHOUT:
		return new DemoralizingShout();
	case CHARGE:
		return new Charge();
	case RALLY:
		return new Rally();
	case CHALLENGE:
		return new Challenge();
	case FLURRY:
		return new Flurry();
	case POWER_ATTACK:
		return new PowerAttack();
	case LASSO:
		return new Lasso();
	case NEEDLELIGHT:
		return new Needlelight();
	case FEINT:
		return new Feint();
	case VENOMOUS_STRIKE:
		return new VenomousStrike();
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
				return unit->getBackSkill();
			case 1:
				return unit->getMidSkill();
			case 0:
				return unit->getFrontSkill();
			}
			return NO_STANDARD_SKILL;
		}
	}
	return NO_STANDARD_SKILL;
}

bool Ability::isAbleToRespond(Battle* battle, Ability* previous, Unit* prevSource, Ability* current, Unit* curSource)
{
	if (!previous->isRespondable())
		return false;

	// Currently, only Melee Attack Abilities can respond to Melee Attack Abilities
	// and Ranged Attack Abilities can respond to Ranged Attack Abilities.
	// Note: Reachability is also checked under the assumption that all melee counterattacks are a range
	// of 1 and ranged counterattacks are a range of 2. If more is desired, will have to change.
	switch (current->getAbilityType())
	{
	case ABILITY_ATTACK_MELEE:
		return previous->getAbilityType() == ABILITY_ATTACK_MELEE &&
			Targeter::canReach(curSource, prevSource, battle, 1, 1);
	case ABILITY_ATTACK_RANGE:
		return previous->getAbilityType() == ABILITY_ATTACK_RANGE &&
			Targeter::canReach(curSource, prevSource, battle, 1, 2);
	}
	return false;
}

void Ability::action(Ability* previous, Unit* current, Battle* battle) 
{
	this->source = current;
	this->battle = battle;
	Event* log = new Event(this, name);
	log->apply(battle);

	battle->addToAbilStack(this);

	current->activateOnExecuteAbility(this);
	battle->getGlobalTrigger()->activateOnExecuteAbility(this);
};

// Executed after certain triggers during the execution of a single ability,
// true means that the ability can continue as planned.
bool Ability::checkpoint(Unit* current)
{
	current->activateOnCheckpoint(this);
	battle->getGlobalTrigger()->activateOnCheckpoint(this);
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
			Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
			targeter->set(battle, 1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
				targeter->provoked = true;

				Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target);
				log->apply(battle);

				if (log->success)
				{
					Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_LOW, DAMAGE_PHYSICAL);

					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);
				}

			}
			targeters.push_back(targeter);
		}
	}
}

void Block::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Effect* effect = new Effect(current, battle, name, current);

	Status* status = new StatusBlock(effect, battle->getGlobalTrigger());
	status->setTimed(true, 1);

	Event* log = new EventCauseStatus(this, name, Event::AUTO_HIT_CHANCE, status, true);
	log->apply(battle);

	effect->applyEffect();
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

			Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);
			}
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
			Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
			targeter->set(battle, 1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
				targeter->provoked = true;

				Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target);
				log->apply(battle);

				if (log->success)
				{
					Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);
				}
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
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusTaunt(effect, targets[i], current);

		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
	}
	effect->applyEffect();
}

void BattleShout::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());

	vector<Unit*> targets = allyGroup->allyUnits();
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusBattleShout(effect, targets[i], 1);
		status->setTimed(true, 1);

		Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
		log->apply(battle);
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

			Event* log = new EventAttack(this, name, Event::RANGE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);
			}
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
			Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
			targeter->set(battle, 1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
				targeter->provoked = true;

				Event* log = new EventAttack(this, name, Event::RANGE_HIT_CHANCE, type, target);
				log->apply(battle);

				if (log->success)
				{
					Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);
				}
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
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusHaste(effect, targets[i], 1);

		Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
		log->apply(battle);
	}
	effect->applyEffect();
}

void Scope::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());

	vector<Unit*> targets = allyGroup->allyUnits();

	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusScope(effect, targets[i], 1);

		Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
		log->apply(battle);
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
		Targeter* targeter = new Targeter(this, targets, TARGET_ALLIES, TARGET_RANDOM, 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Effect* effect = new Effect(current, battle, name, current);

			Status* status = new StatusTangleTrap(effect, target);

			Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
			log->apply(battle);

			effect->applyEffect();
		}
		targeters.push_back(targeter);
	}
}

void Mend::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	if (previous != NULL)
	{
		if (previous->getSource() != NULL)
		{
			Unit* target = current;

			Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_LOW, DAMAGE_HEALING);

			Event* log = new EventCauseDamage(this, name, Event::HEAL_HIT_CHANCE, damage);
			log->apply(battle);
		}
	}
	else
	{
		Group* allyGroup = battle->getAllyGroup(current->getGrid());

		vector<Unit*> targets = allyGroup->allyUnits();

		if (targets.size() > 0)
		{
			Targeter* targeter = new Targeter(this, targets, TARGET_ALLIES, TARGET_HEALING, 1);
			targeter->set(battle, 1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
				targeter->provoked = true;

				Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_LOW, DAMAGE_HEALING);

				Event* log = new EventCauseDamage(this, name, Event::HEAL_HIT_CHANCE, damage);
				log->apply(battle);
			}
			targeters.push_back(targeter);
		}
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

			Event* log = new EventCauseDamage(this, name, Event::HEAL_HIT_CHANCE, damage);
			log->apply(battle);
		}
	}
	else
	{
		Group* allyGroup = battle->getAllyGroup(current->getGrid());

		vector<Unit*> targets = allyGroup->allyUnits();

		if (targets.size() > 0)
		{
			Targeter* targeter = new Targeter(this, targets, TARGET_ALLIES, TARGET_HEALING, 1);
			targeter->set(battle, 1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
				targeter->provoked = true;

				Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_HEALING);

				Event* log = new EventCauseDamage(this, name, Event::HEAL_HIT_CHANCE, damage);
				log->apply(battle);
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
		Targeter* targeter = new Targeter(this, targets, TARGET_ALLIES, TARGET_MOST_DEBUFFS, 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Event* log = new EventRemoveStatus(this, name, Event::CLEANSE_HIT_CHANCE, target, DEBUFF);
			log->apply(battle);
		}
		targeters.push_back(targeter);
	}
}

void Regeneration::action(Ability* previous, Unit* current, Battle* battle)
{   
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	if (previous != NULL)
	{
		Unit* target = current;

		Effect* effect = new Effect(current, battle, name, target);

		Status* status = new StatusRegeneration(effect, target, 1);

		Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
		log->apply(battle);

		effect->applyEffect();
	}
	else
	{
		Group* allyGroup = battle->getAllyGroup(current->getGrid());

		vector<Unit*> targets = allyGroup->allyUnits();

		if (targets.size() > 0)
		{
			Targeter* targeter = new Targeter(this, targets, TARGET_ALLIES, TARGET_HEALING, 1);
			targeter->set(battle, 1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
				targeter->provoked = true;

				Effect* effect = new Effect(current, battle, name, target);

				Status* status = new StatusRegeneration(effect, target, 1);

				Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
				log->apply(battle);

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

			Effect* effect = new Effect(current, battle, name, target);

			Status* status = new StatusBlind(effect, target, 1);

			Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
			log->apply(battle);

			effect->applyEffect();
		}
	}
	else
	{
		Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

		vector<Unit*> targets = enemyGroup->allyUnits();

		if (targets.size() > 0)
		{
			Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, TARGET_RANDOM, 1);
			targeter->set(battle, 1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
				targeter->provoked = true;

				Effect* effect = new Effect(current, battle, name, target);

				Status* status = new StatusBlind(effect, target, 1);

				Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
				log->apply(battle);

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

	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusBarrier(effect, targets[i], 1);

		Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
		log->apply(battle);
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
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, TARGET_RANDOM, 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Effect* effect = new Effect(current, battle, name, target);

			Status* status = new StatusPolymorph(effect, target, 1);

			Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
			log->apply(battle);

			effect->applyEffect();
		}
		targeters.push_back(targeter);
	}
}

void ArcaneBolt::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	if (previous != NULL)
	{
		if (previous->getSource() != NULL && previous->getSource()->isAvailable())
		{
			Unit* target = previous->getSource();

			Event* log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_ARCANE);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);
			}
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
			Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
			targeter->set(battle, 1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
				targeter->provoked = true;

				Event* log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, target);
				log->apply(battle);

				if (log->success)
				{
					Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_ARCANE);

					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);
				}
			}
			targeters.push_back(targeter);
		}
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
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Event* log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_FIRE);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final > 0)
				{
					Effect* effect = new Effect(current, battle, name, target);

					Status* status = new StatusBurn(effect, target, 1);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
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
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Event* log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_WATER);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final > 0)
				{
					GridPoint pos = GridPoint(target->getGridX(), target->getGridY() + 1);
					bool possibleFlee = false;
					if (pos.y > 2) {
						pos.y = 2;
						possibleFlee = true;
					}

					Event* log = new EventReposition(this, name, Event::KNOCKBACK_HIT_CHANCE, target, pos);
					log->apply(battle);

					if (log->success && possibleFlee)
					{
						Effect* effect = new Effect(current, battle, name, target);

						Status* status = new StatusFlee(effect, target, 1);

						Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
						log->apply(battle);

						effect->applyEffect();
					}
				}
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
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Event* log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_EARTH);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final > 0)
				{
					Effect* effect = new Effect(current, battle, name, target);

					Status* status = new StatusPoison(effect, target, 1);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
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
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Event* log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_ICE);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final > 0)
				{
					Effect* effect = new Effect(current, battle, name, target);

					Status* status = new StatusChill(effect, target, 1);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
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
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Event* log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_LIGHTNING);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final > 0)
				{
					Effect* effect = new Effect(current, battle, name, target);

					Status* status = new StatusStun(effect, target, 1);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
			}
		}
		targeters.push_back(targeter);
	}
}

void Slash::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	if (checkpoint(current)) return;

	int rowRange = 1;
	vector<Unit*> targets = allyGroup->enemyUnitsInRowFurthestInFront(enemyGroup, rowRange);

	if (targets.size() > 0)
	{
		int maxTargets = 3;
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), maxTargets);
		targeter->set(battle, maxTargets);

		if (checkpoint(current)) return;

		for (int i = 0; i < targeter->chosen.size(); ++i) {
			Unit* target = targeter->chosen[i];
			targeter->provoked = true;

			Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);
			}
		}
		targeters.push_back(targeter);

	}
}

void Provoke::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	int rowRange = 1;
	int initialColumnRange = 1;
	vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);

	if (targets.size() > 0)
	{
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_LOW, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);


				if (damage->final > 0)
				{
					Effect* effect = new Effect(current, battle, name, current);

					Status* status = new StatusTaunt(effect, target, current);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
			}
		}
		targeters.push_back(targeter);
	}
}

void DemoralizingShout::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	vector<Unit*> targets = enemyGroup->allyUnits();
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusDemoralize(effect, targets[i], 1);

		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
	}
	effect->applyEffect();
}

void Charge::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	int rowRange = 1;
	int initialColumnRange = 1;
	vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);

	if (targets.size() > 0)
	{
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final > 0)
				{
					GridPoint pos = GridPoint(target->getGridX(), target->getGridY() + 1);
					if (pos.y > 2)
						pos.y = 2;
					Unit* behindUnit = enemyGroup->getUnitAt(pos);

					Event* log = new EventReposition(this, name, Event::KNOCKBACK_HIT_CHANCE, target, pos);
					log->apply(battle);

					if (log->success && behindUnit != NULL && behindUnit != target)
					{
						Damage* damage = new Damage(this, behindUnit, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

						Event* log = new EventCauseDamage(this, name, Event::MELEE_HIT_CHANCE, damage);
						log->apply(battle);
					}
				}
			}
		}
		targeters.push_back(targeter);
	}
}

void Rally::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());

	vector<Unit*> targets = allyGroup->getAllUnits();

	Effect* effect = new Effect(current, battle, name, battle->getGlobalTrigger());

	Status* status = new StatusRally(effect, battle->getGlobalTrigger());

	Event* log = new EventCauseStatus(this, name, Event::AUTO_HIT_CHANCE, status, true);
	log->apply(battle);

	effect->applyEffect();

	for (int i = 0; i < targets.size(); ++i)
	{
		vector<StatusGroup*> sgroups = targets[i]->getCurrentStatus();
		for (int j = 0; j < sgroups.size(); ++j)
		{
			vector<Status*> instances = sgroups[j]->getInstances();
			for (int k = 0; k < instances.size(); ++k)
			{
				if (instances[k]->getCategory() == STATUS_CC)
				{
					int t = instances[k]->getTimer();
					if (t > 0) t--;
					instances[k]->setTimer(t);
					if (instances[k]->hasExpired())
						instances[k]->onKill();
				}
			}
		}
	}
}

void Challenge::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	int rowRange = 3;
	int initialColumnRange = 1;
	vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);

	if (targets.size() > 0)
	{
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, TARGET_RANDOM, 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Effect* effect = new Effect(current, battle, name, current);

			Status* status = new StatusTaunt(effect, target, current);

			Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
			log->apply(battle);

			if (log->success)
			{
				Status* status = new StatusDemoralize(effect, target, 1);

				Event* log = new EventCauseStatus(this, name, Event::AUTO_HIT_CHANCE, status);
				log->apply(battle);
			}

			effect->applyEffect();
		}
		targeters.push_back(targeter);
	}
}

void Flurry::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	double r = 100;
	while (r >= 50)
	{
		if (checkpoint(current)) return;

		int rowRange = 1;
		int initialColumnRange = 1;
		vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);

		if (targets.size() > 0)
		{
			Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
			targeter->set(battle, 1);

			if (checkpoint(current)) return;

			if (targeter->chosen.size() > 0) {
				Unit* target = targeter->chosen[0];
				targeter->provoked = true;

				Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target);
				log->apply(battle);

				if (log->success)
				{
					Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_LOW, DAMAGE_PHYSICAL);

					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);


					if (log->success)
						r = rand() % 100;
					else
						r = 0;
				}
			}
			targeters.push_back(targeter);
		}
	}
}

void PowerAttack::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	int rowRange = 1;
	int initialColumnRange = 1;
	vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);

	if (targets.size() > 0)
	{
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_HIGH, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);
			}
		}
		targeters.push_back(targeter);
	}
}

void Lasso::action(Ability* previous, Unit* current, Battle* battle)
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
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_LOW, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final > 0)
				{
					GridPoint pos = GridPoint(target->getGridX(), target->getGridY() - 1);
					if (pos.y < 0)
						pos.y = 0;

					Event* log = new EventReposition(this, name, Event::KNOCKBACK_HIT_CHANCE, target, pos);
					log->apply(battle);
				}
			}
		}
		targeters.push_back(targeter);
	}
}

void Needlelight::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	int rowRange = 1;
	int initialColumnRange = 1;
	vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);

	if (targets.size() > 0)
	{
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL, true);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);
			}
		}
		targeters.push_back(targeter);
	}
}

void Feint::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;
	
	if (previous != NULL)
	{
		if (previous->getSource() != NULL && previous->getSource()->isAvailable())
		{
			Unit* target = previous->getSource();

			Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, ABILITY_ATTACK_MELEE, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_LOW, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);
				
				if (damage->final > 0)
				{
					if (previous->isInterruptible())
						previous->setCancelled(true);
				}
			}
		}
	}
	else
	{
		Effect* effect = new Effect(current, battle, name, current);

		Status* status = new StatusFeint(effect, battle->getGlobalTrigger());
		status->setTimed(true, 1);

		Event* log = new EventCauseStatus(this, name, Event::AUTO_HIT_CHANCE, status, true);
		log->apply(battle);

		effect->applyEffect();
	}
}

void VenomousStrike::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	int rowRange = 1;
	int initialColumnRange = 1;
	vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);

	if (targets.size() > 0)
	{
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final > 0)
				{
					Effect* effect = new Effect(current, battle, name, target);

					Status* status = new StatusPoison(effect, target, 2);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
			}
		}
		targeters.push_back(targeter);
	}
}

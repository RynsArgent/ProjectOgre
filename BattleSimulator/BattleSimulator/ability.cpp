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

bool compareCurrentLessY(Unit* lhs, Unit* rhs) {
    return lhs->getGridY() < rhs->getGridY();
}

bool compareCurrentMoreY(Unit* lhs, Unit* rhs) {
    return lhs->getGridY() > rhs->getGridY();
}

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
	case DISPEL:
		return new Dispel();
	case REGENERATION:
		return new Regeneration();
	case BLIND:
		return new Blind();
	case FLASH:
		return new Flash();
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
	case PIERCETHROUGH:
		return new Piercethrough();
	case FEINT:
		return new Feint();
	case VENOMOUS_STRIKE:
		return new VenomousStrike();
	case CRIPPLING_SHOT:
		return new CripplingShot();
	case CONFUSE_TRAP:
		return new ConfuseTrap();
	case HUNTERS_MARK:
		return new HuntersMark();
	case TRANQUILIZING_SHOT:
		return new TranquilizingShot();
	case CHARM_TRAP:
		return new CharmTrap();
	case QUICK_NOCK:
		return new QuickNock();
	case RAPID_SHOT:
		return new RapidShot();
	case VOLLEY:
		return new Volley();
	case SNIPE:
		return new Snipe();
	case REFORMATION:
		return new Reformation();
	case LULLABY:
		return new Lullaby();
	case BALLAD_OF_HEROES:
		return new BalladOfHeroes();
	case REQUIEM_OF_WAR:
		return new RequiemOfWar();
	case BANISH:
		return new Banish();
	case GREATER_HEAL:
		return new GreaterHeal();
	case RAISE:
		return new Raise();
	case LIGHTNING_KICKS:
		return new LightningKicks();
	case HURRICANE_KICK:
		return new HurricaneKick();
	case ROUNDHOUSE:
		return new Roundhouse();
	case LEG_SWEEP:
		return new LegSweep();
	case BLINK:
        return new Blink();
    case ARCANE_MISSIL:
        return new ArcaneMissil();
    case WATERBLAST:
        return new Waterblast();
    case FLAMESTRIKE:
        return new Flamestrike();
    case ICICLE:
        return new Icicle();
    case ELECTRIC_CURTAIN:
        return new ElectricCurtain();
    case ROCKSLIDE:
        return new Rockslide();
    case FASCINATION:
        return new Fascination();
    case STUN_WAVE:
        return new StunWave();
    case NOXIOUS_SPORES:
        return new NoxiousSpores();
    case PARALYTIC_CLOUD:
        return new ParalyticCloud();
    case SLEEP_MIASMA:
        return new SleepMiasma();
    case CHILLING_FOG:
        return new ChillingFog();
    case DISORIENTING_MIST:
        return new DisorientingMist();
    case ENERVATE:
        return new Enervate();
    case NATURES_GRASP:
        return new NaturesGrasp();
    case NATURES_EMBRACE:
        return new NaturesEmbrace();
	case MANA_TREE:
		return new ManaTree();
	case TRANQUILITY:
		return new Tranquility();
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
		}
	}
}

void Block::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	if (previous)
	{
		if (previous->getSource() != NULL && previous->getSource()->isAvailable())
		{
			// Guaranteed that this ability responds with an EventAttack at the top of the stack
			EventAttack* evt = static_cast<EventAttack*>(previous->getEvents()[previous->getEvents().size() - 1]);
			
			Unit* target = evt->target;
			
			Effect* effect = new Effect(current, battle, name, target);

			Status* status = new StatusShield(effect, target, Damage::getDamageValue(DAMAGE_MASSIVE, current->getCurrentPhysicalAttack()));

			Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
			log->apply(battle);

			effect->applyEffect();
		}
	}
	else
	{
		Effect* effect = new Effect(current, battle, name, current);

		Status* status = new StatusBlock(effect, battle->getGlobalTrigger());

		Event* log = new EventCauseStatus(this, name, Event::AUTO_HIT_CHANCE, status, true);
		log->apply(battle);

		effect->applyEffect();
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
		}
	}
}

void Taunt::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);
    
	if (checkpoint(current)) return;
    
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
    
	vector<Unit*> targets = enemyGroup->allyUnits();
	for (int i = 0; i < targets.size(); ++i)
	{
        Effect* effect = new Effect(current, battle, name, targets[i]);
        
		Status* status = new StatusProvoke(effect, targets[i], current);
        
		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
        
        effect->applyEffect();
	}
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

			vector<StatusGroup*> statuslist = target->getCurrentStatus();
			for (int i = 0; i < statuslist.size(); ++i)
			{
				if (statuslist[i]->getBenefit() == DEBUFF && statuslist[i]->isDispellable())
				{
					Event* log = new EventRemoveStatus(this, name, Event::CLEANSE_HIT_CHANCE, target, statuslist[i]);
					log->apply(battle);
				}
			}
		}
	}
}

void Dispel::action(Ability* previous, Unit* current, Battle* battle)
{   
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	vector<Unit*> targets = enemyGroup->allyUnits();

	if (targets.size() > 0)
	{
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, TARGET_MOST_BUFFS, 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;

			vector<StatusGroup*> statuslist = target->getCurrentStatus();
			for (int i = 0; i < statuslist.size(); ++i)
			{
				if (statuslist[i]->getBenefit() == BUFF && statuslist[i]->isDispellable())
				{
					Event* log = new EventRemoveStatus(this, name, Event::DISPEL_HIT_CHANCE, target, statuslist[i]);
					log->apply(battle);
				}
			}
		}
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
		}
	}
}

void Flash::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	vector<Unit*> targets = enemyGroup->allyUnits();
	for (int i = 0; i < targets.size(); ++i)
	{
        Effect* effect = new Effect(current, battle, name, targets[i]);
        
		Status* status = new StatusBlind(effect, targets[i], 1);

		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
        
        effect->applyEffect();
	}
}

void Barrier::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	if (previous)
	{
		if (previous->getSource() != NULL && previous->getSource()->isAvailable())
		{
			// Guaranteed that this ability responds with an EventAttack at the top of the stack
			EventAttack* evt = static_cast<EventAttack*>(previous->getEvents()[previous->getEvents().size() - 1]);
			
			Unit* target = evt->target;
			
			Effect* effect = new Effect(current, battle, name, target);

			Status* status = new StatusShell(effect, target, Damage::getDamageValue(DAMAGE_MASSIVE, current->getCurrentMagicAttack()));

			Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
			log->apply(battle);

			effect->applyEffect();
		}
	}
	else
	{
		Effect* effect = new Effect(current, battle, name, current);

		Status* status = new StatusBarrier(effect, battle->getGlobalTrigger());

		Event* log = new EventCauseStatus(this, name, Event::AUTO_HIT_CHANCE, status, true);
		log->apply(battle);

		effect->applyEffect();
	}
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

					Event* log = new EventReposition(this, name, Event::REPOSITION_HIT_CHANCE, target, pos);
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

					Status* status = new StatusParalyze(effect, target, 1);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
			}
		}
	}
}

void Slash::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	if (checkpoint(current)) return;

	int rowRange = 1;
	vector<Unit*> targets = Targeter::selectRowTargets(current, battle, allyGroup, enemyGroup, rowRange, false);

	if (targets.size() > 0)
	{
		for (int i = 0; i < targets.size(); ++i) {
			Unit* target = targets[i];

			Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target, true);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);
			}
		}
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
					Effect* effect = new Effect(current, battle, name, target);

					Status* status = new StatusProvoke(effect, target, current);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
			}
		}
	}
}

void DemoralizingShout::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	vector<Unit*> targets = enemyGroup->allyUnits();
	for (int i = 0; i < targets.size(); ++i)
	{
        Effect* effect = new Effect(current, battle, name, targets[i]);
        
		Status* status = new StatusDemoralize(effect, targets[i], 1);

		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
        
        effect->applyEffect();
	}
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

					Event* log = new EventReposition(this, name, Event::REPOSITION_HIT_CHANCE, target, pos);
					log->apply(battle);

					if (log->success && behindUnit != NULL && behindUnit != target)
					{
						Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target, true);
						log->apply(battle);

						if (log->success)
						{
							Damage* damage = new Damage(this, behindUnit, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

							Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
							log->apply(battle);
						}
					}
				}
			}
		}
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
		if (targets[i]->isDead())
			continue;

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

			Effect* effect = new Effect(current, battle, name, target);

			Status* status = new StatusProvoke(effect, target, current);

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
	}
}

void Flurry::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	double r = 100;
	while (r >= 60)
	{
		r = 0;
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
						r = rand() % 100 + 1;
				}
			}
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

					Event* log = new EventReposition(this, name, Event::REPOSITION_HIT_CHANCE, target, pos);
					log->apply(battle);
				}
			}
		}
	}
}

void Piercethrough::action(Ability* previous, Unit* current, Battle* battle)
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
	}
}

void CripplingShot::action(Ability* previous, Unit* current, Battle* battle)
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

			Event* log = new EventAttack(this, name, Event::RANGE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final > 0)
				{
					Effect* effect = new Effect(current, battle, name, target);

					Status* status = new StatusChill(effect, target, 2);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
			}
		}
	}
}

void ConfuseTrap::action(Ability* previous, Unit* current, Battle* battle)
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

			Status* status = new StatusConfuseTrap(effect, target);

			Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
			log->apply(battle);

			effect->applyEffect();
		}
	}
}

void HuntersMark::action(Ability* previous, Unit* current, Battle* battle)
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

			Event* log = new EventAttack(this, name, Event::RANGE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);


				if (damage->final > 0)
				{
					Effect* effect = new Effect(current, battle, name, current);

					Status* status = new StatusMarked(effect, target, current);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
			}
		}
	}
}

void TranquilizingShot::action(Ability* previous, Unit* current, Battle* battle)
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

			Event* log = new EventAttack(this, name, Event::RANGE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final > 0)
				{
					Effect* effect = new Effect(current, battle, name, target);

					Status* status = new StatusSleep(effect, target, 1);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
			}
		}
	}
}

void CharmTrap::action(Ability* previous, Unit* current, Battle* battle)
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

			Status* status = new StatusCharmTrap(effect, target);

			Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
			log->apply(battle);

			effect->applyEffect();
		}
	}
}

void QuickNock::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	if (previous != NULL)
	{
		if (previous->getSource() != NULL && previous->getSource()->isAvailable())
		{
			Unit* target = previous->getSource();

			Event* log = new EventAttack(this, name, Event::RANGE_HIT_CHANCE, ABILITY_ATTACK_RANGE, target);
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

		Status* status = new StatusQuickNock(effect, battle->getGlobalTrigger());

		Event* log = new EventCauseStatus(this, name, Event::AUTO_HIT_CHANCE, status, true);
		log->apply(battle);

		effect->applyEffect();
	}
}

void RapidShot::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	int numTimes = rand() % 3 + 1;
	for (int i = 0; i < numTimes; ++i)
	{
		if (checkpoint(current)) return;

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
					Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_LOW, DAMAGE_PHYSICAL);

					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);
				}

			}
		}
	}
}

void Volley::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	int rowRange = 3;
	int initialColumnRange = 2;
	vector<Unit*> targets = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, initialColumnRange, rowRange);

	if (targets.size() > 0)
	{
		Targeter* targeter = new Targeter(this, targets, TARGET_ENEMIES, allyGroup->getTargetOrder(), 1);
		targeter->set(battle, 1);

		if (checkpoint(current)) return;

		if (targeter->chosen.size() > 0) {
			Unit* target = targeter->chosen[0];
			targeter->provoked = true;
			
			vector<Unit*> sides = enemyGroup->unitsAdjacentTo(target->getGridX(), target->getGridY());
            sides.push_back(target);
            
			for (int i = 0; i < sides.size(); ++i)
			{
				Unit* side = sides[i];
	
				Event* log = NULL;
				if (side == target) log = new EventAttack(this, name, Event::RANGE_HIT_CHANCE, type, side, false);
				else log = new EventAttack(this, name, Event::RANGE_HIT_CHANCE, type, side, true);
				log->apply(battle);

				if (log->success)
				{
					Damage* damage = new Damage(this, side, current->getCurrentPhysicalAttack(), DAMAGE_LOW, DAMAGE_PHYSICAL);
					
					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);
				}
			}
		}
	}
}

void Snipe::action(Ability* previous, Unit* current, Battle* battle)
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

			Event* log = new EventAttack(this, name, Event::RANGE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_HIGH, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);
			}
		}
	}
}

void Reformation::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());

	vector<Unit*> targets = allyGroup->getUnits();
	for (int i = 0; i < targets.size(); ++i)
	{
		if (targets[i]->isDead())
			continue;

		vector<StatusGroup*> sgroups = targets[i]->getCurrentStatus();
		for (int j = 0; j < sgroups.size(); ++j)
		{
			if (sgroups[j]->getSubname() == "Flee")
			{
				vector<Status*> instances = sgroups[j]->getInstances();
				for (int k = 0; k < instances.size(); ++k)
					instances[k]->onKill();
			}
		}
	}

	allyGroup->resetGrid();

	vector<Unit*> summonedList = allyGroup->getSummoned();
	vector<GridPoint> positions = allyGroup->getAvailablePoints();

	int s;
	for (s = 0; s < summonedList.size() && positions.size() > 0; ++s)
	{
		int r = rand() % positions.size();
		GridPoint pos = positions[r];
		positions[r] = positions[positions.size() - 1];
		positions.pop_back();

		allyGroup->setUnitAt(pos, summonedList[s]);
		summonedList[s]->setOnGrid(pos.x, pos.y);
	}
	for (; s < summonedList.size(); ++s)
	{
		summonedList[s]->setOnGrid(-1, -1);
	}
}

void Lullaby::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	vector<Unit*> targets = enemyGroup->allyUnits();
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusSleep(effect, targets[i], 1);

		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
	}
	effect->applyEffect();
}

void BalladOfHeroes::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());

	vector<Unit*> targets = allyGroup->getAllUnits();

	for (int i = 0; i < targets.size(); ++i)
	{
		if (targets[i]->isDead()) 
			continue;

		Damage* damage = new Damage(this, targets[i], current->getCurrentMagicAttack(), DAMAGE_MINOR, DAMAGE_HEALING);

		Event* log = new EventCauseDamage(this, name, Event::HEAL_HIT_CHANCE, damage);
		log->apply(battle);

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

void RequiemOfWar::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());

	vector<Unit*> targets = allyGroup->allyUnits();
	Effect* effect = new Effect(current, battle, name, current);
	for (int i = 0; i < targets.size(); ++i)
	{
		Status* status = new StatusDetermination(effect, targets[i], 1);

		Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
		log->apply(battle);
	}
	effect->applyEffect();
}

void Banish::action(Ability* previous, Unit* current, Battle* battle)
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
				int damageAmount = current->getCurrentMagicAttack();
				if (target->isSummoned())
					damageAmount *= 2;
				Damage* damage = new Damage(this, target, damageAmount, DAMAGE_MEDIUM, DAMAGE_HOLY);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final > 0 && target->isSummoned())
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
}

void GreaterHeal::action(Ability* previous, Unit* current, Battle* battle)
{   
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

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
			
			vector<Unit*> sides = allyGroup->unitsAdjacentTo(target->getGridX(), target->getGridY());
			sides.push_back(target);

			for (int i = 0; i < sides.size(); ++i)
			{
				Unit* side = sides[i];

				Damage* damage = new Damage(this, side, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_HEALING);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);
			}
		}
	}
}

void Raise::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());

	vector<Unit*> dead = allyGroup->getDead();
	
	vector<GridPoint> positions = allyGroup->getAvailablePoints();
	if (dead.size() > 0 && positions.size() > 0)
	{
		Unit* target = dead[0];

		GridPoint pos = GridPoint(target->getFormX(), target->getFormY());
		if (allyGroup->hasUnitAt(pos)) {
			pos = positions[rand() % positions.size()];
		}
		
		Event* log = new EventRaise(this, name, Event::RAISE_HIT_CHANCE, target, pos);
		log->apply(battle);
	}
}

void LightningKicks::action(Ability* previous, Unit* current, Battle* battle)
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
			double r = 100;
			while (r >= 60)
			{
				r = 0;
				
				if (checkpoint(current)) return;

				Event* log = new EventAttack(this, name, Event::MELEE_HIT_CHANCE, type, target);
				log->apply(battle);

				if (log->success)
				{
					Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_LOW, DAMAGE_PHYSICAL);

					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);

					if (log->success)
						r = rand() % 100 + 1;
				}
			}
		}
	}
}

void HurricaneKick::action(Ability* previous, Unit* current, Battle* battle)
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
			
			Event* log = new EventAttack(this, name, Event::RANGE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final > 0)
				{
					Effect* effect = new Effect(current, battle, name, target);

					Status* status = new StatusConfusion(effect, target, 1);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);
					
					effect->applyEffect();
				}
				
				vector<Unit*> sides = enemyGroup->unitsAdjacentTo(target->getGridX(), target->getGridY());
				for (int i = 0; i < sides.size(); ++i)
				{
					Unit* side = sides[i];

					Event* log = new EventAttack(this, name, Event::RANGE_HIT_CHANCE, type, side, true);
					log->apply(battle);

					if (log->success)
					{
						Damage* damage = new Damage(this, side, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

						Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
						log->apply(battle);

						if (damage->final > 0)
						{
							Effect* effect = new Effect(current, battle, name, side);

							Status* status = new StatusConfusion(effect, side, 1);

							Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
							log->apply(battle);
							
							effect->applyEffect();
						}
					}
				}
			}
		}
	}
}

void Roundhouse::action(Ability* previous, Unit* current, Battle* battle)
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

			Event* log = new EventAttack(this, name, Event::RANGE_HIT_CHANCE, type, target);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_MEDIUM, DAMAGE_PHYSICAL);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);
				
				vector<GridPoint> positions = enemyGroup->getAvailableAdjacentPoints(target->getGridX(), target->getGridY());
				if (damage->final > 0 && positions.size() > 0)
				{
					GridPoint pos = positions[rand() % positions.size()];
					Event* log = new EventReposition(this, name, Event::REPOSITION_HIT_CHANCE, target, pos);
					log->apply(battle);
				}
			}
		}
	}
}

void LegSweep::action(Ability* previous, Unit* current, Battle* battle)
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
				Damage* damage = new Damage(this, target, current->getCurrentPhysicalAttack(), DAMAGE_LOW, DAMAGE_PHYSICAL);

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
	}
}

void Blink::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	if (previous != NULL)
	{
		if (previous->getSource() != NULL && previous->getSource()->isAvailable())
		{
			// Guaranteed that this ability responds with an EventAttack at the top of the stack
			EventAttack* evt = static_cast<EventAttack*>(previous->getEvents()[previous->getEvents().size() - 1]);
		
			Group* allyGroup = battle->getAllyGroup(current->getGrid());
			
			vector<GridPoint> positions = allyGroup->getAvailablePoints();
			if (positions.size() > 0)
			{
				GridPoint pos = positions[rand() % positions.size()];
				
				Event* log = new EventReposition(this, name, Event::REPOSITION_HIT_CHANCE, current, pos);
				log->apply(battle);

				if (log->success)
					evt->chance = -1;
			}
		}
	}
	else
	{
		Effect* effect = new Effect(current, battle, name, current);

		Status* status = new StatusBlink(effect, current);

		Event* log = new EventCauseStatus(this, name, Event::AUTO_HIT_CHANCE, status, true);
		log->apply(battle);

		effect->applyEffect();
	}
}

void ArcaneMissil::action(Ability* previous, Unit* current, Battle* battle)
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
                
            Event* log = new EventAttack(this, name, Event::AUTO_HIT_CHANCE, type, target);
            log->apply(battle);
                
            if (log->success)
            {
                Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_HIGH, DAMAGE_ARCANE);
                    
                Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
                log->apply(battle);
            }
        }
    }
}

void Waterblast::action(Ability* previous, Unit* current, Battle* battle)
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
			
			vector<Unit*> sides = enemyGroup->unitsAdjacentTo(target->getGridX(), target->getGridY());
            sides.push_back(target);
            sort(sides.begin(), sides.end(), compareCurrentMoreY);
            
			for (int i = 0; i < sides.size(); ++i)
			{
				Unit* side = sides[i];
                
				Event* log = NULL;
				if (side == target) log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, side, false);
				else log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, side, true);
				log->apply(battle);
                
				if (log->success)
				{
					Damage* damage = new Damage(this, side, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_WATER);
					
					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);
                    
                    if (damage->final > 0)
                    {
                        GridPoint pos = GridPoint(side->getGridX(), side->getGridY() + 1);
                        bool possibleFlee = false;
                        if (pos.y > 2) {
                            pos.y = 2;
                            possibleFlee = true;
                        }
                        
                        Event* log = new EventReposition(this, name, Event::REPOSITION_HIT_CHANCE, side, pos);
                        log->apply(battle);
                        
                        if (log->success && possibleFlee)
                        {
                            Effect* effect = new Effect(current, battle, name, side);
                            
                            Status* status = new StatusFlee(effect, side, 1);
                            
                            Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
                            log->apply(battle);
                            
                            effect->applyEffect();
                        }
                    }
				}
			}
		}
	}
}

void Flamestrike::action(Ability* previous, Unit* current, Battle* battle)
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
			
			vector<Unit*> sides = enemyGroup->unitsAdjacentTo(target->getGridX(), target->getGridY());
            sides.push_back(target);
            
			for (int i = 0; i < sides.size(); ++i)
			{
				Unit* side = sides[i];
                
				Event* log = NULL;
				if (side == target) log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, side, false);
				else log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, side, true);
				log->apply(battle);
                
				if (log->success)
				{
					Damage* damage = new Damage(this, side, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_FIRE);
					
					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);
                    
                    if (damage->final > 0)
                    {
                        Effect* effect = new Effect(current, battle, name, side);
                        
                        Status* status = new StatusBurn(effect, side, 1);
                        
                        Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
                        log->apply(battle);
                        
                        effect->applyEffect();
                    }
				}
			}
		}
	}
}

void Icicle::action(Ability* previous, Unit* current, Battle* battle)
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
			
			vector<Unit*> sides = enemyGroup->unitsAdjacentTo(target->getGridX(), target->getGridY());
            sides.push_back(target);
            
			for (int i = 0; i < sides.size(); ++i)
			{
				Unit* side = sides[i];
                
				Event* log = NULL;
				if (side == target) log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, side, false);
				else log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, side, true);
				log->apply(battle);
                
				if (log->success)
				{
					Damage* damage = new Damage(this, side, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_ICE);
					
					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);
                    
                    if (damage->final > 0)
                    {
                        Effect* effect = new Effect(current, battle, name, side);
                        
                        Status* status = new StatusChill(effect, side, 1);
                        
                        Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
                        log->apply(battle);
                        
                        effect->applyEffect();
                    }
				}
			}
		}
	}
}

void ElectricCurtain::action(Ability* previous, Unit* current, Battle* battle)
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
			
			vector<Unit*> sides = enemyGroup->unitsAdjacentTo(target->getGridX(), target->getGridY());
            sides.push_back(target);
            
			for (int i = 0; i < sides.size(); ++i)
			{
				Unit* side = sides[i];
                
				Event* log = NULL;
				if (side == target) log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, side, false);
				else log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, side, true);
				log->apply(battle);
                
				if (log->success)
				{
					Damage* damage = new Damage(this, side, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_LIGHTNING);
					
					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);
                    
                    if (damage->final > 0)
                    {
                        Effect* effect = new Effect(current, battle, name, side);
                        
                        Status* status = new StatusParalyze(effect, side, 1);
                        
                        Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
                        log->apply(battle);
                        
                        effect->applyEffect();
                    }
				}
			}
		}
	}
}

void Rockslide::action(Ability* previous, Unit* current, Battle* battle)
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
			
			vector<Unit*> sides = enemyGroup->unitsAdjacentTo(target->getGridX(), target->getGridY());
            sides.push_back(target);
            
			for (int i = 0; i < sides.size(); ++i)
			{
				Unit* side = sides[i];
                
				Event* log = NULL;
				if (side == target) log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, side, false);
				else log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, side, true);
				log->apply(battle);
                
				if (log->success)
				{
					Damage* damage = new Damage(this, side, current->getCurrentMagicAttack(), DAMAGE_MEDIUM, DAMAGE_EARTH);
					
					Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
					log->apply(battle);
                    
                    if (damage->final > 0)
                    {
                        Effect* effect = new Effect(current, battle, name, side);
                        
                        Status* status = new StatusConfusion(effect, side, 1);
                        
                        Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
                        log->apply(battle);
                        
                        effect->applyEffect();
                    }
				}
			}
		}
	}
}

void Fascination::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);
    
	if (checkpoint(current)) return;
    
    Effect* effect = new Effect(current, battle, name, current);
            
    Status* status = new StatusFascination(effect, current);
            
    Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
    log->apply(battle);
            
    effect->applyEffect();
}

void StunWave::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);
    
	if (checkpoint(current)) return;
    
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
    
	vector<Unit*> targets = enemyGroup->allyUnits();
	for (int i = 0; i < targets.size(); ++i)
	{
        Effect* effect = new Effect(current, battle, name, targets[i]);
        
		Status* status = new StatusStun(effect, targets[i], 1);
        
		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
        
        effect->applyEffect();
	}
}

void NoxiousSpores::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);
    
	if (checkpoint(current)) return;
    
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
    
	vector<Unit*> targets = enemyGroup->allyUnits();
	for (int i = 0; i < targets.size(); ++i)
	{
        Effect* effect = new Effect(current, battle, name, targets[i]);
        
		Status* status = new StatusPoison(effect, targets[i], 1);
        
		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
        
        effect->applyEffect();
	}
}

void ParalyticCloud::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);
    
	if (checkpoint(current)) return;
    
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
    
	vector<Unit*> targets = enemyGroup->allyUnits();
	for (int i = 0; i < targets.size(); ++i)
	{
        Effect* effect = new Effect(current, battle, name, targets[i]);
        
		Status* status = new StatusParalyze(effect, targets[i], 1);
        
		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
        
        effect->applyEffect();
	}
}

void SleepMiasma::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);
    
	if (checkpoint(current)) return;
    
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
    
	vector<Unit*> targets = enemyGroup->allyUnits();
	for (int i = 0; i < targets.size(); ++i)
	{
        Effect* effect = new Effect(current, battle, name, targets[i]);
        
		Status* status = new StatusSleep(effect, targets[i], 1);
        
		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
        
        effect->applyEffect();
	}
}

void ChillingFog::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);
    
	if (checkpoint(current)) return;
    
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
    
	vector<Unit*> targets = enemyGroup->allyUnits();
	for (int i = 0; i < targets.size(); ++i)
	{
        Effect* effect = new Effect(current, battle, name, targets[i]);
        
		Status* status = new StatusChill(effect, targets[i], 1);
        
		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
        
        effect->applyEffect();
	}
}

void DisorientingMist::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);
    
	if (checkpoint(current)) return;
    
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
    
	vector<Unit*> targets = enemyGroup->allyUnits();
	for (int i = 0; i < targets.size(); ++i)
	{
        Effect* effect = new Effect(current, battle, name, targets[i]);
        
		Status* status = new StatusConfusion(effect, targets[i], 1);
        
		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
        
        effect->applyEffect();
	}
}

void Enervate::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);
    
	if (checkpoint(current)) return;
    
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());
    
	vector<Unit*> targets = enemyGroup->allyUnits();
	for (int i = 0; i < targets.size(); ++i)
	{
        Effect* effect = new Effect(current, battle, name, targets[i]);
        
		Status* status = new StatusWeaken(effect, targets[i], 1);
        
		Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
		log->apply(battle);
        
        effect->applyEffect();
	}
}

void NaturesGrasp::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	if (checkpoint(current)) return;

	int rowRange = 2;
	vector<Unit*> targets = Targeter::selectRowTargets(current, battle, allyGroup, enemyGroup, rowRange, true);

	if (targets.size() > 0)
	{
		for (int i = 0; i < targets.size(); ++i) {
			Unit* target = targets[i];

			Event* log = new EventAttack(this, name, Event::MAGIC_HIT_CHANCE, type, target, true);
			log->apply(battle);

			if (log->success)
			{
				Damage* damage = new Damage(this, target, current->getCurrentMagicAttack(), DAMAGE_LOW, DAMAGE_EARTH);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);
				
				if (damage->final > 0)
				{
					Effect* effect = new Effect(current, battle, name, target);

					Status* status = new StatusParalyze(effect, target, 1);

					Event* log = new EventCauseStatus(this, name, Event::DEBUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
			}
		}
	}
}

void NaturesEmbrace::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

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
			
			vector<Unit*> sides = allyGroup->unitsAdjacentTo(target->getGridX(), target->getGridY());
			sides.push_back(target);

			for (int i = 0; i < sides.size(); ++i)
			{
				Unit* side = sides[i];

				Damage* damage = new Damage(this, side, current->getCurrentMagicAttack(), DAMAGE_LOW, DAMAGE_HEALING);

				Event* log = new EventCauseDamage(this, name, Event::AUTO_HIT_CHANCE, damage);
				log->apply(battle);

				if (damage->final < 0)
				{
					Effect* effect = new Effect(current, battle, name, side);

					Status* status = new StatusRegeneration(effect, side, 1);

					Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
					log->apply(battle);

					effect->applyEffect();
				}
			}
		}
	}
}

void ManaTree::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());

	vector<GridPoint> positions = allyGroup->getAvailablePoints();

	if (positions.size() > 0)
	{
		GridPoint pos = positions[rand() % positions.size()];

		if (allyGroup->getSummon(current) == NULL)
		{
			Character* character = new Character("Mana Tree", JOB_TREE_OF_LIFE, ELEMENT_EARTH);
			allyGroup->addSummonAt(pos.x, pos.y, character, current);
			battle->addToUnitOrder(allyGroup->getUnitAt(pos));
		}
	}
}

void Tranquility::action(Ability* previous, Unit* current, Battle* battle)
{
	Ability::action(previous, current, battle);

	if (checkpoint(current)) return;

	Group* allyGroup = battle->getAllyGroup(current->getGrid());

	vector<Unit*> targets = allyGroup->allyUnits();

	for (int i = 0; i < targets.size(); ++i)
	{
		Unit* target = targets[i];
		
		Event* log = new EventRemoveStatus(this, name, Event::CLEANSE_HIT_CHANCE, target, DEBUFF);
		log->apply(battle);

		if (log->success)
		{
			Effect* effect = new Effect(current, battle, name, target);

			Status* status = new StatusRegeneration(effect, target, 1);

			Event* log = new EventCauseStatus(this, name, Event::BUFF_HIT_CHANCE, status);
			log->apply(battle);

			effect->applyEffect();
		}
	}
}

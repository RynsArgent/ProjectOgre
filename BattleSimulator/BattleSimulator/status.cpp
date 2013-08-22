#include "status.h"

#include <cstdlib>
#include "action.h"
#include "ability.h"
#include "damage.h"
#include "event.h"
#include "target.h"
#include "battle.h"

bool Status::hasExpired() const {
	return clean || (timed && timer <= 0);
}

void Status::onSpawn()
{
	if (target != NULL) 
	{
		switch (benefit) 
		{
			case BUFF:
			{
				int val = target->getNumBuffs();
				val += 1;
				target->setNumBuffs(val);
				break;
			}
			case DEBUFF:
			{
				int val = target->getNumDebuffs();
				val += 1;
				target->setNumDebuffs(val);
				break;
			}
			case NEUTRAL:
			{
				int val = target->getNumNeutrals();
				val += 1;
				target->setNumNeutrals(val);
				break;
			}
		}
		target->currentStatus.push_back(this);
	}
}

void Status::onRound() 
{	
	if (timed && timer > 0)
		--timer;
}

void Status::onKill() 
{
	// Remove Status buff/debuff link from target
	if (!clean && target != NULL) {
		switch (benefit) 
		{
			case BUFF:
			{
				int val = target->getNumBuffs();
				val -= 1;
				target->setNumBuffs(val);
				break;
			}
			case DEBUFF:
			{
				int val = target->getNumDebuffs();
				val -= 1;
				target->setNumDebuffs(val);
				break;
			}
			case NEUTRAL:
			{
				int val = target->getNumNeutrals();
				val -= 1;
				target->setNumNeutrals(val);
				break;
			}
		}
		target->currentStatus.erase(find(target->currentStatus.begin(), target->currentStatus.end(), this));
	}
	clean = true;
}

void Status::onPrePerformHit(Event* event)
{
}

void Status::onPostPerformHit(Event* event)
{
}

void Status::onPreReactHit(Event* event)
{
}

void Status::onPostReactHit(Event* event)
{
}

void Status::onPreApplyDamage(Damage* applier)
{
}

void Status::onPostApplyDamage(Damage* applier)
{
}

void Status::onPreReceiveDamage(Damage* applier)
{
}

void Status::onPostReceiveDamage(Damage* applier)
{
}

void Status::onPreFindTarget(Targeter* system)
{
}

void Status::onPostFindTarget(Targeter* system)
{
}

void Status::onPreBecomeTarget(Targeter* system)
{
}

void Status::onPostBecomeTarget(Targeter* system)
{
}

void Status::onCheckpoint(Ability* ability)
{
}

void Status::onSelectAbility(Unit* caster)
{
}

StatusMergeResult StatusStun::getMergeResult() const
{
	StatusMergeResult res;
	res.timer = timer;
	return res;
}

void StatusStun::onMerge(const StatusMergeResult & mergeResult)
{
	timer += mergeResult.timer;
}

void StatusStun::onCheckpoint(Ability* ability)
{
    if (hasExpired())
        return;
    Status::onCheckpoint(ability);
    
    if (ability->isInterruptible())
        ability->setCancelled(true);
}

void StatusStun::onSelectAbility(Unit* caster)
{
    if (hasExpired())
        return;
	Status::onSelectAbility(caster);

	if (caster->getCurrentTier() > 0)
		caster->setCurrentTier(0);
}

StatusMergeResult StatusSleep::getMergeResult() const
{
	StatusMergeResult res;
	res.timer = timer;
	return res;
}

void StatusSleep::onMerge(const StatusMergeResult & mergeResult)
{
	timer += mergeResult.timer;
}

void StatusSleep::onPostReceiveDamage(Damage* applier)
{
    if (hasExpired())
        return;
    
    if (applier->final > 0)
        onKill();
}

void StatusSleep::onCheckpoint(Ability* ability)
{
    if (hasExpired())
        return;
    Status::onCheckpoint(ability);
    
    if (ability->isInterruptible())
        ability->setCancelled(true);
}

void StatusSleep::onSelectAbility(Unit* caster)
{
    if (hasExpired())
        return;
	Status::onSelectAbility(caster);

	if (caster->getCurrentTier() > 0)
		caster->setCurrentTier(0);
}

StatusMergeResult StatusFlee::getMergeResult() const
{
	StatusMergeResult res;
	res.timer = timer;
	return res;
}

void StatusFlee::onMerge(const StatusMergeResult & mergeResult)
{
	timer += mergeResult.timer;
}

void StatusFlee::onSpawn()
{
    Status::onSpawn();
    if (GridPoint(target->getGridX(), target->getGridY()) != GridPoint(-1, -1))
    {
        Battle* battle = effect->getBattle();
        Group* allyGroup = battle->getAllyGroup(target->getGrid());
        allyGroup->setUnitAt(target->getGridX(), target->getGridY(), NULL);
    }
    target->setOnGridX(-1);
    target->setOnGridY(-1);
}

void StatusFlee::onCheckpoint(Ability* ability)
{
    if (hasExpired())
        return;
    Status::onCheckpoint(ability);
    
    if (ability->isInterruptible())
        ability->setCancelled(true);
}

void StatusFlee::onKill()
{
    if (GridPoint(target->getGridX(), target->getGridY()) == GridPoint(-1, -1))
    {
        Battle* battle = effect->getBattle();
        Group* allyGroup = battle->getAllyGroup(target->getGrid());
        GridPoint place = GridPoint(target->getFormX(), target->getFormY());
        
        if (allyGroup->hasUnitAt(place))
        {
            vector<GridPoint> possible = allyGroup->getAvailablePoints();
            int r = rand() % possible.size();
            place = possible[r];
        }
        allyGroup->setUnitAt(place, target);	
        target->setOnGridX(place.x);
        target->setOnGridY(place.y);
    }
    Status::onKill();
}

void StatusFlee::onSelectAbility(Unit* caster)
{
    if (hasExpired())
        return;
	Status::onSelectAbility(caster);

	if (caster->getCurrentTier() > 0)
		caster->setCurrentTier(0);
}

StatusMergeResult StatusConfusion::getMergeResult() const
{
	StatusMergeResult res;
	res.timer = timer;
	return res;
}

void StatusConfusion::onMerge(const StatusMergeResult & mergeResult)
{
	timer += mergeResult.timer;
}

void StatusConfusion::onPreFindTarget(Targeter* system)
{
    if (hasExpired())
        return;
    Status::onPreFindTarget(system);
    
    if (system->method != TARGET_CONFUSED)
    {                 
		system->candidates = system->base;

        vector<Unit*> units;
        if (system->group == TARGET_ENEMIES)
            units = system->ref->getBattle()->getAllyGroup(system->ref->getSource()->getGrid())->allyUnits();
        else if (system->group == TARGET_ALLIES)
            units = system->ref->getBattle()->getEnemyGroup(system->ref->getSource()->getGrid())->allyUnits();

        for (int i = 0; i < units.size(); ++i)
            system->candidates.push_back(units[i]);

		system->method = TARGET_CONFUSED;
    }
}

void StatusConfusion::onSelectAbility(Unit* caster)
{
    if (hasExpired())
        return;
	Status::onSelectAbility(caster);
	
	if (caster->getCurrentTier() > 1)
		caster->setCurrentTier(1);
}

StatusMergeResult StatusCharm::getMergeResult() const
{
	StatusMergeResult res;
	res.timer = timer;
	return res;
}

void StatusCharm::onMerge(const StatusMergeResult & mergeResult)
{
	timer += mergeResult.timer;
}

void StatusCharm::onPostReceiveDamage(Damage* applier)
{
    if (hasExpired())
        return;
	Status::onPostReceiveDamage(applier);
	
    if (applier->final > 0)
        onKill();
}

void StatusCharm::onPreFindTarget(Targeter* system)
{
    if (hasExpired())
        return;
    Status::onPreFindTarget(system);
    
    if (system->method != TARGET_CONFUSED && system->method != TARGET_CHARMED) 
    {
        if (system->group == TARGET_ENEMIES)
            system->candidates = system->ref->getBattle()->getAllyGroup(system->ref->getSource()->getGrid())->allyUnits();
        else if (system->group == TARGET_ALLIES)
            system->candidates = system->ref->getBattle()->getEnemyGroup(system->ref->getSource()->getGrid())->allyUnits();

		system->method = TARGET_CHARMED;
    }
}

void StatusCharm::onSelectAbility(Unit* caster)
{
    if (hasExpired())
        return;
	Status::onSelectAbility(caster);

	if (caster->getCurrentTier() > 1)
		caster->setCurrentTier(1);
}

StatusMergeResult StatusPoison::getMergeResult() const
{
	StatusMergeResult res;
	res.timer = timer;
	return res;
}

void StatusPoison::onMerge(const StatusMergeResult & mergeResult)
{
	timer += mergeResult.timer;
}

void StatusPoison::applyTimedDamage()
{
	Damage* damage = new Damage(effect, target, amount, DAMAGE_MEDIUM, DAMAGE_EARTH);
	
	Event* log = new EventCauseDamage(effect, 100, damage);
	log->apply();

	/*
     // Has a nice idea for divided damage, however poison is one type
     int dividedAmount = amount / damageTypes.size();
     int dividedRemainder = amount % damageTypes.size();
     for (int i = 0; i < damageTypes.size(); ++i)
     {
     if (i < dividedRemainder)
     Damage(this, dividedAmount + 1, damageTypes[i]).apply(target);
     else
     Damage(this, dividedAmount, damageTypes[i]).apply(target);
     }
     */
}

void StatusPoison::onRound()
{
	if (hasExpired())
		return;
	Status::onRound();
    
	applyTimedDamage();
}

StatusMergeResult StatusBlind::getMergeResult() const
{
	StatusMergeResult res;
	res.timer = timer;
	return res;
}

void StatusBlind::onMerge(const StatusMergeResult & mergeResult)
{
	timer += mergeResult.timer;
}

void StatusBlind::onPrePerformHit(Event* event)
{
	if (hasExpired())
		return;
	Status::onPrePerformHit(event);

	Action* act = event->ref;
	if (act != NULL) {
		AbilityType type = act->getAbilityType();
		if (type == ABILITY_ATTACK_MELEE ||
			type == ABILITY_ATTACK_RANGE)
		{
			event->chance -= 50;
		}
	}
}

StatusMergeResult StatusMortality::getMergeResult() const
{
	StatusMergeResult res;
	res.val1 = amount;
	return res;
}

void StatusMortality::onMerge(const StatusMergeResult & mergeResult)
{
	amount += mergeResult.val1;
}

void StatusMortality::onSpawn()
{
	Status::onSpawn();
	int val = target->getMaxHealth();
	val -= amount;
	target->setMaxHealth(val);
}

void StatusMortality::onKill()
{
	int val = target->getMaxHealth();
	val += amount;
	target->setMaxHealth(val);
	Status::onKill();
}

StatusMergeResult StatusBlock::getMergeResult() const
{
	StatusMergeResult res;
	return res;
}

void StatusBlock::onMerge(const StatusMergeResult & mergeResult)
{
}

bool StatusBlock::hasExpired() const
{
	return Status::hasExpired() || (limited && amount <= 0);
}

void StatusBlock::applyDamagePrevention(Damage* applier)
{ 
	int currentPrevention = amount;
	for (DamageNode* n = applier->head; n != NULL; n = n->next) {
		if (n->type == DAMAGE_PHYSICAL) {
			int startingDamage = n->amount;
			int resultantDamage = startingDamage - currentPrevention;
			if (resultantDamage < 0)
				resultantDamage = 0;
			n->amount = resultantDamage;

			currentPrevention -= (startingDamage - resultantDamage);
		}
	}
	if (limited)
		amount = currentPrevention;
}
	
void StatusBlock::onPreReceiveDamage(Damage* applier)
{
	if (hasExpired())
		return;
	Status::onPreReceiveDamage(applier);
	
	applyDamagePrevention(applier);

	if (hasExpired())
		onKill();
}

StatusMergeResult StatusTaunt::getMergeResult() const
{
	StatusMergeResult res;
	res.timer = timer;
	return res;
}

void StatusTaunt::onMerge(const StatusMergeResult & mergeResult)
{
	timer = mergeResult.timer;
}

void StatusTaunt::addToPriorityList(Targeter* system) const
{
	for (int i = 0; i < system->candidates.size(); ++i) {
		if (focus == system->candidates[i])
			system->priorities.push_back(i);
	}
}

void StatusTaunt::onPreFindTarget(Targeter* system)
{
	if (hasExpired())
		return;
	Status::onPreFindTarget(system);
	
	addToPriorityList(system);
}

StatusMergeResult StatusBattleShout::getMergeResult() const
{
	StatusMergeResult res;
	res.timer = timer;
	return res;
}

void StatusBattleShout::onMerge(const StatusMergeResult & mergeResult)
{
	timer += mergeResult.timer;

	/*
	// This is in case Battle Shout lasts for more than 1 turn, if it does, the damage will begin stacking
	// So, under the assumption that the amount is the same for each stack. this cancel with onSpawn.
	int val = target->getCurrentPhysicalAttack();
	val -= amount;
	target->setCurrentPhysicalAttack(val);
	*/
}

void StatusBattleShout::onSpawn()
{
	Status::onSpawn();
	int val = target->getCurrentPhysicalAttack();
	val += amount;
	target->setCurrentPhysicalAttack(val);
}

void StatusBattleShout::onKill()
{
	int val = target->getCurrentPhysicalAttack();
	val -= amount;
	target->setCurrentPhysicalAttack(val);
	Status::onKill();
}

StatusMergeResult StatusHaste::getMergeResult() const
{
	StatusMergeResult res;
	return res;
}

void StatusHaste::onMerge(const StatusMergeResult & mergeResult)
{
}

void StatusHaste::onSpawn()
{
	Status::onSpawn();
	int val = target->getCurrentSpeed();
	val += amount;
	target->setCurrentSpeed(val);
}

void StatusHaste::onKill()
{
	int val = target->getCurrentSpeed();
	val -= amount;
	target->setCurrentSpeed(val);
	Status::onKill();
}

StatusMergeResult StatusScope::getMergeResult() const
{
	StatusMergeResult res;
	return res;
}

void StatusScope::onMerge(const StatusMergeResult & mergeResult)
{
}

void StatusScope::onPreApplyDamage(Damage* applier)
{
	if (hasExpired())
		return;
	Status::onPreApplyDamage(applier);

	if (!applier->action || applier->action->getAbilityType() != ABILITY_ATTACK_RANGE ||
		applier->size <= 0)
		return;
	
	int div = amount / applier->size;
	int rem = amount % applier->size;
	for (DamageNode* node = applier->head; node != NULL; node = node->next) {
		if (node == applier->head)
			node->amount += Damage::getDamageValue(node->rating, div + rem);
		else
			node->amount += Damage::getDamageValue(node->rating, div);
		node->start = node->amount;
	}
}

StatusMergeResult StatusTangleTrap::getMergeResult() const
{
	StatusMergeResult res;
	return res;
}

void StatusTangleTrap::onMerge(const StatusMergeResult & mergeResult)
{
}

void StatusTangleTrap::onPostBecomeTarget(Targeter* system)
{
	if (hasExpired())
		return;
	Status::onPostBecomeTarget(system);

	if (system->ref && system->ref->getSource() &&
		system->ref->getAbilityType() == ABILITY_ATTACK_MELEE)
	{
		string name = "Stun";
		Effect* neffect = new Effect(effect->getSource(), effect->getBattle(), name, system->ref->getSource());
		Status* status = new StatusStun(neffect, "Stun", system->ref->getSource());
		status->setTimed(true, 1);
		
		Event* log = new EventCauseStatus(effect, 100, status);
		log->apply();

		neffect->applyEffect();

		onKill();
	}
}

void Effect::print() const
{
}

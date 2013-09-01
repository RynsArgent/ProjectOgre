#include "status.h"

#include <cstdlib>
#include "action.h"
#include "ability.h"
#include "damage.h"
#include "event.h"
#include "target.h"
#include "group.h"
#include "battle.h"

bool Status::hasExpired() const {
	return clean || numStacks <= 0 || (timed && timer <= 0);
}

bool Status::canMergeWith(Status* rhs) const {
	// Sanity check, if the match modes are not the same, there is a problem
	if (match != rhs->match)
		return false;

	// If the two Status effects are not even for the same target, do not merge.
	if (!target || !rhs->target || target->getName() != rhs->target->getName())
		return false;

	if (this->hasExpired() || rhs->hasExpired())
		return false;

	switch (match) 
	{
	case STATUS_UNMATCHABLE:
		return false;
	case STATUS_SELFMATCHABLE:
		{
			if (subname != rhs->subname)
				return false;
			Unit* lhsSource = effect->getSource();
			Unit* rhsSource = rhs->effect->getSource();
			if (!lhsSource || !rhsSource)
				return true;
			return lhsSource->getName() == rhsSource->getName();
		}
	case STATUS_ALLMATCHABLE:
		return subname == rhs->subname;
	}
	return false;
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

StatusInstance Status::getMergeResult() const
{
	StatusInstance res;
	res.timer = timer;
	res.stacks = numStacks;
	return res;
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

StatusInstance StatusStun::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusStun::onMerge(const StatusInstance & mergeResult)
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

StatusInstance StatusSleep::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusSleep::onMerge(const StatusInstance & mergeResult)
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

StatusInstance StatusFlee::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusFlee::onMerge(const StatusInstance & mergeResult)
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

StatusInstance StatusConfusion::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusConfusion::onMerge(const StatusInstance & mergeResult)
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

StatusInstance StatusCharm::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusCharm::onMerge(const StatusInstance & mergeResult)
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

StatusInstance StatusPoison::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusPoison::onMerge(const StatusInstance & mergeResult)
{
	addStacks(mergeResult.stacks);
}

void StatusPoison::applyTimedDamage()
{
	Damage* damage = new Damage(effect, target, numStacks * amount, DAMAGE_MEDIUM, DAMAGE_EARTH);
	
	Event* log = new EventCauseDamage(effect, Event::AUTO_HIT_CHANCE, damage);
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

StatusInstance StatusBleed::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusBleed::onMerge(const StatusInstance & mergeResult)
{
	addStacks(mergeResult.stacks);
}

void StatusBleed::applyTimedDamage()
{
	Damage* damage = new Damage(effect, target, numStacks * amount, DAMAGE_MEDIUM, DAMAGE_PHYSICAL);
	
	Event* log = new EventCauseDamage(effect, Event::AUTO_HIT_CHANCE, damage);
	log->apply();
}

void StatusBleed::onRound()
{
	if (hasExpired())
		return;
	Status::onRound();
    
	applyTimedDamage();
}

StatusInstance StatusBurn::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusBurn::onMerge(const StatusInstance & mergeResult)
{
	addStacks(mergeResult.stacks);
}

void StatusBurn::applyTimedDamage()
{
	Damage* damage = new Damage(effect, target, numStacks * amount, DAMAGE_MEDIUM, DAMAGE_FIRE);
	
	Event* log = new EventCauseDamage(effect, Event::AUTO_HIT_CHANCE, damage);
	log->apply();
}

void StatusBurn::onRound()
{
	if (hasExpired())
		return;
	Status::onRound();
    
	applyTimedDamage();
}

StatusInstance StatusRegeneration::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusRegeneration::onMerge(const StatusInstance & mergeResult)
{
	addStacks(mergeResult.stacks);
}

void StatusRegeneration::applyTimedHeal()
{
	int healAmount = numStacks * effect->getSource()->getCurrentMagicAttack();
	Damage* damage = new Damage(effect, target, healAmount, DAMAGE_LOW, DAMAGE_HEALING);
	
	Event* log = new EventCauseDamage(effect, Event::AUTO_HIT_CHANCE, damage);
	log->apply();
}

void StatusRegeneration::onRound()
{
	if (hasExpired())
		return;
	Status::onRound();
    
	applyTimedHeal();
}

StatusInstance StatusPolymorph::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusPolymorph::onMerge(const StatusInstance & mergeResult)
{
	timer += mergeResult.timer;
}

void StatusPolymorph::onPostReceiveDamage(Damage* applier)
{
    if (hasExpired())
        return;
    
    if (applier->final > 0)
        onKill();
}

void StatusPolymorph::onCheckpoint(Ability* ability)
{
    if (hasExpired())
        return;
    Status::onCheckpoint(ability);
    
    if (ability->isInterruptible())
        ability->setCancelled(true);
}

void StatusPolymorph::onSelectAbility(Unit* caster)
{
    if (hasExpired())
        return;
	Status::onSelectAbility(caster);

	if (caster->getCurrentTier() > 0)
		caster->setCurrentTier(0);
}

StatusInstance StatusBlind::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusBlind::onMerge(const StatusInstance & mergeResult)
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

void StatusMortality::modifyAttributes(int dvalue)
{
	int val = target->getMaxHealth();
	val += dvalue;
	target->setMaxHealth(val);
}

StatusInstance StatusMortality::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusMortality::onMerge(const StatusInstance & mergeResult)
{
	int stacks = addStacks(mergeResult.stacks);
	modifyAttributes(-stacks * amount);
}

void StatusMortality::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(-numStacks * amount);
}

void StatusMortality::onKill()
{
	modifyAttributes(numStacks * amount);
	Status::onKill();
}

StatusInstance StatusBlock::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusBlock::onMerge(const StatusInstance & mergeResult)
{
}

bool StatusBlock::hasExpired() const
{
	return Status::hasExpired() || (limited && amount <= 0);
}

void StatusBlock::applyDamagePrevention(Damage* applier)
{ 
	int currentPrevention = numStacks * amount;
	for (DamageNode* n = applier->head; n != NULL; n = n->next) {
		if (n->type == DAMAGE_PHYSICAL) {
			int startingDamage = n->amount;
			int resultantDamage = startingDamage - currentPrevention;
			bound(resultantDamage, VALUE_DAMAGE);
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

StatusInstance StatusTaunt::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusTaunt::onMerge(const StatusInstance & mergeResult)
{
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

void StatusBattleShout::modifyAttributes(int dvalue)
{
	int val = target->getCurrentPhysicalAttack();
	val += dvalue;
	target->setCurrentPhysicalAttack(val);
}

StatusInstance StatusBattleShout::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusBattleShout::onMerge(const StatusInstance & mergeResult)
{
	int stacks = addStacks(mergeResult.stacks);
	modifyAttributes(stacks * amount);
}

void StatusBattleShout::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(numStacks * amount);
}

void StatusBattleShout::onKill()
{
	modifyAttributes(-numStacks * amount);
	Status::onKill();
}

void StatusBarrier::modifyAttributes(int dvalue)
{
	int val;
	val = target->getCurrentFireDefense();
	val += dvalue;
	target->setCurrentFireDefense(val);
	val = target->getCurrentWaterDefense();
	val += dvalue;
	target->setCurrentWaterDefense(val);
	val = target->getCurrentEarthDefense();
	val += dvalue;
	target->setCurrentEarthDefense(val);
	val = target->getCurrentIceDefense();
	val += dvalue;
	target->setCurrentIceDefense(val);
	val = target->getCurrentLightningDefense();
	val += dvalue;
	target->setCurrentLightningDefense(val);
}

StatusInstance StatusBarrier::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusBarrier::onMerge(const StatusInstance & mergeResult)
{
	cout << "Merging " << endl;
	int stacks = addStacks(mergeResult.stacks);
	modifyAttributes(stacks * amount);
}

void StatusBarrier::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(numStacks * amount);
}

void StatusBarrier::onKill()
{
	modifyAttributes(-numStacks * amount);
	Status::onKill();
}

void StatusHaste::modifyAttributes(int dvalue)
{
	int val = target->getCurrentSpeed();
	val += dvalue;
	target->setCurrentSpeed(val);
}

StatusInstance StatusHaste::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusHaste::onMerge(const StatusInstance & mergeResult)
{
	int stacks = addStacks(mergeResult.stacks);
	modifyAttributes(stacks * amount);
}

void StatusHaste::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(numStacks * amount);
}

void StatusHaste::onKill()
{
	modifyAttributes(-numStacks * amount);
	Status::onKill();
}

void StatusChill::modifyAttributes(int dvalue)
{
	int val = target->getCurrentSpeed();
	val += dvalue;
	target->setCurrentSpeed(val);
}

StatusInstance StatusChill::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusChill::onMerge(const StatusInstance & mergeResult)
{
	int stacks = addStacks(mergeResult.stacks);
	modifyAttributes(-stacks * amount);
}

void StatusChill::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(-numStacks * amount);
}

void StatusChill::onKill()
{
	int val = target->getCurrentSpeed();
	modifyAttributes(numStacks * amount);
}

StatusInstance StatusScope::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusScope::onMerge(const StatusInstance & mergeResult)
{
	addStacks(mergeResult.stacks);
}

void StatusScope::onPreApplyDamage(Damage* applier)
{
	if (hasExpired())
		return;
	Status::onPreApplyDamage(applier);

	if (!applier->action || applier->action->getAbilityType() != ABILITY_ATTACK_RANGE ||
		applier->size <= 0)
		return;
	
	int div = (numStacks * amount) / applier->size;
	int rem = (numStacks * amount) % applier->size;

	for (DamageNode* node = applier->head; node != NULL; node = node->next) {
		if (node == applier->head)
			node->start += Damage::getDamageValue(node->rating, div + rem);
		else
			node->start += Damage::getDamageValue(node->rating, div);
		node->amount = node->start;
	}
}

StatusInstance StatusTangleTrap::getMergeResult() const
{
	StatusInstance res = Status::getMergeResult();
	return res;
}

void StatusTangleTrap::onMerge(const StatusInstance & mergeResult)
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
		Status* status = new StatusStun(neffect, system->ref->getSource(), 1);
		
		Event* log = new EventCauseStatus(effect, Event::DEBUFF_HIT_CHANCE, status);
		log->apply();

		neffect->applyEffect();

		onKill();
	}
}

void Effect::print(ostream& out) const
{
}

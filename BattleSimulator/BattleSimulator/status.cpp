#include "status.h"

#include "action.h"
#include "ability.h"
#include "damage.h"
#include "event.h"
#include "target.h"
#include "group.h"
#include "battle.h"
#include <cstdlib>

int Status::useStacks() const
{
	if (collective)
		return grouplist->getTotalStacks();
	return stacks;
}

int Status::addStacks(int oldstacks)
{
	int add = stacks;
	if (oldstacks + add > this->getMaxSingleStacks())
		add = this->getMaxSingleStacks() - oldstacks;
	oldstacks += add;
	stacks = oldstacks;
	return add;
}

bool Status::hasExpired() const {
	return clean || stacks <= 0 || (timed && timer <= 0);
}

void Status::findSetMerger() 
{
	vector<StatusGroup*> statusGroups = target->getStatusBySubname(subname);

	merger = NULL;
	for (int i = 0; i < statusGroups.size(); ++i) {
		Status* partner = statusGroups[i]->getMatchingStatus(this);
		if (partner != NULL) {
			merger = partner;
			break;
		}
	}
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
		// Cannot not merge with any status
		return false;
	case STATUS_SELFMATCHABLE:
		{
			// Only merge with similar status of same source
			if (subname != rhs->subname)
				return false;
			Unit* lhsSource = effect->getSource();
			Unit* rhsSource = rhs->effect->getSource();
			if (!lhsSource || !rhsSource)
				return true;
			return lhsSource->getName() == rhsSource->getName();
		}
	case STATUS_ALLMATCHABLE:
		// Merge with any similar status
		return subname == rhs->subname;
	}
	return false;
}

int Status::onMerge(Status* status)
{
	int appliedStacks = 0;
	grouplist = status->grouplist;
	if (instancing) {
		// Add a new instance,
        // new stacks are determined by the StatusGroup class
        // with MAX_GROUP_STACKS and with each own instance's
        // MAX_SINGLE_STACKS
		appliedStacks = grouplist->addInstance(this);
	} else {
		// Merge the two instances, there should always be
        // one instance in the Status Group. the new stacks
        // are determined by MAX_SINGLE_STACKS
		appliedStacks = addStacks(status->getStacks());
        
        // Before killing the old status effect, we need
        // to add the new Status instance so the Status
        // Group object is not deallocated
        grouplist->addInstance(this);
		status->Status::onKill();
	}
    
    // Refresh effect order of old status;
    Effect* mergeEffect = status->getEffect();
    mergeEffect->getTrigger()->eraseEffect(mergeEffect);
    mergeEffect->getTrigger()->addEffect(mergeEffect);
    
	// Refresh the order
	if (target != NULL && grouplist != NULL) {
		target->eraseStatusGroup(grouplist);
		target->addStatusGroup(grouplist);
	}

	return appliedStacks;
}

void Status::onSpawn()
{
	grouplist = new StatusGroup(subname, target, benefit, match, category, dispellable, instancing, collective,
		this->getMaxSingleStacks(), this->getMaxGroupStacks());
	grouplist->addInstance(this);
	if (target != NULL && grouplist != NULL)
		target->addStatusGroup(grouplist);
}

void Status::onKill() 
{
	// Remove Status buff/debuff link from target
	if (!clean) {
		grouplist->removeInstance(this);

		// Deallocate the grouplist if it contains no stacks
		if (grouplist->getTotalStacks() <= 0) {
			if (target != NULL)
				target->eraseStatusGroup(grouplist);
			effect->getBattle()->addToCleanup(grouplist);
		}
	}
	setCleaning();
}

void Status::onRound() 
{	
	if (timed && timer > 0)
		--timer;
}

void Status::onPrePerformHit(Event* evt)
{
}

void Status::onPostPerformHit(Event* evt)
{
}

void Status::onPreReactHit(Event* evt)
{
}

void Status::onPostReactHit(Event* evt)
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

void Status::onExecuteAbility(Ability* ability)
{
}

int StatusStun::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	timer += status->getTimer();
	return appliedStacks;
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

int StatusSleep::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	timer += status->getTimer();
	return appliedStacks;
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


int StatusFlee::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	timer += status->getTimer();
	return appliedStacks;
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

int StatusConfusion::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	timer += status->getTimer();
	return appliedStacks;
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

int StatusDemoralize::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	timer += status->getTimer();
	return appliedStacks;
}

void StatusDemoralize::onSelectAbility(Unit* caster)
{
    if (hasExpired())
        return;
	Status::onSelectAbility(caster);
	
	if (caster->getCurrentTier() > 1)
		caster->setCurrentTier(1);
}

int StatusCharm::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	timer += status->getTimer();
	return appliedStacks;
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

int StatusPoison::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	return appliedStacks;
}

void StatusPoison::applyTimedDamage()
{
	Damage* damage = new Damage(NULL, target, useStacks() * amount, DAMAGE_MEDIUM, DAMAGE_EARTH);
	
	Event* log = new EventCauseDamage(this->effect, grouplist->getSubname(), Event::AUTO_HIT_CHANCE, damage, true);
	log->apply(effect->getBattle());

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

	if (target->isAlive() && (!collective || !grouplist->isExecuted()))
		applyTimedDamage();
}

int StatusBleed::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	return appliedStacks;
}

void StatusBleed::applyTimedDamage()
{
	Damage* damage = new Damage(NULL, target, useStacks() * amount, DAMAGE_MEDIUM, DAMAGE_PHYSICAL);
	
	Event* log = new EventCauseDamage(this->effect, grouplist->getSubname(), Event::AUTO_HIT_CHANCE, damage, true);
	log->apply(effect->getBattle());
}

void StatusBleed::onRound()
{
	if (hasExpired())
		return;
	Status::onRound();
    
	if (target->isAlive() && (!collective || !grouplist->isExecuted()))
		applyTimedDamage();
}

int StatusBurn::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	return appliedStacks;
}

void StatusBurn::applyTimedDamage()
{
	Damage* damage = new Damage(NULL, target, useStacks() * amount, DAMAGE_MEDIUM, DAMAGE_FIRE);
	
	Event* log = new EventCauseDamage(this->effect, grouplist->getSubname(), Event::AUTO_HIT_CHANCE, damage, true);
	log->apply(effect->getBattle());
}

void StatusBurn::onRound()
{
	if (hasExpired())
		return;
	Status::onRound();
    
	if (target->isAlive() && (!collective || !grouplist->isExecuted()))
		applyTimedDamage();
}

int StatusRegeneration::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	return appliedStacks;
}

void StatusRegeneration::applyTimedHeal()
{
	//int healAmount = useStacks() * effect->getSource()->getCurrentMagicAttack();
	int healAmount = useStacks() * amount;
	Damage* damage = new Damage(NULL, target, useStacks() * healAmount, DAMAGE_MEDIUM, DAMAGE_HEALING);
	
	Event* log = new EventCauseDamage(this->effect, grouplist->getSubname(), Event::AUTO_HIT_CHANCE, damage, true);
	log->apply(effect->getBattle());
}

void StatusRegeneration::onRound()
{
	if (hasExpired())
		return;
	Status::onRound();
    
	if (target->isAlive() && (!collective || !grouplist->isExecuted()))
		applyTimedHeal();
}

int StatusPolymorph::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	timer += status->getTimer();
	return appliedStacks;
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

int StatusBlind::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	timer += status->getTimer();
	return appliedStacks;
}

void StatusBlind::onPrePerformHit(Event* evt)
{
	if (hasExpired())
		return;
	Status::onPrePerformHit(evt);

	Action* act = evt->ref;
	if (act != NULL) {
		AbilityType type = act->getAbilityType();
		if (type == ABILITY_ATTACK_MELEE ||
			type == ABILITY_ATTACK_RANGE)
		{
			evt->chance -= 50;
		}
	}
}

void StatusMortality::modifyAttributes(int dvalue)
{
	int val = target->getMaxHealth();
	val += dvalue;
	target->setMaxHealth(val);
}

int StatusMortality::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	modifyAttributes(-appliedStacks * amount);
	return appliedStacks;
}

void StatusMortality::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(-stacks * amount);
}

void StatusMortality::onKill()
{
	modifyAttributes(stacks * amount);
	Status::onKill();
}

int StatusShield::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	return appliedStacks;
}

bool StatusShield::hasExpired() const
{
	return Status::hasExpired() || (limited && amount <= 0);
}

void StatusShield::applyDamagePrevention(Damage* applier)
{ 
	int currentPrevention = useStacks() * amount;
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
	
void StatusShield::onPreReceiveDamage(Damage* applier)
{
	if (hasExpired())
		return;
	Status::onPreReceiveDamage(applier);
	
	applyDamagePrevention(applier);

	if (hasExpired())
		onKill();
}

int StatusBlock::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	return appliedStacks;
}

void StatusBlock::applyDamageReduction(Damage* applier)
{ 
	for (DamageNode* n = applier->head; n != NULL; n = n->next) {
		if (n->type == DAMAGE_PHYSICAL) {
			int startingDamage = n->amount;
			int resultantDamage = startingDamage / 2;
			bound(resultantDamage, VALUE_DAMAGE);
			n->amount = resultantDamage;
		}
	}
}
	
void StatusBlock::onPreReceiveDamage(Damage* applier)
{
	if (hasExpired())
		return;
	Status::onPreReceiveDamage(applier);

	// Special rules for global trigger
	Unit* source = effect->getSource();
	Unit* target = applier->target;
	if (source->getGrid() == target->getGrid() && // from same group?
		source->getGridY() < target->getGridY()) // target is behind blocker
	{
		applyDamageReduction(applier);
	}
}

int StatusTaunt::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	return appliedStacks;
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

int StatusBattleShout::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	modifyAttributes(appliedStacks * amount);
	return appliedStacks;
}

void StatusBattleShout::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(stacks * amount);
}

void StatusBattleShout::onKill()
{
	modifyAttributes(-stacks * amount);
	Status::onKill();
}

void StatusBarrier::modifyAttributes(int dvalue)
{
	int val;
	val = target->getCurrentArcaneDefense();
	val += dvalue;
	target->setCurrentArcaneDefense(val);
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

int StatusBarrier::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	modifyAttributes(appliedStacks * amount);
	return appliedStacks;
}

void StatusBarrier::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(stacks * amount);
}

void StatusBarrier::onKill()
{
	modifyAttributes(-stacks * amount);
	Status::onKill();
}

void StatusHaste::modifyAttributes(int dvalue)
{
	int val = target->getCurrentSpeed();
	val += dvalue;
	target->setCurrentSpeed(val);
}

int StatusHaste::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	modifyAttributes(appliedStacks * amount);
	return appliedStacks;
}

void StatusHaste::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(stacks * amount);
}

void StatusHaste::onKill()
{
	modifyAttributes(-stacks * amount);
	Status::onKill();
}

void StatusChill::modifyAttributes(int dvalue)
{
	int val = target->getCurrentSpeed();
	val += dvalue;
	target->setCurrentSpeed(val);
}

int StatusChill::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	modifyAttributes(-appliedStacks * amount);
	return appliedStacks;
}

void StatusChill::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(-stacks * amount);
}

void StatusChill::onKill()
{
	int val = target->getCurrentSpeed();
	modifyAttributes(stacks * amount);
}

int StatusScope::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	return appliedStacks;
}

void StatusScope::onPreApplyDamage(Damage* applier)
{
	if (hasExpired())
		return;
	Status::onPreApplyDamage(applier);

	if (!applier->action || applier->action->getAbilityType() != ABILITY_ATTACK_RANGE ||
		applier->size <= 0)
		return;
	
	int div = (useStacks() * amount) / applier->size;
	int rem = (useStacks() * amount) % applier->size;

	for (DamageNode* node = applier->head; node != NULL; node = node->next) {
		if (node == applier->head)
			node->start += Damage::getDamageValue(node->rating, div + rem);
		else
			node->start += Damage::getDamageValue(node->rating, div);
		node->amount = node->start;
	}
}

int StatusTangleTrap::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	return appliedStacks;
}

void StatusTangleTrap::onPostBecomeTarget(Targeter* system)
{
	if (hasExpired())
		return;
	Status::onPostBecomeTarget(system);

	if (system->ref && system->ref->getSource() &&
		system->ref->getAbilityType() == ABILITY_ATTACK_MELEE)
	{
		Effect* neffect = new Effect(effect->getSource(), effect->getBattle(), effect->getName(), system->ref->getSource());
		Status* status = new StatusStun(neffect, system->ref->getSource(), 1);
		
		Event* log = new EventCauseStatus(effect, effect->getName(), Event::DEBUFF_HIT_CHANCE, status);
		log->apply(effect->getBattle());

		neffect->applyEffect();

		onKill();
	}
}

int StatusRally::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	return appliedStacks;
}

void StatusRally::onExecuteAbility(Ability* ability)
{
	if (hasExpired())
		return;
	Status::onExecuteAbility(ability);
    
    ability->setRespondable(false);
    ability->setInterruptible(false);
    onKill();
}

void Effect::print(ostream& out) const
{
}

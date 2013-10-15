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

void Status::onBeginRound() 
{
}

void Status::onEndRound() 
{	
	if (!fresh && timed && timer > 0)
		--timer;
}

void Status::onPrePerformHit(EventAttack* evt)
{
}

void Status::onPostPerformHit(EventAttack* evt)
{
}

void Status::onPreReactHit(EventAttack* evt)
{
}

void Status::onPostReactHit(EventAttack* evt)
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

void StatusParalyze::onExecuteAbility(Ability* ability)
{
    if (hasExpired())
        return;
	Status::onExecuteAbility(ability);

	if (ability->isInterruptible())
	{
		int r = rand() % 100 + 1;
		if (r > 50)
			ability->setCancelled(true);
	}
}

void StatusSleep::onPostReceiveDamage(Damage* applier)
{
    if (hasExpired())
        return;
    
    if (applier->final > 0 && !fresh)
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
    if (GridPoint(target->getGridX(), target->getGridY()) == GridPoint(-1, -1) && !target->isDead())
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

void StatusDemoralize::onSelectAbility(Unit* caster)
{
    if (hasExpired())
        return;
	Status::onSelectAbility(caster);
	
	if (caster->getCurrentTier() > 1)
		caster->setCurrentTier(1);
}

void StatusCharm::onPostReceiveDamage(Damage* applier)
{
    if (hasExpired())
        return;
	Status::onPostReceiveDamage(applier);
	
    if (applier->final > 0 && !fresh)
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

void StatusPoison::onBeginRound()
{
	if (hasExpired())
		return;
	Status::onBeginRound();

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

void StatusBleed::onBeginRound()
{
	if (hasExpired())
		return;
	Status::onBeginRound();
    
	if (target->isAlive() && (!collective || !grouplist->isExecuted()))
		applyTimedDamage();
}

int StatusBurn::onMerge(Status* status)
{
	timer += status->getTimer();
	int appliedStacks = Status::onMerge(status); // Order matters, timer is set to 0 here
	return appliedStacks;
}

void StatusBurn::applyTimedDamage()
{
	Damage* damage = new Damage(NULL, target, useStacks() * amount, DAMAGE_MEDIUM, DAMAGE_FIRE);
	
	Event* log = new EventCauseDamage(this->effect, grouplist->getSubname(), Event::AUTO_HIT_CHANCE, damage, true);
	log->apply(effect->getBattle());
}

void StatusBurn::onBeginRound()
{
	if (hasExpired())
		return;
	Status::onBeginRound();
    
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

void StatusRegeneration::onBeginRound()
{
	if (hasExpired())
		return;
	Status::onBeginRound();
    
	if (target->isAlive() && (!collective || !grouplist->isExecuted()))
		applyTimedHeal();
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

void StatusBlind::onPrePerformHit(EventAttack* evt)
{
	if (hasExpired())
		return;
	Status::onPrePerformHit(evt);

	AbilityType atkType = evt->attack;
	if (!evt->isAutomaticSuccess() &&
		(atkType == ABILITY_ATTACK_MELEE ||
		 atkType == ABILITY_ATTACK_RANGE ||
		 atkType == ABILITY_ATTACK_MAGIC))
	{
		evt->chance -= 50;
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

void StatusVitality::modifyAttributes(int dvalue)
{
	int val = target->getMaxHealth();
	val += dvalue;
	target->setMaxHealth(val);
	if (dvalue > 0)
		target->setCurrentHealth(target->getCurrentHealth() + dvalue);
}

int StatusVitality::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	modifyAttributes(appliedStacks * amount);
	return appliedStacks;
}

void StatusVitality::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(stacks * amount);
}

void StatusVitality::onKill()
{
	modifyAttributes(-stacks * amount);
	Status::onKill();
}

bool StatusShield::hasExpired() const
{
	return Status::hasExpired() || amount <= 0 || applied;
}

void StatusShield::applyDamagePrevention(Damage* applier)
{ 
	int currentPrevention = useStacks() * amount;
	for (DamageNode* n = applier->head; n != NULL; n = n->next) {
		if (n->type == DAMAGE_PHYSICAL) {
			if (!n->pierce)
			{
				int startingDamage = n->amount;
				int resultantDamage = startingDamage - currentPrevention;
				bound(resultantDamage, VALUE_DAMAGE);
				n->amount = resultantDamage;
				currentPrevention -= (startingDamage - resultantDamage);
			}
		}
	}
	amount = currentPrevention;
	applied = true;
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

bool StatusShell::hasExpired() const
{
	return Status::hasExpired() || amount <= 0 || applied;
}

void StatusShell::applyDamagePrevention(Damage* applier)
{ 
	int currentPrevention = useStacks() * amount;
	for (DamageNode* n = applier->head; n != NULL; n = n->next) {
		if (n->type != DAMAGE_PHYSICAL) {
			if (!n->pierce)
			{
				int startingDamage = n->amount;
				int resultantDamage = startingDamage - currentPrevention;
				bound(resultantDamage, VALUE_DAMAGE);
				n->amount = resultantDamage;
				currentPrevention -= (startingDamage - resultantDamage);
			}
		}
	}
	amount = currentPrevention;
	applied = true;
}
	
void StatusShell::onPreReceiveDamage(Damage* applier)
{
	if (hasExpired())
		return;
	Status::onPreReceiveDamage(applier);
	
	applyDamagePrevention(applier);

	if (hasExpired())
		onKill();
}

void StatusBlock::onPreReactHit(EventAttack* evt)
{
	if (hasExpired())
		return;
	Status::onPreReactHit(evt);
	
	Unit* source = effect->getSource();
	Unit* target = evt->target;
	if (source && target && source->getGrid() == target->getGrid()) // Same group
	{
		// check if source is alive, target is behind blocker, the attack is an ability and a melee or ranged attack
		if (source->isAvailable() && 
			source->getGridY() < target->getGridY() && evt->ref->getAction() != EFFECT_TRIGGER &&
			(evt->ref->getAbilityType() == ABILITY_ATTACK_MELEE || evt->ref->getAbilityType() == ABILITY_ATTACK_RANGE)) 
		{
			Ability* response = NULL;
			// Execute the ability
			response = Ability::getAbility(BLOCK);
			response->action(static_cast<Ability*>(evt->ref), source, effect->getBattle());
			
			onKill();
		}
	}
}

void StatusBarrier::onPreReactHit(EventAttack* evt)
{
	if (hasExpired())
		return;
	Status::onPreReactHit(evt);
	
	Unit* source = effect->getSource();
	Unit* target = evt->target;
	if (source && target && source->getGrid() == target->getGrid()) // Same group
	{
		// check if source is alive, target is behind blocker, the attack is an ability and a magic attack
		if (source->isAvailable() && 
			source->getGridY() < target->getGridY() && evt->ref->getAction() != EFFECT_TRIGGER &&
			evt->ref->getAbilityType() == ABILITY_ATTACK_MAGIC)
		{
			Ability* response = NULL;
			// Execute the ability
			response = Ability::getAbility(BARRIER);
			response->action(static_cast<Ability*>(evt->ref), source, effect->getBattle());
			
			onKill();
		}
	}
}

void StatusBlink::onPreReactHit(EventAttack* evt)
{
	if (hasExpired())
		return;
	Status::onPreReactHit(evt);

	Unit* target = evt->target;
	if (target)
	{
		// check if target is alive and the attack is an ability and a melee/ranged/magic attack
		if (target->isAvailable() && 
			evt->ref->getAction() != EFFECT_TRIGGER &&
			(evt->ref->getAbilityType() == ABILITY_ATTACK_MELEE || 
			evt->ref->getAbilityType() == ABILITY_ATTACK_RANGE || 
			evt->ref->getAbilityType() == ABILITY_ATTACK_MAGIC)) 
		{
			Ability* response = NULL;
			// Execute the ability
			response = Ability::getAbility(BLINK);
			response->action(static_cast<Ability*>(evt->ref), target, effect->getBattle());
			
			onKill();
		}
	}
}

void StatusWounding::applyHealingReduction(Damage* applier)
{ 
	for (DamageNode* n = applier->head; n != NULL; n = n->next) {
		if (n->type == DAMAGE_HEALING && !n->pierce) {
			int startingDamage = n->amount;
			int resultantDamage = startingDamage / 2;
			bound(resultantDamage, VALUE_DAMAGE);
			n->amount = resultantDamage;
		}
	}
}
	
void StatusWounding::onPreReceiveDamage(Damage* applier)
{
	if (hasExpired())
		return;
	Status::onPreReceiveDamage(applier);

	applyHealingReduction(applier);
}

void StatusProvoke::addToPriorityList(Targeter* system) const
{
	for (int i = 0; i < system->candidates.size(); ++i) {
		if (focus == system->candidates[i])
			system->priorities.push_back(i);
	}
}

void StatusProvoke::onPreFindTarget(Targeter* system)
{
	if (hasExpired())
		return;
	Status::onPreFindTarget(system);
	
	addToPriorityList(system);
}

void StatusMarked::addToPriorityList(Targeter* system) const
{
	for (int i = 0; i < system->candidates.size(); ++i) {
		if (target == system->candidates[i])
			system->priorities.push_back(i);
	}
}
	
void StatusMarked::onPreBecomeTarget(Targeter* system)
{
	if (hasExpired())
		return;
	Status::onPreBecomeTarget(system);

	if (system->group == TARGET_ENEMIES)
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

void StatusDetermination::modifyAttributes(int dvalue)
{
	int val;
	val = target->getCurrentPhysicalAttack();
	val += dvalue;
	target->setCurrentPhysicalAttack(val);
	val = target->getCurrentMagicAttack();
	val += dvalue;
	target->setCurrentMagicAttack(val);
}

int StatusDetermination::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	modifyAttributes(appliedStacks * amount);
	return appliedStacks;
}

void StatusDetermination::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(stacks * amount);
}

void StatusDetermination::onKill()
{
	modifyAttributes(-stacks * amount);
	Status::onKill();
}

void StatusWeaken::modifyAttributes(int dvalue)
{
	int val;
	val = target->getCurrentPhysicalAttack();
	val += dvalue;
	target->setCurrentPhysicalAttack(val);
	val = target->getCurrentMagicAttack();
	val += dvalue;
	target->setCurrentMagicAttack(val);
}

int StatusWeaken::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	modifyAttributes(-appliedStacks * amount);
	return appliedStacks;
}

void StatusWeaken::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(-stacks * amount);
}

void StatusWeaken::onKill()
{
	modifyAttributes(stacks * amount);
	Status::onKill();
}

void StatusResistance::modifyAttributes(int dvalue)
{
	int val;
	val = target->getCurrentPhysicalDefense();
	val += dvalue;
	target->setCurrentPhysicalDefense(val);
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

int StatusResistance::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	modifyAttributes(appliedStacks * amount);
	return appliedStacks;
}

void StatusResistance::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(stacks * amount);
}

void StatusResistance::onKill()
{
	modifyAttributes(-stacks * amount);
	Status::onKill();
}

void StatusVulnerability::modifyAttributes(int dvalue)
{
	int val;
	val = target->getCurrentPhysicalDefense();
	val += dvalue;
	target->setCurrentPhysicalDefense(val);
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

int StatusVulnerability::onMerge(Status* status)
{
	int appliedStacks = Status::onMerge(status);
	modifyAttributes(-appliedStacks * amount);
	return appliedStacks;
}

void StatusVulnerability::onSpawn()
{
	Status::onSpawn();
	modifyAttributes(-stacks * amount);
}

void StatusVulnerability::onKill()
{
	modifyAttributes(stacks * amount);
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

void StatusTangleTrap::onPreReactHit(EventAttack* evt)
{
	if (hasExpired())
		return;
	Status::onPreReactHit(evt);
	
	AbilityType atkType = evt->attack;
	if (atkType == ABILITY_ATTACK_MELEE && !evt->indirect && evt->ref)
	{
		Damage* damage = new Damage(NULL, target, useStacks() * AMOUNT, DAMAGE_MEDIUM, DAMAGE_EARTH);
	
		Event* log = new EventCauseDamage(this->effect, grouplist->getSubname(), Event::AUTO_HIT_CHANCE, damage, true);
		log->apply(effect->getBattle());

		if (damage->final > 0)
		{
			Effect* neffect = new Effect(effect->getSource(), effect->getBattle(), effect->getName(), evt->ref->getSource());
			Status* status = new StatusStun(neffect, evt->ref->getSource(), 1);
		
			Event* log = new EventCauseStatus(effect, effect->getName(), Event::DEBUFF_HIT_CHANCE, status);
			log->apply(effect->getBattle());

			if (log->success)
				evt->chance = -1; // Auto fail if stun succeeded

			neffect->applyEffect();
		}
		onKill();
	}
}
/*
void StatusTangleTrap::onPostBecomeTarget(Targeter* system)
{
	if (hasExpired())
		return;
	Status::onPostBecomeTarget(system);

	if (system->ref && system->ref->getSource() &&
		system->ref->getAbilityType() == ABILITY_ATTACK_MELEE)
	{
		Damage* damage = new Damage(NULL, target, useStacks() * AMOUNT, DAMAGE_MEDIUM, DAMAGE_EARTH);
	
		Event* log = new EventCauseDamage(this->effect, grouplist->getSubname(), Event::AUTO_HIT_CHANCE, damage, true);
		log->apply(effect->getBattle());

		if (damage->final > 0)
		{
			Effect* neffect = new Effect(effect->getSource(), effect->getBattle(), effect->getName(), system->ref->getSource());
			Status* status = new StatusStun(neffect, system->ref->getSource(), 1);
		
			Event* log = new EventCauseStatus(effect, effect->getName(), Event::DEBUFF_HIT_CHANCE, status);
			log->apply(effect->getBattle());

			neffect->applyEffect();
		}
		onKill();
	}
}
*/
void StatusRally::onExecuteAbility(Ability* ability)
{
	if (hasExpired())
		return;
	Status::onExecuteAbility(ability);
    
	Unit* source = effect->getSource();
	Unit* caster = ability->getSource();
	if (source && caster && source->getGrid() == caster->getGrid()) // Same group
	{
		ability->setRespondable(false);
		ability->setInterruptible(false);
		onKill();
	}
}

void StatusFeint::onExecuteAbility(Ability* ability)
{
	if (hasExpired())
		return;
	Status::onExecuteAbility(ability);
	
	Unit* source = effect->getSource();
	Unit* caster = ability->getSource();
	if (source && caster && source->getGrid() != caster->getGrid()) // Not same group
	{
		if (source->isAvailable() && Targeter::canReach(source, caster, effect->getBattle(), 1, 1))
		{
			Ability* response = NULL;
			// Execute the ability
			response = Ability::getAbility(FEINT);
			response->action(ability, source, effect->getBattle());

			onKill();
		}
	}
}

void StatusQuickNock::onExecuteAbility(Ability* ability)
{
	if (hasExpired())
		return;
	Status::onExecuteAbility(ability);
	
	Unit* source = effect->getSource();
	Unit* caster = ability->getSource();
	if (source && caster && source->getGrid() != caster->getGrid()) // Not same group
	{
		if (source->isAvailable() && Targeter::canReach(source, caster, effect->getBattle(), 1, 2))
		{
			Ability* response = NULL;
			// Execute the ability
			response = Ability::getAbility(QUICK_NOCK);
			response->action(ability, source, effect->getBattle());

			onKill();
		}
	}
}

void StatusConfuseTrap::onPreReactHit(EventAttack* evt)
{
	if (hasExpired())
		return;
	Status::onPreReactHit(evt);
	
	AbilityType atkType = evt->attack;
	if (atkType == ABILITY_ATTACK_MELEE && !evt->indirect && evt->ref)
	{
		Damage* damage = new Damage(NULL, target, useStacks() * AMOUNT, DAMAGE_MEDIUM, DAMAGE_EARTH);
	
		Event* log = new EventCauseDamage(this->effect, grouplist->getSubname(), Event::AUTO_HIT_CHANCE, damage, true);
		log->apply(effect->getBattle());

		if (damage->final > 0)
		{
			Effect* neffect = new Effect(effect->getSource(), effect->getBattle(), effect->getName(), evt->ref->getSource());
			Status* status = new StatusConfusion(neffect, evt->ref->getSource(), 1);
		
			Event* log = new EventCauseStatus(effect, effect->getName(), Event::DEBUFF_HIT_CHANCE, status);
			log->apply(effect->getBattle());

			if (log->success)
				evt->chance = -1; // Auto fail if stun succeeded

			neffect->applyEffect();
		}
		onKill();
	}
}

void StatusCharmTrap::onPreReactHit(EventAttack* evt)
{
	if (hasExpired())
		return;
	Status::onPreReactHit(evt);
	
	AbilityType atkType = evt->attack;
	if (atkType == ABILITY_ATTACK_MELEE && !evt->indirect && evt->ref)
	{
		Damage* damage = new Damage(NULL, target, useStacks() * AMOUNT, DAMAGE_MEDIUM, DAMAGE_EARTH);
	
		Event* log = new EventCauseDamage(this->effect, grouplist->getSubname(), Event::AUTO_HIT_CHANCE, damage, true);
		log->apply(effect->getBattle());

		if (damage->final > 0)
		{
			Effect* neffect = new Effect(effect->getSource(), effect->getBattle(), effect->getName(), evt->ref->getSource());
			Status* status = new StatusCharm(neffect, evt->ref->getSource(), 1);
		
			Event* log = new EventCauseStatus(effect, effect->getName(), Event::DEBUFF_HIT_CHANCE, status);
			log->apply(effect->getBattle());

			if (log->success)
				evt->chance = -1; // Auto fail if stun succeeded

			neffect->applyEffect();
		}
		onKill();
	}
}

void Effect::print(ostream& out) const
{
}

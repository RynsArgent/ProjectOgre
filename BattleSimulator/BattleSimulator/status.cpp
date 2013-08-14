#include "status.h"

#include <cstdlib>
#include "ability.h"
#include "damage.h"
#include "target.h"
#include "battle.h"

bool Status::hasExpired() const {
	return clean || (timed && timer <= 0);
}

void Status::onSpawn()
{
}

void Status::onRound() 
{	
	if (timed && timer > 0)
		--timer;
}

void Status::onKill() 
{
	clean = true;
    subname = "-----";
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
    Status::onKill();
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
            units = system->getBattle()->getAllyGroup(system->getSource()->getGrid())->allyUnits();
        else if (system->group == TARGET_ALLIES)
            units = system->getBattle()->getEnemyGroup(system->getSource()->getGrid())->allyUnits();

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

	if (caster->getCurrentSkill() == NO_STANDARD_SKILL)
		caster->setCurrentSkill(caster->getBasicSkill());
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
            system->candidates = system->getBattle()->getAllyGroup(system->getSource()->getGrid())->allyUnits();
        else if (system->group == TARGET_ALLIES)
            system->candidates = system->getBattle()->getEnemyGroup(system->getSource()->getGrid())->allyUnits();

		system->method = TARGET_CHARMED;
    }
}

void StatusCharm::onSelectAbility(Unit* caster)
{
    if (hasExpired())
        return;
	Status::onSelectAbility(caster);

	if (caster->getCurrentSkill() == NO_STANDARD_SKILL)
		caster->setCurrentSkill(caster->getBasicSkill());
}

void StatusPoison::onMerge(const StatusMergeResult & mergeResult)
{
	timer += mergeResult.timer;
}

void StatusPoison::applyTimedDamage()
{
	Damage* damage = new Damage(this, target, 10, DAMAGE_EARTH);
    effect->getBattle()->addToActionStack(damage);
	damage->apply();
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

void StatusBlock::onMerge(const StatusMergeResult & mergeResult)
{
	timer = mergeResult.timer;
	amount = mergeResult.val1;
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

void StatusBattleShout::onMerge(const StatusMergeResult & mergeResult)
{
	timer += mergeResult.timer;

	/*
	// The following shows how to implement stackable damage for Battle Shout with resetting timer
	timer = mergeResult.timer;
	int val = target->getCurrentPhysicalAttack();
	val += mergeResult.val1;
	amount += mergeResult.val1;
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
	Status::onKill();
	int val = target->getCurrentPhysicalAttack();
	val -= amount;
	target->setCurrentPhysicalAttack(val);
}

/*
bool StatusAttackResponse::hasExpired() const
{
	return Status::hasExpired() || (limited && amount <= 0);
}

void StatusAttackResponse::applyAbility(Unit* caster, Battle* battle)
{
	if (hasExpired())
		return;
	Ability* ability = NULL;
	ability = getAbility(skill);
	ability->action(NULL, caster, battle);
	if (limited)
		--amount;
}
*/
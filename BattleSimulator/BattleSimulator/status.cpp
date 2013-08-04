#include "status.h"

#include "damage.h"
#include "target.h"

bool Status::hasExpired() const {
	return (timed && timer <= 0);
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

void Status::onPreTarget(Targeter* system)
{
}

void Status::onPostTarget(Targeter* system)
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
		name = "-----";
}

void StatusPoison::applyTimedDamage()
{
	Damage* damage = new Damage(this, NULL, target, 10, DAMAGE_EARTH);
	damage->apply();
	delete damage;
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

	if (hasExpired())
		name = "-----";
}

void StatusTaunt::addToPriorityList(Targeter* system) const
{
	for (int i = 0; i < system->candidates.size(); ++i) {
		if (focus == system->candidates[i])
			system->priorities.push_back(i);
	}
}

void StatusTaunt::onPreTarget(Targeter* system)
{
	if (hasExpired())
		return;
	Status::onPreTarget(system);
	
	addToPriorityList(system);

	if (hasExpired())
		name = "-----";
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
#include "status.h"

#include "damage.h"

void Status::init()
{
}

void Status::processRound() 
{	
	if (timed && timer > 0)
		--timer;
}

bool Status::hasExpired() const {
	return (timed && timer <= 0);
}

void Status::end() 
{
	clean = true;
}

bool StatusDamagePrevention::hasExpired() const
{
	return Status::hasExpired() || (limited && amount <= 0);
}

int StatusDamagePrevention::applyDamagePrevention(int appliedDamage, vector<DamageType> appliedTypes)
{ 
	int resultantDamage = appliedDamage;
	int numMatching = findNumMatching(preventedTypes, appliedTypes);
	if (numMatching > 0)
	{
		resultantDamage = appliedDamage - amount;
		if (resultantDamage < 0)
			resultantDamage = 0;

		if (limited)
			amount -= (appliedDamage - resultantDamage);
	}

	if (hasExpired())
		name = "-----";
	return resultantDamage;
}
	
void StatusDamageOverTime::processRound()
{
	Status::processRound();
	applyTimedDamage();
}

void StatusDamageOverTime::applyTimedDamage()
{
	int dividedAmount = amount / damageTypes.size();
	int dividedRemainder = amount % damageTypes.size();
	for (int i = 0; i < damageTypes.size(); ++i)
	{
		if (i < dividedRemainder)
			Damage(this, dividedAmount + 1, damageTypes[i]).apply(target);
		else
			Damage(this, dividedAmount, damageTypes[i]).apply(target);
	}
}

void StatusAttackBonus::init()
{
	int val = target->getCurrentPhysicalAttack();
	val += amount;
	target->setCurrentPhysicalAttack(val);
}

void StatusAttackBonus::end()
{
	Status::end();
	int val = target->getCurrentPhysicalAttack();
	val -= amount;
	target->setCurrentPhysicalAttack(val);
}

bool StatusAttackResponse::hasExpired() const
{
	return Status::hasExpired() || (limited && amount <= 0);
}

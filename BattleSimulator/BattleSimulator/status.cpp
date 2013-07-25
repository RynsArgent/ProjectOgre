#include "status.h"

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
	return Status::hasExpired() || amount <= 0;
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
			applyDamage(target, dividedAmount + 1, damageTypes[i]);
		else
			applyDamage(target, dividedAmount, damageTypes[i]);
	}
}

void StatusAttackBonus::init()
{
	target->addCurrentPhysicalAttack(amount);
	target->addCurrentPhysicalAttack(amount);
}

void StatusAttackBonus::end()
{
	Status::end();
	target->addCurrentPhysicalAttack(-amount);
	target->addCurrentPhysicalAttack(-amount);
}
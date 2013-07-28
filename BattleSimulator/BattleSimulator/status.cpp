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

int Damage::apply(Unit* target)
{
	int totalDamage = amount;
	switch (type)
	{
	case DAMAGE_PHYSICAL:
		totalDamage -= target->getCurrentPhysicalDefense();
		break;
	case DAMAGE_FIRE:
		totalDamage -= target->getCurrentFireDefense();
		break;
	case DAMAGE_WATER:
		totalDamage -= target->getCurrentWaterDefense();
		break;
	case DAMAGE_EARTH:
		totalDamage -= target->getCurrentEarthDefense();
		break;
	case DAMAGE_ICE:
		totalDamage -= target->getCurrentIceDefense();
		break;
	case DAMAGE_LIGHTNING:
		totalDamage -= target->getCurrentLightningDefense();
		break;
	case DAMAGE_NONE:
		break;
	}
	
	for (int i = 0; i < target->getCurrentStatus().size(); ++i)
	{
		Status* status = target->getCurrentStatus()[i];

		vector<DamageType> types;
		types.push_back(type);
		if (status->getType() == STATUS_DAMAGE_PREVENTION)
			totalDamage = dynamic_cast<StatusDamagePrevention*>(status)->applyDamagePrevention(totalDamage, types);
	}
	
	if (totalDamage < 0)
		totalDamage = 0;
	int val = target->getCurrentHealth();
	val -= totalDamage;
	target->setCurrentHealth(val);

	return totalDamage;
}

int findNumMatching(const vector<DamageType> & types1, const vector<DamageType> & types2)
{
	vector<DamageType> res(types1.size() + types2.size());
	vector<DamageType>::iterator it;
	it = set_union(types1.begin(), types1.end(), types2.begin(), types2.end(), res.begin());
	res.resize(it - res.begin());
	return res.size();
} 

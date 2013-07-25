#include "unit.h"

#include <algorithm>

void Unit::carryOverCharacterStatistics()
{
	currentHealth = character->getBaseHealth();
	maxHealth = character->getBaseHealth();

	currentSpeed = character->getBaseSpeed();
	currentInitiative = 0;
	
	currentPhysicalAttack = character->getBasePhysicalAttack();
	currentMagicAttack = character->getBaseMagicAttack();
	
	currentMeleeDefense = character->getBaseMeleeDefense();
	currentRangeDefense = character->getBaseRangeDefense();
	currentFireDefense = character->getBaseFireDefense();
	currentWaterDefense = character->getBaseWaterDefense();
	currentEarthDefense = character->getBaseEarthDefense();
	currentIceDefense = character->getBaseIceDefense();
	currentLightningDefense = character->getBaseLightningDefense();

	currentSize = character->getBaseSize();

	currentElement = character->getFavoredElement();
}

Unit::Unit(Character* character, int gid, int x, int y)
	: character(character), backSkill(NO_SKILL), midSkill(NO_SKILL), frontSkill(NO_SKILL), 
	gid(gid), gridX(x), gridY(y), currentEffects(), currentStatus()
{
	carryOverCharacterStatistics();
}

void Unit::processEffects()
{
	for (int i = 0; i < currentEffects.size(); ++i)
	{
		Effect* effect = currentEffects[i];
		effect->processRound();
	}
}

void Unit::cleanEffects()
{
	int c = 0;
	vector<Effect*> neffects(currentEffects.size());
	for (int i = 0; i < currentEffects.size(); ++i) {
		Effect* effect = currentEffects[i];
		if (!effect->needsCleaning()) {
			neffects[c] = effect;
			++c;
		}
		else delete currentEffects[i];
	}
	neffects.resize(c);
	currentEffects = neffects; 
}

int findNumMatching(const vector<DamageType> & types1, const vector<DamageType> & types2)
{
	vector<DamageType> res(types1.size() + types2.size());
	vector<DamageType>::iterator it;
	it = set_union(types1.begin(), types1.end(), types2.begin(), types2.end(), res.begin());
	res.resize(it - res.begin());
	return res.size();
} 

int applyDamage(Unit* caster, Unit* target, DamageType type)
{
	int totalDamage = 0;

	switch (type)
	{
	case DAMAGE_MELEE:
		totalDamage += caster->currentPhysicalAttack;
		totalDamage -= target->currentMeleeDefense;
		break;
	case DAMAGE_RANGE:
		totalDamage += caster->currentPhysicalAttack;
		totalDamage -= target->currentRangeDefense;
		break;
	case DAMAGE_FIRE:
		totalDamage += caster->currentMagicAttack;
		totalDamage -= target->currentFireDefense;
		break;
	case DAMAGE_WATER:
		totalDamage += caster->currentMagicAttack;
		totalDamage -= target->currentWaterDefense;
		break;
	case DAMAGE_EARTH:
		totalDamage += caster->currentMagicAttack;
		totalDamage -= target->currentEarthDefense;
		break;
	case DAMAGE_ICE:
		totalDamage += caster->currentMagicAttack;
		totalDamage -= target->currentIceDefense;
		break;
	case DAMAGE_LIGHTNING:
		totalDamage += caster->currentMagicAttack;
		totalDamage -= target->currentLightningDefense;
		break;
	case DAMAGE_HEALING:
		totalDamage -= caster->currentMagicAttack;
		break;
	case DAMAGE_NONE:
		break;
	}
	
	for (int i = 0; i < target->currentStatus.size(); ++i)
	{
		Status* status = target->currentStatus[i];

		vector<DamageType> types;
		types.push_back(type);
		if (status->getType() == STATUS_DAMAGE_PREVENTION)
			totalDamage = dynamic_cast<StatusDamagePrevention*>(status)->applyDamagePrevention(totalDamage, types);
	}

	target->currentHealth -= totalDamage;
	return totalDamage;
}

int applyDamage(Unit* target, int damage, DamageType type)
{
	int totalDamage = damage;
	switch (type)
	{
	case DAMAGE_MELEE:
		totalDamage -= target->currentMeleeDefense;
		break;
	case DAMAGE_RANGE:
		totalDamage -= target->currentRangeDefense;
		break;
	case DAMAGE_FIRE:
		totalDamage -= target->currentFireDefense;
		break;
	case DAMAGE_WATER:
		totalDamage -= target->currentWaterDefense;
		break;
	case DAMAGE_EARTH:
		totalDamage -= target->currentEarthDefense;
		break;
	case DAMAGE_ICE:
		totalDamage -= target->currentIceDefense;
		break;
	case DAMAGE_LIGHTNING:
		totalDamage -= target->currentLightningDefense;
		break;
	case DAMAGE_HEALING:
		totalDamage = -totalDamage;
		break;
	case DAMAGE_NONE:
		break;
	}
	
	for (int i = 0; i < target->currentStatus.size(); ++i)
	{
		Status* status = target->currentStatus[i];

		vector<DamageType> types;
		types.push_back(type);
		if (status->getType() == STATUS_DAMAGE_PREVENTION)
			totalDamage = dynamic_cast<StatusDamagePrevention*>(status)->applyDamagePrevention(totalDamage, types);
	}

	target->currentHealth -= totalDamage;
	return totalDamage;
}

void Unit::print() const
{
	cout << getName() << " - HP: " << currentHealth << " ";
	/*
	cout << " Effects: ";
	for (int i = 0; i < currentEffects.size(); ++i)
		cout << currentEffects[i]->getName() << " ";
	*/
	cout << " Status: ";
	for (int i = 0; i < currentStatus.size(); ++i)
		cout << currentStatus[i]->getName() << " ";
	cout << endl;
}
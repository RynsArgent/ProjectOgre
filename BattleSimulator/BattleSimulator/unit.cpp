#include "unit.h"

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
	: character(character), backSkill(NO_SKILL), midSkill(NO_SKILL), frontSkill(NO_SKILL), gid(gid), gridX(x), gridY(y)
{
	carryOverCharacterStatistics();
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
	target->currentHealth -= totalDamage;
	return totalDamage;
}
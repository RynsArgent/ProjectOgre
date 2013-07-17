#include "unit.h"

void Unit::carryOverCharacterStatistics()
{
	currentHealth = character->getBaseHealth();
	maxHealth = character->getBaseHealth();

	currentSpeed = character->getBaseSpeed();

	currentDamage = character->getBaseDamage();

	currentPhysicalDefense = character->getBasePhysicalDefense();
	currentFireDefense = character->getBaseFireDefense();
	currentWaterDefense = character->getBaseWaterDefense();
	currentEarthDefense = character->getBaseEarthDefense();
	currentIceDefense = character->getBaseIceDefense();
	currentLightningDefense = character->getBaseLightningDefense();

	currentSize = character->getBaseSize();

	currentElement = character->getFavoredElement();
}

Unit::Unit(Character* character, int x, int y)
	: character(character), currentX(x), currentY(y)
{
	carryOverCharacterStatistics();
}
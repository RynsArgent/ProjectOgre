#pragma once

#include "character.h"

class Unit
{
private:
	Character *character;

	int currentHealth;
	int maxHealth;

	int currentSpeed;

	int currentDamage;
	
	int currentPhysicalDefense;
	int currentFireDefense;
	int currentWaterDefense;
	int currentEarthDefense;
	int currentIceDefense;
	int currentLightningDefense;
	
	int currentSize;
	
	ElementType currentElement;
	
	int currentX;
	int currentY;

	void carryOverCharacterStatistics();
public:
	Unit(Character* character, int x, int y);
};
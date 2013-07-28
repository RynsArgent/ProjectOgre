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
	
	currentPhysicalDefense = character->getBasePhysicalDefense();
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
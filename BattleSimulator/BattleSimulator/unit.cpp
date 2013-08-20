#include "unit.h"

#include <algorithm>
#include "status.h"

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
	: character(character), 
	basicSkill(NO_STANDARD_SKILL), backSkill(NO_STANDARD_SKILL), midSkill(NO_STANDARD_SKILL), frontSkill(NO_STANDARD_SKILL), 
	currentSkill(NO_STANDARD_SKILL), currentTier(0),
	gid(gid), formX(x), formY(y), gridX(x), gridY(y), currentEffects(), currentStatus()
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
		if (!effect->isExpired()) {
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
	cout << getName();
	if (currentHealth <= 0)
	{
		cout << " *dead*";
	}
	else
	{
		cout << " - HP: " << currentHealth << "/" << maxHealth;
		/*
		cout << " Effects: ";
		for (int i = 0; i < currentEffects.size(); ++i)
			cout << currentEffects[i]->getName() << " ";
		*/
		cout << " Status: ";
		for (int i = 0; i < currentStatus.size(); ++i)
			cout << currentStatus[i]->getSubname() << currentStatus[i]->getTimer() << " ";
	}
	cout << endl;
}
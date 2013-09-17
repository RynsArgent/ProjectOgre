#include "character.h"

Character::Character(const string & name, int health, int physicalAttack, int magicAttack, int speed, 
	int physicalDefense, int fireDefense, int waterDefense, 
	int earthDefense, int iceDefense, int lightningDefense,
	int size, ElementType favoredElement, int upkeep)
	: name(name), baseHealth(health), basePhysicalAttack(physicalAttack), baseMagicAttack(magicAttack), baseSpeed(speed),
	basePhysicalDefense(physicalDefense), baseFireDefense(fireDefense), baseWaterDefense(waterDefense),
	baseEarthDefense(earthDefense), baseIceDefense(iceDefense), baseLightningDefense(lightningDefense),
	baseSize(size), favoredElement(favoredElement), backSkills(), midSkills(), frontSkills(), basicSkills(),
	backSkillIndex(0), midSkillIndex(0), frontSkillIndex(0), basicSkillIndex(0), baseUpkeep(upkeep)
{
}

Fighter::Fighter(const string & name, ElementType favoredElement, int upkeep)
	: Character(name, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
		BASE_PHYSICAL_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_SIZE, favoredElement, upkeep)
{
	init();
}

void Fighter::init() 
{
	backSkills.push_back(TAUNT);
	backSkills.push_back(BATTLE_SHOUT);
	midSkills.push_back(STRIKE);
	midSkills.push_back(BLOCK);
	frontSkills.push_back(HUNDRED_BLADES);
	frontSkills.push_back(BLOCK);
	basicSkills.push_back(STRIKE);
}

Scout::Scout(const string & name, ElementType favoredElement, int upkeep)
	: Character(name, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
		BASE_PHYSICAL_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_SIZE, favoredElement, upkeep)
{
	init();
}

void Scout::init() 
{
	backSkills.push_back(SHOOT);
	backSkills.push_back(HASTE);
	midSkills.push_back(SHOOT);
	midSkills.push_back(SCOPE);
	frontSkills.push_back(TANGLE_TRAP);
	frontSkills.push_back(SCOPE);
	basicSkills.push_back(SHOOT);
}

Acolyte::Acolyte(const string & name, ElementType favoredElement, int upkeep)
	: Character(name, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
		BASE_PHYSICAL_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_SIZE, favoredElement, upkeep)
{
	init();
}

void Acolyte::init() 
{
	backSkills.push_back(HEAL);
	backSkills.push_back(CLEANSE);
	midSkills.push_back(HEAL);
	midSkills.push_back(REJUVENATE);
	frontSkills.push_back(HEAL);
	frontSkills.push_back(BLIND);
	basicSkills.push_back(NO_STANDARD_SKILL);
}

Mage::Mage(const string & name, ElementType favoredElement, int upkeep)
	: Character(name, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
		BASE_PHYSICAL_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_SIZE, favoredElement, upkeep)
{
	init();
}

void Mage::init() 
{
	backSkills.push_back(retrieveSkill(SPELL1, favoredElement));
	backSkills.push_back(BARRIER);
	midSkills.push_back(retrieveSkill(SPELL1, favoredElement));
	midSkills.push_back(POLYMORPH);
	frontSkills.push_back(retrieveSkill(SPELL1, favoredElement));
	frontSkills.push_back(BARRIER);
	basicSkills.push_back(NO_STANDARD_SKILL);
}
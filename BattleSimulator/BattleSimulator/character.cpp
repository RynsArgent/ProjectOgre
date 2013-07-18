#include "character.h"

Character::Character(const string & name, int health, int physicalAttack, int magicAttack, int speed, 
	int meleeDefense, int rangeDefense, int fireDefense, int waterDefense, 
	int earthDefense, int iceDefense, int lightningDefense,
	int size, ElementType favoredElement, int upkeep)
	: name(name), baseHealth(health), basePhysicalAttack(physicalAttack), baseMagicAttack(magicAttack), baseSpeed(speed),
	baseMeleeDefense(meleeDefense), baseRangeDefense(rangeDefense), baseFireDefense(fireDefense), baseWaterDefense(waterDefense),
	baseEarthDefense(earthDefense), baseIceDefense(iceDefense), baseLightningDefense(lightningDefense),
	baseSize(size), favoredElement(favoredElement), backSkills(), midSkills(), frontSkills(), baseUpkeep(upkeep)
{
}

Fighter::Fighter(const string & name, ElementType favoredElement, int upkeep)
	: Character(name, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
		BASE_MELEE_DEFENSE, BASE_RANGE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_SIZE, favoredElement, upkeep)
{
	backSkills.push_back(TAUNT);
	backSkills.push_back(BATTLE_SHOUT);
	midSkills.push_back(STRIKE);
	midSkills.push_back(BLOCK);
	frontSkills.push_back(HUNDRED_BLADES);
	frontSkills.push_back(BLOCK);
}

Scout::Scout(const string & name, ElementType favoredElement, int upkeep)
	: Character(name, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
		BASE_MELEE_DEFENSE, BASE_RANGE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_SIZE, favoredElement, upkeep)
{
	backSkills.push_back(SHOOT);
	backSkills.push_back(FOREWARN);
	midSkills.push_back(SHOOT);
	midSkills.push_back(SCOPE);
	frontSkills.push_back(TANGLE_TRAP);
	frontSkills.push_back(SCOPE);
}

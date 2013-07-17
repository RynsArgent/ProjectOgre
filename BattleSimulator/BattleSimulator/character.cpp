#include "character.h"

Character::Character(const string & name, int health, int damage, int speed, 
	int physicalDefense, int fireDefense, int waterDefense, 
	int earthDefense, int iceDefense, int lightningDefense,
	int size, ElementType favoredElement, int upkeep)
	: name(name), baseHealth(health), baseDamage(damage), baseSpeed(speed),
	basePhysicalDefense(physicalDefense), baseFireDefense(fireDefense), baseWaterDefense(waterDefense),
	baseEarthDefense(earthDefense), baseIceDefense(iceDefense), baseLightningDefense(lightningDefense),
	baseSize(size), favoredElement(favoredElement), baseUpkeep(upkeep)
{

}

Fighter::Fighter(const string & name, ElementType favoredElement, int upkeep)
	: Character(name, BASE_HEALTH, BASE_DAMAGE, BASE_SPEED,
		BASE_PHYSICAL_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_SIZE, favoredElement, upkeep)
{
}

Scout::Scout(const string & name, ElementType favoredElement, int upkeep)
	: Character(name, BASE_HEALTH, BASE_DAMAGE, BASE_SPEED,
		BASE_PHYSICAL_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_SIZE, favoredElement, upkeep)
{
}

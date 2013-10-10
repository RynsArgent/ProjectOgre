#include "job.h"

#include "character.h"

Job* Job::getJob(JobType job, Character* target)
{
	switch (job)
	{   
	case JOB_NONE:
		return NULL;
	case JOB_FIGHTER:
		return new Fighter(target);
	case JOB_SCOUT:
		return new Scout(target);
	case JOB_ACOLYTE:
		return new Acolyte(target);
	case JOB_MAGE:
		return new Mage(target);
	case JOB_WARRIOR:
		return new Warrior(target);
    case JOB_KNIGHT:
            return new Knight(target);
	default:
		return NULL;
	}
}

Job::Job(Character* character, JobType type, int health, int physicalAttack, int magicAttack, int speed, 
		int physicalDefense, int arcaneDefense, int fireDefense, int waterDefense,
		int earthDefense, int iceDefense, int lightningDefense,
		int upkeep)
	: character(character), type(type), baseHealth(health), basePhysicalAttack(physicalAttack), baseMagicAttack(magicAttack), baseSpeed(speed),
	basePhysicalDefense(physicalDefense), baseArcaneDefense(arcaneDefense), baseFireDefense(fireDefense), baseWaterDefense(waterDefense),
	baseEarthDefense(earthDefense), baseIceDefense(iceDefense), baseLightningDefense(lightningDefense),
	backSkills(), midSkills(), frontSkills(), basicSkills(),
	backSkillIndex(0), midSkillIndex(0), frontSkillIndex(0), basicSkillIndex(0), baseUpkeep(upkeep)
{
}

void Job::init() 
{
	backSkills.clear();
	midSkills.clear();
	frontSkills.clear();
	basicSkills.clear();
}

Fighter::Fighter(Character* character)
	: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
		BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_UPKEEP)
{
	init();
}

void Fighter::init() 
{
	Job::init();
	backSkills.push_back(BATTLE_SHOUT);
	backSkills.push_back(TAUNT);
	midSkills.push_back(STRIKE);
	midSkills.push_back(BLOCK);
	frontSkills.push_back(HUNDRED_BLADES);
	frontSkills.push_back(BLOCK);
	basicSkills.push_back(STRIKE);
}

Scout::Scout(Character* character)
	: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
		BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_UPKEEP)
{
	init();
}

void Scout::init() 
{
	Job::init();
	backSkills.push_back(SHOOT);
	backSkills.push_back(SCOPE);
	midSkills.push_back(SHOOT);
	midSkills.push_back(TANGLE_TRAP);
	frontSkills.push_back(SHOOT);
	frontSkills.push_back(TANGLE_TRAP);
	basicSkills.push_back(SHOOT);
}

Acolyte::Acolyte(Character* character)
	: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
		BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_UPKEEP)
{
	init();
}

void Acolyte::init() 
{
	Job::init();
	backSkills.push_back(HEAL);
	backSkills.push_back(REGENERATION);
	midSkills.push_back(HEAL);
	midSkills.push_back(REGENERATION);
	frontSkills.push_back(HEAL);
	frontSkills.push_back(BLIND);
	basicSkills.push_back(MEND);
}

Mage::Mage(Character* character)
	: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
		BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_UPKEEP)
{
	init();
}

void Mage::init() 
{
	Job::init();
	backSkills.push_back(retrieveSkill(SPELL1, character->getFavoredElement()));
	backSkills.push_back(HASTE);
	midSkills.push_back(retrieveSkill(SPELL1, character->getFavoredElement()));
	midSkills.push_back(HASTE);
	frontSkills.push_back(retrieveSkill(SPELL1, character->getFavoredElement()));
	frontSkills.push_back(BARRIER);
	basicSkills.push_back(ARCANE_BOLT);
}

Warrior::Warrior(Character* character)
	: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
		BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
		BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
		BASE_UPKEEP)
{
	init();
}

void Warrior::init() 
{
	Job::init();
	backSkills.push_back(STRIKE);
	backSkills.push_back(BATTLE_SHOUT);
	midSkills.push_back(HUNDRED_BLADES);
	midSkills.push_back(PROVOKE);
	frontSkills.push_back(SLASH);
	frontSkills.push_back(PROVOKE);
	basicSkills.push_back(STRIKE);
}

Knight::Knight(Character* character)
: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
      BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
      BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
      BASE_UPKEEP)
{
	init();
}

void Knight::init()
{
	Job::init();
	backSkills.push_back(CHALLENGE);
	backSkills.push_back(RALLY);
	midSkills.push_back(CHALLENGE);
	midSkills.push_back(STRIKE);
	frontSkills.push_back(CHARGE);
	frontSkills.push_back(BLOCK);
	basicSkills.push_back(STRIKE);
}
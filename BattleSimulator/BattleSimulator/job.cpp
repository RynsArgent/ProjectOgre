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
    case JOB_BARBARIAN:
            return new Barbarian(target);
    case JOB_ROGUE:
            return new Rogue(target);
    case JOB_HUNTER:
            return new Hunter(target);
    case JOB_ARCHER:
            return new Archer(target);
    case JOB_BARD:
            return new Bard(target);
    case JOB_PRIEST:
            return new Priest(target);
    case JOB_PUGILIST:
            return new Pugilist(target);
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
	midSkills.push_back(SCOPE);
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
	frontSkills.push_back(FLASH);
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
	backSkills.push_back(POLYMORPH);
	midSkills.push_back(retrieveSkill(SPELL1, character->getFavoredElement()));
	midSkills.push_back(POLYMORPH);
	frontSkills.push_back(retrieveSkill(SPELL1, character->getFavoredElement()));
	frontSkills.push_back(BLINK);
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

Barbarian::Barbarian(Character* character)
: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
      BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
      BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
      BASE_UPKEEP)
{
	init();
}

void Barbarian::init()
{
	Job::init(); 
	backSkills.push_back(STRIKE);
	backSkills.push_back(DEMORALIZING_SHOUT);
	midSkills.push_back(HUNDRED_BLADES);
	midSkills.push_back(DEMORALIZING_SHOUT);
	frontSkills.push_back(FLURRY);
	frontSkills.push_back(POWER_ATTACK);
	basicSkills.push_back(STRIKE);
}

Rogue::Rogue(Character* character)
: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
      BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
      BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
      BASE_UPKEEP)
{
	init();
}

void Rogue::init()
{
	Job::init();
	backSkills.push_back(FEINT);
	backSkills.push_back(VENOMOUS_STRIKE);
	midSkills.push_back(FEINT);
	midSkills.push_back(VENOMOUS_STRIKE);
	frontSkills.push_back(PIERCETHROUGH);
	frontSkills.push_back(LASSO);
	basicSkills.push_back(STRIKE);
}

Hunter::Hunter(Character* character)
: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
      BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
      BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
      BASE_UPKEEP)
{
	init();
}

void Hunter::init()
{
	Job::init();
	backSkills.push_back(CHARM_TRAP);
	backSkills.push_back(HUNTERS_MARK);
	midSkills.push_back(TRANQUILIZING_SHOT);
	midSkills.push_back(HUNTERS_MARK);
	frontSkills.push_back(CRIPPLING_SHOT);
	frontSkills.push_back(CONFUSE_TRAP);
	basicSkills.push_back(SHOOT);
}

Archer::Archer(Character* character)
: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
      BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
      BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
      BASE_UPKEEP)
{
	init();
}

void Archer::init()
{
	Job::init();
	backSkills.push_back(SNIPE);
	backSkills.push_back(VOLLEY);
	midSkills.push_back(RAPID_SHOT);
	midSkills.push_back(VOLLEY);
	frontSkills.push_back(SHOOT);
	frontSkills.push_back(QUICK_NOCK);
	basicSkills.push_back(SHOOT);
}

Bard::Bard(Character* character)
: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
      BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
      BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
      BASE_UPKEEP)
{
	init();
}

void Bard::init()
{
	Job::init();
	backSkills.push_back(BALLAD_OF_HEROES);
	backSkills.push_back(REQUIEM_OF_WAR);
	midSkills.push_back(BALLAD_OF_HEROES);
	midSkills.push_back(HASTE);
	frontSkills.push_back(REFORMATION);
	frontSkills.push_back(LULLABY);
	basicSkills.push_back(MEND);
}

Priest::Priest(Character* character)
: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
      BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
      BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
      BASE_UPKEEP)
{
	init();
}

void Priest::init()
{
	Job::init();
	backSkills.push_back(GREATER_HEAL);
	backSkills.push_back(RAISE);
	midSkills.push_back(GREATER_HEAL);
	midSkills.push_back(DISPEL);
	frontSkills.push_back(HEAL);
	frontSkills.push_back(BANISH);
	basicSkills.push_back(MEND);
}

Pugilist::Pugilist(Character* character)
: Job(character, JOB_TYPE, BASE_HEALTH, BASE_PHYSICAL_ATTACK, BASE_MAGIC_ATTACK, BASE_SPEED,
      BASE_PHYSICAL_DEFENSE, BASE_ARCANE_DEFENSE, BASE_FIRE_DEFENSE, BASE_WATER_DEFENSE,
      BASE_EARTH_DEFENSE, BASE_ICE_DEFENSE, BASE_LIGHTNING_DEFENSE,
      BASE_UPKEEP)
{
	init();
}

void Pugilist::init()
{
	Job::init();
	backSkills.push_back(LEG_SWEEP);
	backSkills.push_back(ROUNDHOUSE);
	midSkills.push_back(LIGHTNING_KICKS);
	midSkills.push_back(ROUNDHOUSE);
	frontSkills.push_back(LIGHTNING_KICKS);
	frontSkills.push_back(HURRICANE_KICK);
	basicSkills.push_back(MEND);
}


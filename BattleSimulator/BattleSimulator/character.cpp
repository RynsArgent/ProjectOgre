#include "character.h"

#include "job.h"

void Character::carryOverJobStatistics()
{
	baseHealth = job->getBaseHealth();
	basePhysicalAttack = job->getBasePhysicalAttack();
	baseMagicAttack = job->getBaseMagicAttack();
	baseSpeed = job->getBaseSpeed();
	basePhysicalDefense = job->getBasePhysicalDefense();
	baseArcaneDefense = job->getBaseArcaneDefense();
	baseFireDefense = job->getBaseFireDefense();
	baseWaterDefense = job->getBaseWaterDefense();
	baseEarthDefense = job->getBaseEarthDefense();
	baseIceDefense = job->getBaseIceDefense();
	baseLightningDefense = job->getBaseLightningDefense();
	
	backSkills = job->getBackSkills();
	midSkills = job->getMidSkills();
	frontSkills = job->getFrontSkills();
	basicSkills = job->getBasicSkills();
	
	int backSkillIndex = job->getBackSkillIndex();
	int midSkillIndex = job->getMidSkillIndex();
	int frontSkillIndex = job->getFrontSkillIndex();
	int basicSkillIndex = job->getBasicSkillIndex();
}

Character::Character(const string & name, JobType jobType, ElementType favoredElement, 
	int health, int physicalAttack, int magicAttack, int speed, 
	int physicalDefense, int arcaneDefense, int fireDefense, int waterDefense,
	int earthDefense, int iceDefense, int lightningDefense,
	int size, int upkeep)
	: name(name), job(NULL), baseHealth(health), basePhysicalAttack(physicalAttack), baseMagicAttack(magicAttack), baseSpeed(speed),
	basePhysicalDefense(physicalDefense), baseArcaneDefense(arcaneDefense), baseFireDefense(fireDefense), baseWaterDefense(waterDefense),
	baseEarthDefense(earthDefense), baseIceDefense(iceDefense), baseLightningDefense(lightningDefense),
	baseSize(size), favoredElement(favoredElement), backSkills(), midSkills(), frontSkills(), basicSkills(),
	backSkillIndex(0), midSkillIndex(0), frontSkillIndex(0), basicSkillIndex(0), baseUpkeep(upkeep)
{
	setJob(jobType);
}

void Character::setJob(JobType jobType)
{
	if (job != NULL)
		delete job;
	job = Job::getJob(jobType, this);
	updateJobInfo();
}

void Character::updateJobInfo()
{
	job->init();
	carryOverJobStatistics();
}

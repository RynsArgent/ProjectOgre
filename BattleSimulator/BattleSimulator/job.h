#ifndef __JOB_H__
#define __JOB_H__

#include "pch.h"

#include <vector>
#include <string>

using namespace std;

// To add a job: 
// -go to pch.h and add the job to the JobType enum
// -go to pch.cpp and update the toStringJob
// -go to job.h and create the new derived class for the Job
// -go to job.cpp and add to getJob at the top of the file the new enum
// -go to job.cpp and implement the constructor and init functions
// -go to renderer.cpp near the top of the file and add the new job to the jobs array

class Job
{
protected:
	Character* character;

	JobType type;

	int baseHealth;
	int basePhysicalAttack;
	int baseMagicAttack;
	int baseSpeed;
	
	int basePhysicalDefense;
	int baseArcaneDefense;
	int baseFireDefense;
	int baseWaterDefense;
	int baseEarthDefense;
	int baseIceDefense;
	int baseLightningDefense;
	
	std::vector<Skill> backSkills;
	std::vector<Skill> midSkills;
	std::vector<Skill> frontSkills;
	std::vector<Skill> basicSkills;
	int backSkillIndex;
	int midSkillIndex;
	int frontSkillIndex;
	int basicSkillIndex;

	int baseUpkeep;
public:
	static Job* getJob(JobType jobType, Character* target);
		
	Job(Character* character, JobType type, int health = 0, int physicalAttack = 0, int magicAttack = 0, int speed = 0, 
		int physicalDefense = 0, int arcaneDefense = 0, int fireDefense = 0, int waterDefense = 0,
		int earthDefense = 0, int iceDefense = 0, int lightningDefense = 0,
		int upkeep = 0);

	virtual void init();

	JobType getType() const {
		return type;
	}

	int getBasePhysicalAttack() const {
		return basePhysicalAttack;
	}

	void setBasePhysicalAttack(int physicalAttack) {
		this->basePhysicalAttack = physicalAttack;
	}

	int getBaseMagicAttack() const {
		return baseMagicAttack;
	}

	void setBaseMagicAttack(int magicAttack) {
		this->baseMagicAttack = magicAttack;
	}

	int getBaseEarthDefense() const {
		return baseEarthDefense;
	}

	void setBaseEarthDefense(int baseEarthDefense) {
		this->baseEarthDefense = baseEarthDefense;
	}
    
	int getBaseArcaneDefense() const {
		return baseArcaneDefense;
	}
    
	void setBaseArcaneDefense(int baseArcaneDefense) {
		this->baseArcaneDefense = baseArcaneDefense;
	}
    
	int getBaseFireDefense() const {
		return baseFireDefense;
	}

	void setBaseFireDefense(int baseFireDefense) {
		this->baseFireDefense = baseFireDefense;
	}

	int getBaseHealth() const {
		return baseHealth;
	}

	void setBaseHealth(int baseHealth) {
		this->baseHealth = baseHealth;
	}

	int getBaseIceDefense() const {
		return baseIceDefense;
	}

	void setBaseIceDefense(int baseIceDefense) {
		this->baseIceDefense = baseIceDefense;
	}

	int getBaseLightningDefense() const {
		return baseLightningDefense;
	}

	void setBaseLightningDefense(int baseLightningDefense) {
		this->baseLightningDefense = baseLightningDefense;
	}

	int getBasePhysicalDefense() const {
		return basePhysicalDefense;
	}

	void setBasePhysicalDefense(int basePhysicalDefense) {
		this->basePhysicalDefense = basePhysicalDefense;
	}

	int getBaseSpeed() const {
		return baseSpeed;
	}

	void setBaseSpeed(int baseSpeed) {
		this->baseSpeed = baseSpeed;
	}

	int getBaseUpkeep() const {
		return baseUpkeep;
	}

	void setBaseUpkeep(int baseUpkeep) {
		this->baseUpkeep = baseUpkeep;
	}

	int getBaseWaterDefense() const {
		return baseWaterDefense;
	}

	void setBaseWaterDefense(int baseWaterDefense) {
		this->baseWaterDefense = baseWaterDefense;
	}
	
	vector<Skill> getBackSkills() const {
		return backSkills;
	}
	
	vector<Skill> getMidSkills() const {
		return midSkills;
	}
	
	vector<Skill> getFrontSkills() const {
		return frontSkills;
	}
	
	vector<Skill> getBasicSkills() const {
		return basicSkills;
	}

	Skill getBackSkill() const {
		return backSkills[backSkillIndex];
	}
	
	Skill getMidSkill() const {
		return midSkills[midSkillIndex];
	}
	
	Skill getFrontSkill() const {
		return frontSkills[frontSkillIndex];
	}
	
	Skill getBasicSkill() const {
		return basicSkills[basicSkillIndex];
	}

	void setBackSkillIndex(int value) {
		backSkillIndex = value;
	}
	
	int getBackSkillIndex() const {
		return backSkillIndex;
	}

	void setMidSkillIndex(int value) {
		midSkillIndex = value;
	}
	
	int getMidSkillIndex() const {
		return midSkillIndex;
	}

	void setFrontSkillIndex(int value) {
		frontSkillIndex = value;
	}
	
	int getFrontSkillIndex() const {
		return frontSkillIndex;
	}

	void setBasicSkillIndex(int value) {
		basicSkillIndex = value;
	}
	
	int getBasicSkillIndex() const {
		return basicSkillIndex;
	}

	virtual ~Job() {}
};

class Fighter : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_FIGHTER;

	const static int BASE_HEALTH = 120;
	const static int BASE_PHYSICAL_ATTACK = 30;
	const static int BASE_MAGIC_ATTACK = 10;
	const static int BASE_SPEED = 3;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;

	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Fighter(Character* character);
	virtual void init();
	~Fighter() {}
};

class Scout : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_SCOUT;

	const static int BASE_HEALTH = 100;
	const static int BASE_PHYSICAL_ATTACK = 20;
	const static int BASE_MAGIC_ATTACK = 20;
	const static int BASE_SPEED = 5;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Scout(Character* character);
	virtual void init();
	~Scout() {}
};

class Acolyte : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_ACOLYTE;

	const static int BASE_HEALTH = 80;
	const static int BASE_PHYSICAL_ATTACK = 10;
	const static int BASE_MAGIC_ATTACK = 30;
	const static int BASE_SPEED = 1;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Acolyte(Character* character);
	virtual void init();
	~Acolyte() {}
};

class Mage : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_MAGE;

	const static int BASE_HEALTH = 80;
	const static int BASE_PHYSICAL_ATTACK = 10;
	const static int BASE_MAGIC_ATTACK = 30;
	const static int BASE_SPEED = 1;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Mage(Character* character);
	virtual void init();
	~Mage() {}
};

class Warrior : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_WARRIOR;

	const static int BASE_HEALTH = 120;
	const static int BASE_PHYSICAL_ATTACK = 30;
	const static int BASE_MAGIC_ATTACK = 10;
	const static int BASE_SPEED = 3;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Warrior(Character* character);
	virtual void init();
	~Warrior() {}
};

class Knight : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_KNIGHT;
    
	const static int BASE_HEALTH = 140;
	const static int BASE_PHYSICAL_ATTACK = 30;
	const static int BASE_MAGIC_ATTACK = 10;
	const static int BASE_SPEED = 3;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Knight(Character* character);
	virtual void init();
	~Knight() {}
};

class Barbarian : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_BARBARIAN;
    
	const static int BASE_HEALTH = 100;
	const static int BASE_PHYSICAL_ATTACK = 50;
	const static int BASE_MAGIC_ATTACK = 10;
	const static int BASE_SPEED = 4;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Barbarian(Character* character);
	virtual void init();
	~Barbarian() {}
};

class Rogue : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_ROGUE;
    
	const static int BASE_HEALTH = 100;
	const static int BASE_PHYSICAL_ATTACK = 40;
	const static int BASE_MAGIC_ATTACK = 10;
	const static int BASE_SPEED = 4;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Rogue(Character* character);
	virtual void init();
	~Rogue() {}
};

class Hunter : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_HUNTER;
    
	const static int BASE_HEALTH = 100;
	const static int BASE_PHYSICAL_ATTACK = 30;
	const static int BASE_MAGIC_ATTACK = 10;
	const static int BASE_SPEED = 4;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Hunter(Character* character);
	virtual void init();
	~Hunter() {}
};

class Archer : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_ARCHER;
    
	const static int BASE_HEALTH = 100;
	const static int BASE_PHYSICAL_ATTACK = 30;
	const static int BASE_MAGIC_ATTACK = 10;
	const static int BASE_SPEED = 4;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Archer(Character* character);
	virtual void init();
	~Archer() {}
};

class Bard : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_BARD;
    
	const static int BASE_HEALTH = 80;
	const static int BASE_PHYSICAL_ATTACK = 10;
	const static int BASE_MAGIC_ATTACK = 20;
	const static int BASE_SPEED = 4;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Bard(Character* character);
	virtual void init();
	~Bard() {}
};

class Priest : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_PRIEST;
    
	const static int BASE_HEALTH = 80;
	const static int BASE_PHYSICAL_ATTACK = 10;
	const static int BASE_MAGIC_ATTACK = 30;
	const static int BASE_SPEED = 1;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Priest(Character* character);
	virtual void init();
	~Priest() {}
};

class Pugilist : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_PUGILIST;
    
	const static int BASE_HEALTH = 80;
	const static int BASE_PHYSICAL_ATTACK = 30;
	const static int BASE_MAGIC_ATTACK = 10;
	const static int BASE_SPEED = 1;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Pugilist(Character* character);
	virtual void init();
	~Pugilist() {}
};

class Wizard : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_WIZARD;
    
	const static int BASE_HEALTH = 60;
	const static int BASE_PHYSICAL_ATTACK = 10;
	const static int BASE_MAGIC_ATTACK = 30;
	const static int BASE_SPEED = 1;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Wizard(Character* character);
	virtual void init();
	~Wizard() {}
};

class Enchanter : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_ENCHANTER;
    
	const static int BASE_HEALTH = 60;
	const static int BASE_PHYSICAL_ATTACK = 10;
	const static int BASE_MAGIC_ATTACK = 30;
	const static int BASE_SPEED = 2;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Enchanter(Character* character);
	virtual void init();
	~Enchanter() {}
};

class Druid : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_DRUID;
    
	const static int BASE_HEALTH = 60;
	const static int BASE_PHYSICAL_ATTACK = 10;
	const static int BASE_MAGIC_ATTACK = 30;
	const static int BASE_SPEED = 2;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	Druid(Character* character);
	virtual void init();
	~Druid() {}
};

class TreeOfLife : public Job
{
protected:
	const static JobType JOB_TYPE = JOB_DRUID;
    
	const static int BASE_HEALTH = 10;
	const static int BASE_PHYSICAL_ATTACK = 10;
	const static int BASE_MAGIC_ATTACK = 30;
	const static int BASE_SPEED = 1;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_ARCANE_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
	const static int BASE_UPKEEP = 0;
public:
	TreeOfLife(Character* character);
	virtual void init();
	~TreeOfLife() {}
};

#endif

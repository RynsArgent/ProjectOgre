#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "pch.h"

#include <vector>
#include <string>

using namespace std;

class Character
{
protected:
	string name;

	int baseHealth;
	int basePhysicalAttack;
	int baseMagicAttack;
	int baseSpeed;
	
	int basePhysicalDefense;
	int baseFireDefense;
	int baseWaterDefense;
	int baseEarthDefense;
	int baseIceDefense;
	int baseLightningDefense;
	
	int baseSize;
	
	ElementType favoredElement;
	
	std::vector<Skill> backSkills;
	std::vector<Skill> midSkills;
	std::vector<Skill> frontSkills;
	Skill basicSkill;
	Skill backSkill;
	Skill midSkill;
	Skill frontSkill;

	int baseUpkeep;
public:
	Character(const string & name = "", int health = 0, int physicalAttack = 0, int magicAttack = 0, int speed = 0, 
		int basePhysicalDefense = 0, int fireDefense = 0, int waterDefense = 0, 
		int earthDefense = 0, int iceDefense = 0, int lightningDefense = 0,
		int size = 0, ElementType favoredElement = ELEMENT_NONE, int upkeep = 0);

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

	int getBaseSize() const {
		return baseSize;
	}

	void setBaseSize(int baseSize) {
		this->baseSize = baseSize;
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

	ElementType getFavoredElement() const {
		return favoredElement;
	}

	void setFavoredElement(ElementType favoredElement) {
		this->favoredElement = favoredElement;
	}

	Skill getBasicSkill() const {
		return basicSkill;
	}
	
	void setBasicSkill(Skill value) {
		basicSkill = value;
	}

	Skill getBackSkill() const {
		return backSkill;
	}
	
	void setBackSkill(Skill value) {
		backSkill = value;
	}

	Skill getMidSkill() const {
		return midSkill;
	}
	
	void setMidSkill(Skill value) {
		midSkill = value;
	}
	
	Skill getFrontSkill() const {
		return frontSkill;
	}

	void setFrontSkill(Skill value) {
		frontSkill = value;
	}

	const string& getName() const {
		return name;
	}

	void setName(const string& name) {
		this->name = name;
	}

	~Character() {}
};

class Fighter : public Character
{
protected:
	const static int BASE_HEALTH = 100;
	const static int BASE_PHYSICAL_ATTACK = 30;
	const static int BASE_MAGIC_ATTACK = 10;
	const static int BASE_SPEED = 3;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;

	const static int BASE_SIZE = 1;
public:
	Fighter(const string & name = "", ElementType favoredElement = ELEMENT_NONE, int upkeep = 0);

	~Fighter() {}
};

class Scout : public Character
{
protected:
	const static int BASE_HEALTH = 80;
	const static int BASE_PHYSICAL_ATTACK = 20;
	const static int BASE_MAGIC_ATTACK = 20;
	const static int BASE_SPEED = 5;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
public:
	Scout(const string & name = "", ElementType favoredElement = ELEMENT_NONE, int upkeep = 0);

	~Scout() {}
};

class Acolyte : public Character
{
protected:
	const static int BASE_HEALTH = 60;
	const static int BASE_PHYSICAL_ATTACK = 10;
	const static int BASE_MAGIC_ATTACK = 30;
	const static int BASE_SPEED = 1;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
public:
	Acolyte(const string & name = "", ElementType favoredElement = ELEMENT_NONE, int upkeep = 0);

	~Acolyte() {}
};

class Mage : public Character
{
protected:
	const static int BASE_HEALTH = 40;
	const static int BASE_PHYSICAL_ATTACK = 10;
	const static int BASE_MAGIC_ATTACK = 30;
	const static int BASE_SPEED = 1;
	
	const static int BASE_PHYSICAL_DEFENSE = 0;
	const static int BASE_FIRE_DEFENSE = 0;
	const static int BASE_WATER_DEFENSE = 0;
	const static int BASE_EARTH_DEFENSE = 0;
	const static int BASE_ICE_DEFENSE = 0;
	const static int BASE_LIGHTNING_DEFENSE = 0;
	
	const static int BASE_SIZE = 1;
public:
	Mage(const string & name = "", ElementType favoredElement = ELEMENT_NONE, int upkeep = 0);

	~Mage() {}
};

#endif

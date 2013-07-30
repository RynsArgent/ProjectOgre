#ifndef __UNIT_H__
#define __UNIT_H__

#include "pch.h"

#include "character.h"

class Unit
{
private:
	Character* character;

	int currentHealth;
	int maxHealth;

	int currentSpeed;
	int currentInitiative;
	
	int currentPhysicalAttack;
	int currentMagicAttack;
	
	int currentPhysicalDefense;
	int currentFireDefense;
	int currentWaterDefense;
	int currentEarthDefense;
	int currentIceDefense;
	int currentLightningDefense;
	
	int currentSize;
	
	ElementType currentElement;
	
	Skill backSkill;
	Skill midSkill;
	Skill frontSkill;

	int gid;
	int gridX;
	int gridY;

	vector<Effect*> currentEffects;
	vector<Status*> currentStatus;

	void carryOverCharacterStatistics();
public:
	Unit(Character* character, int gid, int x, int y);

	void applyCharacterSkillSets() {
		backSkill = character->getBackSkill();
		midSkill = character->getMidSkill();
		frontSkill = character->getFrontSkill();
	}
	
	string getName() const {
		return character->getName();
	}

	int getCurrentPhysicalAttack() const {
		return currentPhysicalAttack;
	}

	void setCurrentPhysicalAttack(int physicalAttack) {
		this->currentPhysicalAttack = physicalAttack;
	}

	int getCurrentMagicAttack() const {
		return currentMagicAttack;
	}

	void setCurrentMagicAttack(int magicAttack) {
		this->currentMagicAttack = magicAttack;
	}

	int getCurrentEarthDefense() const {
		return currentEarthDefense;
	}

	void setCurrentEarthDefense(int currentEarthDefense) {
		this->currentEarthDefense = currentEarthDefense;
	}

	int getCurrentFireDefense() const {
		return currentFireDefense;
	}

	void setCurrentFireDefense(int currentFireDefense) {
		this->currentFireDefense = currentFireDefense;
	}

	int getCurrentHealth() const {
		return currentHealth;
	}

	void setCurrentHealth(int currentHealth) {
		this->currentHealth = currentHealth;
	}

	int getCurrentIceDefense() const {
		return currentIceDefense;
	}

	void setCurrentIceDefense(int currentIceDefense) {
		this->currentIceDefense = currentIceDefense;
	}

	int getCurrentLightningDefense() const {
		return currentLightningDefense;
	}

	void setCurrentLightningDefense(int currentLightningDefense) {
		this->currentLightningDefense = currentLightningDefense;
	}

	int getCurrentPhysicalDefense() const {
		return currentPhysicalDefense;
	}

	void setCurrentPhysicalDefense(int currentPhysicalDefense) {
		this->currentPhysicalDefense = currentPhysicalDefense;
	}

	int getCurrentSize() const {
		return currentSize;
	}

	void setCurrentSize(int currentSize) {
		this->currentSize = currentSize;
	}

	int getCurrentSpeed() const {
		return currentSpeed;
	}

	void setCurrentSpeed(int currentSpeed) {
		this->currentSpeed = currentSpeed;
	}

	int getCurrentWaterDefense() const {
		return currentWaterDefense;
	}

	void setCurrentWaterDefense(int currentWaterDefense) {
		this->currentWaterDefense = currentWaterDefense;
	}

	ElementType getCurrentElement() const {
		return currentElement;
	}

	void setCurrentElement(ElementType currentElement) {
		this->currentElement = currentElement;
	}
	
	int getCurrentInitiative() const {
		return currentInitiative;
	}
	
	void setCurrentInitiative(int initiative) {
		currentInitiative = initiative;
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

	int getGrid() const {
		return gid;
	}

	int getGridX() const {
		return gridX;
	}

	void setOnGridX(int x) {
		gridX = x;
	}

	int getGridY() const {
		return gridY;
	}

	void setOnGridY(int y) {
		gridY = y;
	}
	
	void setOnGrid(int x, int y) {
		gridX = x;
		gridY = y;
	}
	
	vector<Effect*> getCurrentEffects() const {
		return currentEffects;
	}

	vector<Status*> getCurrentStatus() const {
		return currentStatus;
	}

	bool isAlive() const {
		return currentHealth > 0;
	}

	bool isDead() const {
		return currentHealth <= 0;
	}
	
	// For fleeing units ect. - not implemented yet
	bool isAvailable() const {
		return isAlive();
	}

	// Process all Effects that have originated from this unit
	void processEffects();

	// Clean up all expired Effects
	void cleanEffects();

	void print() const;

	~Unit() {}

	friend class Ability;
	friend class Effect;
	friend class Status;
	friend class Group;
};

#endif

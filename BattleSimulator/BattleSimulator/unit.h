#ifndef __UNIT_H__
#define __UNIT_H__

#include "pch.h"

#include "character.h"
#include <cstdlib>
#include <vector>

class Unit
{
protected:
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
	
	Skill basicSkill;
	Skill backSkill;
	Skill midSkill;
	Skill frontSkill;
	Skill currentSkill;
	int currentTier; 

	int gid;
	int formX; // Formation Position
	int formY;
	int gridX; // Actual Positions in battles
	int gridY;

	vector<Effect*> currentEffects;
	vector<StatusGroup*> currentStatus;

	bool leader;
	bool done;

	int rValue; // Used to settle ties, can be used for multiple things

	void carryOverCharacterStatistics();
public:
	Unit(Character* character, int gid, int x, int y);

	void applyCharacterSkillSets() {
		basicSkill = character->getBasicSkill();
		backSkill = character->getBackSkill();
		midSkill = character->getMidSkill();
		frontSkill = character->getFrontSkill();
	}
	
	string getName() const {
		return character->getName();
	}
	
	int getCurrentHealth() const {
		return currentHealth;
	}

	void setCurrentHealth(int currentHealth) {
		this->currentHealth = currentHealth;
		this->currentHealth = bound(this->currentHealth, 0, bound(maxHealth, VALUE_HEALTH));
	}
	
	int getMaxHealth() const {
		return maxHealth;
	}

	void setMaxHealth(int maxHealth) {
		this->maxHealth = maxHealth;
		setCurrentHealth(currentHealth);
	}
	
	double getPercentHealth() const {
		return static_cast<double>(currentHealth) / maxHealth;
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

	Skill getCurrentSkill() const {
		return currentSkill;
	}

	void setCurrentSkill(Skill value) {
		currentSkill = value;
	}

	int getCurrentTier() const {
		return currentTier;
	}

	void setCurrentTier(int value) {
		currentTier = value;
	}

	int getGrid() const {
		return gid;
	}

	int getFormX() const {
		return formX;
	}
    
	void setOnFormX(int x) {
		formX = x;
	}
    
	int getFormY() const {
		return formY;
	}
    
	void setOnFormY(int y) {
		formY = y;
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
	
	bool isLeader() const {
		return leader;
	}

	void setLeader(bool value) {
		leader = value;
	}

	bool isDone() const {
		return done;
	}

	void setDone(bool value) {
		done = value;
	}

	vector<Effect*> getCurrentEffects() const {
		return currentEffects;
	}

	vector<StatusGroup*> getCurrentStatus() const {
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
		return isAlive() && gridX >= 0 && gridY >= 0;
	}

	void setRValue() {
		rValue = rand();
	}

	int getRValue() const {
		return rValue;
	}

	void addStatusGroup(StatusGroup* group);
	void eraseStatusGroup(StatusGroup* group);
	void addEffect(Effect* effect);
	void eraseEffect(Effect* effect);
	int getNumDebuffs() const;
	int getNumBuffs() const;
	int getNumNeutrals() const;
	bool hasStatus(StatusBenefit benefit) const;
	vector<StatusGroup*> getStatusBySubname(const string & subname) const;
	vector<StatusGroup*> getDispellableStatusByBenefit(StatusBenefit benefit) const;
	vector<StatusGroup*> getDispellableStatusBySubname(const string & subname) const;

	void activateOnRound();
	void activateOnPrePerformHit(Event* evt);
	void activateOnPostPerformHit(Event* evt);
	void activateOnPreReactHit(Event* evt);
	void activateOnPostReactHit(Event* evt);
	void activateOnPreApplyDamage(Damage* applier);
	void activateOnPostApplyDamage(Damage* applier);
	void activateOnPreReceiveDamage(Damage* applier);
	void activateOnPostReceiveDamage(Damage* applier);
	void activateOnPreFindTarget(Targeter* system);
    void activateOnPostFindTarget(Targeter* system);
	void activateOnPreBecomeTarget(Targeter* system);
	void activateOnPostBecomeTarget(Targeter* system);
	void activateOnCheckpoint(Ability* ability);
    void activateOnSelectAbility(Unit* caster);

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

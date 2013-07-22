#pragma once

#include "pch.h"

#include "character.h"

enum DamageType { DAMAGE_NONE, DAMAGE_MELEE, DAMAGE_RANGE, DAMAGE_FIRE, DAMAGE_WATER, DAMAGE_EARTH, DAMAGE_ICE, DAMAGE_LIGHTNING, DAMAGE_HEALING };

class Effect;
class Status;

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
	
	int currentMeleeDefense;
	int currentRangeDefense;
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

	void addCurrentPhysicalAttack(int value) {
		currentPhysicalAttack += value;
	}

	void addCurrentMagicAttack(int value) {
		currentMagicAttack += value;
	}

	int getCurrentSpeed() const {
		return currentSpeed;
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

	int getGid() const {
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

	void processEffects();

	void cleanEffects();

	void print() const;

	~Unit() {}
	
	friend class Ability;
	friend class Effect;
	friend class Status;
	friend class Group;
	friend int applyDamage(Unit* caster, Unit* target, DamageType type);
	friend int applyDamage(Unit* target, int damage, DamageType type);
};

#include "status.h"

int findNumMatching(const vector<DamageType> & types1, const vector<DamageType> & types2);
int applyDamage(Unit* caster, Unit* target, DamageType type);
int applyDamage(Unit* target, int damage, DamageType type);
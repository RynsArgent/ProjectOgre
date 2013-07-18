#pragma once

#include "character.h"

enum DamageType { DAMAGE_NONE, DAMAGE_MELEE, DAMAGE_RANGE, DAMAGE_FIRE, DAMAGE_WATER, DAMAGE_EARTH, DAMAGE_ICE, DAMAGE_LIGHTNING, DAMAGE_HEALING };

class Unit
{
private:
	Character *character;

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

	void carryOverCharacterStatistics();
public:
	Unit(Character* character, int gid, int x, int y);

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
	
	Skill getMidSkill() const {
		return midSkill;
	}
	
	Skill getFrontSkill() const {
		return frontSkill;
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

	~Unit() {}
	
	friend class Ability;
	friend int applyDamage(Unit* caster, Unit* target, DamageType type);
	friend int applyDamage(Unit* target, int damage, DamageType type);
};

int applyDamage(Unit* caster, Unit* target, DamageType type);
int applyDamage(Unit* target, int damage, DamageType type);
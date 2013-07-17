#pragma once

#include <string>

using namespace std;

enum ElementType { ELEMENT_NONE, ELEMENT_PHYSICAL, ELEMENT_FIRE, ELEMENT_WATER, ELEMENT_EARTH, ELEMENT_ICE, ELEMENT_LIGHTNING };

class Character
{
protected:
	string name;

	int baseHealth;
	int baseDamage;
	int baseSpeed;
	
	int basePhysicalDefense;
	int baseFireDefense;
	int baseWaterDefense;
	int baseEarthDefense;
	int baseIceDefense;
	int baseLightningDefense;
	
	int baseSize;
	
	ElementType favoredElement;
	int baseUpkeep;
public:
	Character(const string & name = "", int health = 0, int damage = 0, int speed = 0, 
		int physicalDefense = 0, int fireDefense = 0, int waterDefense = 0, 
		int earthDefense = 0, int iceDefense = 0, int lightningDefense = 0,
		int size = 0, ElementType favoredElement = ELEMENT_NONE, int upkeep = 0);

		int getBaseDamage() const {
		return baseDamage;
	}

	void setBaseDamage(int baseDamage) {
		this->baseDamage = baseDamage;
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

	const string& getName() const {
		return name;
	}

	void setName(const string& name) {
		this->name = name;
	}
};

class Fighter : public Character
{
protected:
	const static int BASE_HEALTH = 10;
	const static int BASE_DAMAGE = 3;
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
};

class Scout : public Character
{
protected:
	const static int BASE_HEALTH = 8;
	const static int BASE_DAMAGE = 2;
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
};

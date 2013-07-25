#pragma once

#include "pch.h"

#include "battle.h"

// Base class for abilities
class Ability
{
protected:
	int cost;
public:
	Ability(int cost = 0);
	
	// Most abilities will have their logic implemented in this function
	virtual void action(Unit* caster, Battle* battle) const = 0;

	~Ability() {}
};

class NoSkill : public Ability
{
protected:
	static const int COST = 0;
public:
	NoSkill() : Ability(COST) {}
	virtual void action(Unit* caster, Battle* battle) const {}
	~NoSkill() {}
};

class HundredBlades : public Ability
{
protected:
	static const int COST = 1;
public:
	HundredBlades() : Ability(COST) {}
	virtual void action(Unit* caster, Battle* battle) const;
	~HundredBlades() {}
};

class Block : public Ability
{
protected:
	static const int COST = 1;
public:
	Block() : Ability(COST) {}
	virtual void action(Unit* caster, Battle* battle) const;
	~Block() {}
};

class Strike : public Ability
{
protected:
	static const int COST = 1;
public:
	Strike() : Ability(COST) {}
	virtual void action(Unit* caster, Battle* battle) const;
	~Strike() {}
};

class Taunt : public Ability
{
protected:
	static const int COST = 1;
public:
	Taunt() : Ability(COST) {}
	virtual void action(Unit* caster, Battle* battle) const;
	~Taunt() {}
};

class BattleShout : public Ability
{
protected:
	static const int COST = 1;
public:
	BattleShout() : Ability(COST) {}
	virtual void action(Unit* caster, Battle* battle) const;
	~BattleShout() {}
};

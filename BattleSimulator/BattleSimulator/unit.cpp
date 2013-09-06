#include "unit.h"

#include <algorithm>
#include "status.h"

void Unit::carryOverCharacterStatistics()
{
	currentHealth = character->getBaseHealth();
	maxHealth = character->getBaseHealth();

	currentSpeed = character->getBaseSpeed();
	currentInitiative = 0;
	
	currentPhysicalAttack = character->getBasePhysicalAttack();
	currentMagicAttack = character->getBaseMagicAttack();
	
	currentPhysicalDefense = character->getBasePhysicalDefense();
	currentFireDefense = character->getBaseFireDefense();
	currentWaterDefense = character->getBaseWaterDefense();
	currentEarthDefense = character->getBaseEarthDefense();
	currentIceDefense = character->getBaseIceDefense();
	currentLightningDefense = character->getBaseLightningDefense();

	currentSize = character->getBaseSize();

	currentElement = character->getFavoredElement();
}

Unit::Unit(Character* character, int gid, int x, int y)
	: character(character), 
	basicSkill(NO_STANDARD_SKILL), backSkill(NO_STANDARD_SKILL), midSkill(NO_STANDARD_SKILL), frontSkill(NO_STANDARD_SKILL), 
	currentSkill(NO_STANDARD_SKILL), currentTier(0),
	gid(gid), formX(x), formY(y), gridX(x), gridY(y), currentEffects(), currentStatus(),
	leader(false), done(false), rValue(0)
{
	carryOverCharacterStatistics();
}

void Unit::addStatusGroup(StatusGroup* statusGroup)
{
	currentStatus.push_back(statusGroup);
}

void Unit::eraseStatusGroup(StatusGroup* statusGroup)
{
	currentStatus.erase(find(currentStatus.begin(), currentStatus.end(), statusGroup));
}

void Unit::addEffect(Effect* effect)
{
	currentEffects.push_back(effect);
}

void Unit::eraseEffect(Effect* effect)
{
	currentEffects.erase(find(currentEffects.begin(), currentEffects.end(), effect));
}

int Unit::getNumDebuffs() const 
{
	int count = 0;
	for (int i = 0; i < currentStatus.size(); ++i)
		if (currentStatus[i]->getBenefit() == DEBUFF &&
			currentStatus[i]->getTotalStacks() > 0)
			++count;
	return count;
}

int Unit::getNumBuffs() const 
{
	int count = 0;
	for (int i = 0; i < currentStatus.size(); ++i)
		if (currentStatus[i]->getBenefit() == BUFF &&
			currentStatus[i]->getTotalStacks() > 0)
			++count;
	return count;
}

int Unit::getNumNeutrals() const 
{
	int count = 0;
	for (int i = 0; i < currentStatus.size(); ++i)
		if (currentStatus[i]->getBenefit() == NEUTRAL &&
			currentStatus[i]->getTotalStacks() > 0)
			++count;
	return count;
}

bool Unit::hasStatus(StatusBenefit benefit) const
{
	for (int i = 0; i < currentStatus.size(); ++i)
		if (currentStatus[i]->getBenefit() == benefit &&
			currentStatus[i]->getTotalStacks() > 0)
			return true;
	return false;
}

vector<StatusGroup*> Unit::getDispellableStatusByBenefit(StatusBenefit benefit) const
{
	vector<StatusGroup*> ret;
	for (int i = 0; i < currentStatus.size(); ++i)
		if (currentStatus[i]->getBenefit() == benefit &&
			currentStatus[i]->getTotalStacks() > 0 &&
			currentStatus[i]->isDispellable())
			ret.push_back(currentStatus[i]);
	return ret;
}

vector<StatusGroup*> Unit::getDispellableStatusBySubname(const string & subname) const
{
	vector<StatusGroup*> ret;
	for (int i = 0; i < currentStatus.size(); ++i)
		if (currentStatus[i]->getSubname() == subname &&
			currentStatus[i]->getTotalStacks() > 0 &&
			currentStatus[i]->isDispellable())
			ret.push_back(currentStatus[i]);
	return ret;
}

vector<StatusGroup*> Unit::getStatusBySubname(const string & subname) const
{
	vector<StatusGroup*> ret;
	for (int i = 0; i < currentStatus.size(); ++i)
		if (currentStatus[i]->getSubname() == subname &&
			currentStatus[i]->getTotalStacks() > 0)
			ret.push_back(currentStatus[i]);
	return ret;
}

// Unused..., onRound is triggered by effect list
void Unit::activateOnRound()
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onRound();
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnPrePerformHit(Event* evt)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onPrePerformHit(evt);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnPostPerformHit(Event* evt)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onPostPerformHit(evt);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnPreReactHit(Event* event)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onPreReactHit(event);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnPostReactHit(Event* event)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onPostReactHit(event);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnPreApplyDamage(Damage* applier)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onPreApplyDamage(applier);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnPostApplyDamage(Damage* applier)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onPostApplyDamage(applier);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnPreReceiveDamage(Damage* applier)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onPreReceiveDamage(applier);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnPostReceiveDamage(Damage* applier)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onPostReceiveDamage(applier);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnPreFindTarget(Targeter* system)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onPreFindTarget(system);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnPostFindTarget(Targeter* system)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onPostFindTarget(system);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnPreBecomeTarget(Targeter* system)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onPreBecomeTarget(system);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnPostBecomeTarget(Targeter* system)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onPostBecomeTarget(system);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnCheckpoint(Ability* ability)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onCheckpoint(ability);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::activateOnSelectAbility(Unit* caster)
{
	for (int i = 0; i < currentStatus.size(); ++i) {
		vector<Status*> instances = currentStatus[i]->getInstances();
		for (int j = 0; j < instances.size(); ++j) {
			instances[j]->onSelectAbility(caster);
			currentStatus[i]->setExecuted(true);
		}
		currentStatus[i]->setExecuted(false);
	}
}

void Unit::processEffects()
{
	for (int i = 0; i < currentEffects.size(); ++i)
	{
		Effect* effect = currentEffects[i];
		effect->processRound();
	}
	for (int i = 0; i < currentEffects.size(); ++i)
	{
		Effect* effect = currentEffects[i];
		effect->resetStatusFlag();
	}
}

void Unit::cleanEffects()
{
	int c = 0;
	vector<Effect*> neffects(currentEffects.size());
	for (int i = 0; i < currentEffects.size(); ++i) {
		Effect* effect = currentEffects[i];

		effect->cleanStatus();

		if (!effect->isExpired()) {
			neffects[c] = effect;
			++c;
		}
		else delete currentEffects[i];
	}
	neffects.resize(c);
	currentEffects = neffects; 
}

void Unit::print() const
{
	cout << getName();
	if (currentHealth <= 0)
	{
		cout << " *dead*";
	}
	else
	{
		cout << " - HP: " << currentHealth << "/" << maxHealth;
		cout << " - ";
		/*
		cout << " Effects: ";
		for (int i = 0; i < currentEffects.size(); ++i)
			cout << currentEffects[i]->getName() << " ";
		cout << " - ";
		cout << "Status: ";
		*/
		for (int i = 0; i < currentStatus.size(); ++i)
			cout << currentStatus[i]->getSubname() << currentStatus[i]->getTotalStacks() << " ";
	}
	cout << endl;
}

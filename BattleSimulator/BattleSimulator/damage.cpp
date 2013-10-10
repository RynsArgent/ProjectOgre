#include "damage.h"

#include "unit.h"
#include "ability.h"
#include "status.h"
#include "battle.h"
#include <algorithm>

DamageNode::DamageNode(int amount, DamageRating rating, DamageType type, DamageNode* next)
	: start(Damage::getDamageValue(rating, amount)), amount(start), final(0), rating(rating), type(type), next(next)
{}

void DamageNode::modify(Unit* target)
{
	int totalDamage = amount;
	switch (type)
	{
	case DAMAGE_PHYSICAL:
		totalDamage -= bound(target->getCurrentPhysicalDefense(), VALUE_PHYSICAL_DEFENSE);
        break;
    case DAMAGE_ARCANE:
        totalDamage -= bound(target->getCurrentArcaneDefense(), VALUE_ARCANE_DEFENSE);
        break;
	case DAMAGE_FIRE:
		totalDamage -= bound(target->getCurrentFireDefense(), VALUE_FIRE_DEFENSE);
		break;
	case DAMAGE_WATER:
		totalDamage -= bound(target->getCurrentWaterDefense(), VALUE_WATER_DEFENSE);
		break;
	case DAMAGE_EARTH:
		totalDamage -= bound(target->getCurrentEarthDefense(), VALUE_EARTH_DEFENSE);
		break;
	case DAMAGE_ICE:
		totalDamage -= bound(target->getCurrentIceDefense(), VALUE_ICE_DEFENSE);
		break;
	case DAMAGE_LIGHTNING:
		totalDamage -= bound(target->getCurrentLightningDefense(), VALUE_LIGHTNING_DEFENSE);
		break;
	case DAMAGE_HEALING:
		break;
	case DAMAGE_TYPELESS:
		break;
	}

	amount = totalDamage;
	if (next)
		next->modify(target);
}

int DamageNode::apply(Unit* target)
{
	final = bound(amount, VALUE_DAMAGE);
	int val = target->getCurrentHealth();
	if (type != DAMAGE_HEALING) val -= final;
	else val += final;
	target->setCurrentHealth(val);

	int totalDamage = final;
	if (next)
		totalDamage += next->apply(target);
	
	if (type != DAMAGE_HEALING) 
		return totalDamage;
	else
		return -totalDamage;
}

void DamageNode::print(ostream& out) const 
{
	if (type != DAMAGE_HEALING)
	   out << " " << final << " (" << start << ") " << toStringDT(type);
	else
	   out << " " << final << " (" << start << ") " << toStringDT(type);
    if (next) next->print(out);
}

void Damage::apply(Battle* battle)
{
	final = 0; // Init calculated damage to 0
	
	head->modify(target);

	// Pre Damage Effects
    Unit* applier = NULL;
	if (action != NULL)
		applier = action->getSource();

	if (applier != NULL)
	{
		applier->activateOnPreApplyDamage(this);
		battle->getGlobalTrigger()->activateOnPreApplyDamage(this);
	}
	target->activateOnPreReceiveDamage(this);
	battle->getGlobalTrigger()->activateOnPreReceiveDamage(this);

	// Apply the final damage
	if (target->isAlive())
		if (head)
			final += head->apply(target);

	// Post Damage Effects
	battle->getGlobalTrigger()->activateOnPostReceiveDamage(this);
	target->activateOnPostReceiveDamage(this);
	if (applier != NULL)
	{
		battle->getGlobalTrigger()->activateOnPostApplyDamage(this);
		applier->activateOnPostApplyDamage(this);
	}
}

void Damage::print(ostream& out) const
{
	if (head) {
		if (head->type != DAMAGE_HEALING)
			out << " does";
		else
			out << " grants";
		head->print(out);
		// Look at first node to determine whether it is healing or damage
		if (head->type != DAMAGE_HEALING)
			out << " damage to " << target->getName();
		else
			out << " to " << target->getName();
	}
}

int Damage::findNumMatching(const vector<DamageType> & types1, const vector<DamageType> & types2)
{
	vector<DamageType> res(types1.size() + types2.size());
	vector<DamageType>::iterator it;
	it = set_union(types1.begin(), types1.end(), types2.begin(), types2.end(), res.begin());
	res.resize(it - res.begin());
	return res.size();
} 

int Damage::getDamageValue(DamageRating rating, int base)
{
	switch (rating)
	{
	case DAMAGE_NONE: // 0%
		return 0;
	case DAMAGE_MINIMAL: // 10%
		return static_cast<int>((base / 10.0) + 0.5);
	case DAMAGE_MINOR: // 25%
		return static_cast<int>((base / 4.0) + 0.5);
	case DAMAGE_LOW: // 50%
		return static_cast<int>((base / 2.0) + 0.5);
	case DAMAGE_MEDIUM: // 100%
		return base;
	case DAMAGE_HIGH: // 150%
		return static_cast<int>((base * 1.5) + 0.5);
	case DAMAGE_MASSIVE: // 200%
		return base * 2;
	case DAMAGE_COLOSSAL: // 400$
		return base * 4;
	case DAMAGE_UNSTOPPABLE: // 1000%
		return base * 10;
	default:
		return 0;
	}
}

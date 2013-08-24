#include "damage.h"

#include "unit.h"
#include "ability.h"
#include "status.h"
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

void Damage::apply()
{
	final = 0; // Init calculated damage to 0
	
	head->modify(target);

	// Pre Damage Effects
    Unit* applier = action->getSource();
	if (applier != NULL) {
		for (int i = 0; i < applier->getCurrentStatus().size(); ++i)
		{
			Status* status = applier->getCurrentStatus()[i];
			status->onPreApplyDamage(this);
		}
	}
	for (int i = 0; i < target->getCurrentStatus().size(); ++i)
	{
		Status* status = target->getCurrentStatus()[i];
		status->onPreReceiveDamage(this);
	}

	// Apply the final damage
	if (head)
		final += head->apply(target);

	// Post Damage Effects
	for (int i = 0; i < target->getCurrentStatus().size(); ++i)
	{
		Status* status = target->getCurrentStatus()[i];
		status->onPostReceiveDamage(this);
	}
	if (applier != NULL) {
		for (int i = 0; i < applier->getCurrentStatus().size(); ++i)
		{
			Status* status = applier->getCurrentStatus()[i];
			status->onPostApplyDamage(this);
		}
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
	case DAMAGE_NONE:
		return 0;
	case DAMAGE_MINOR:
		return static_cast<int>((base / 4.0) + 0.5);
	case DAMAGE_LOW:
		return static_cast<int>((base / 2.0) + 0.5);
	case DAMAGE_MEDIUM:
		return base;
	case DAMAGE_HIGH:
		return static_cast<int>((base * 1.5) + 0.5);
	case DAMAGE_MASSIVE:
		return base * 2;
	case DAMAGE_COLOSSAL:
		return base * 4;
	default:
		return 0;
	}
}

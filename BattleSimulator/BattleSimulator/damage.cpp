#include "damage.h"

#include "unit.h"
#include "ability.h"
#include "status.h"

void DamageNode::modify(Unit* target)
{
	int totalDamage = amount;
	switch (type)
	{
	case DAMAGE_PHYSICAL:
		totalDamage -= target->getCurrentPhysicalDefense();
		break;
	case DAMAGE_FIRE:
		totalDamage -= target->getCurrentFireDefense();
		break;
	case DAMAGE_WATER:
		totalDamage -= target->getCurrentWaterDefense();
		break;
	case DAMAGE_EARTH:
		totalDamage -= target->getCurrentEarthDefense();
		break;
	case DAMAGE_ICE:
		totalDamage -= target->getCurrentIceDefense();
		break;
	case DAMAGE_LIGHTNING:
		totalDamage -= target->getCurrentLightningDefense();
		break;
	case DAMAGE_HEALING:
		totalDamage = -totalDamage;
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
	final = amount;
	
	// If it is not healing, then damage cannot be negative
	if (final < 0 && type != DAMAGE_HEALING)
		final = 0;
	// It if is healing, then damage should be in the negative
	if (final > 0 && type == DAMAGE_HEALING)
		final = 0;
	int val = target->getCurrentHealth();
	val -= final;
	target->setCurrentHealth(val);
	
	int totalDamage = final;
	if (next)
		totalDamage += next->apply(target);

	return totalDamage;
}

void DamageNode::print() const 
{
	if (type != DAMAGE_HEALING)
	   cout << " " << final << " (" << start << ") " << toStringDT(type);
	else
	   cout << " " << -final << " (" << start << ") " << toStringDT(type);
    if (next) next->print();
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
			status->onPreApplyDamage(this);
		}
	}
}

void Damage::print() const
{
	if (head) {
		if (head->type != DAMAGE_HEALING)
			cout << " does";
		else
			cout << " grants";
		head->print();
		// Look at first node to determine whether it is healing or damage
		if (head->type != DAMAGE_HEALING)
			cout << " damage to " << target->getName();
		else
			cout << " to " << target->getName();
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
		return (base / 4.0) + 0.5;
	case DAMAGE_LOW:
		return (base / 2.0) + 0.5;
	case DAMAGE_MEDIUM:
		return base;
	case DAMAGE_HIGH:
		return (base * 1.5) + 0.5;
	case DAMAGE_MASSIVE:
		return base * 2;
	case DAMAGE_COLOSSAL:
		return base * 4;
	default:
		return 0;
	}
}

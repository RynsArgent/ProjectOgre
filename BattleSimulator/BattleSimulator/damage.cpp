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
	case DAMAGE_TYPELESS:
		break;
	}

	if (next)
		next->modify(target);
}

int DamageNode::apply(Unit* target)
{
	final = amount;
	
	if (amount < 0)
		amount = 0;
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
    cout << " " << toStringDT(type) << " " << final << " (" << start << ")";
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
    cout << "Damage";
    head->print();
    cout << " to " << target->getName() << endl;
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
		break;
	case DAMAGE_LOW:
		return (base / 2.0) + 0.5;
		break;
	case DAMAGE_MEDIUM:
		return base;
		break;
	case DAMAGE_HIGH:
		return (base * 1.5) + 0.5;
		break;
	case DAMAGE_MASSIVE:
		return base * 2;
		break;
	default:
		return 0;
	}
}

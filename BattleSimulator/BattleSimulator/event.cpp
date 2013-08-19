#include "event.h"

#include "action.h"
#include "battle.h"
#include "damage.h"
#include "status.h"
#include "unit.h"
#include <cstdlib>

Event::Event(Action* ref, Damage* damage, Status* status, int chance)
: ref(ref), damage(damage), status(status), chance(chance), success(false)
{
    if (ref) {
        ref->addEvent(this);
        ref->getBattle()->addToEventStack(this);
    }
}

void Event::apply()
{
	Unit* source = ref->getSource();
	Unit* target = NULL;
	if (damage) target = damage->target;
	else if (status) target = status->getTarget();

	// Trigger Pre on hit status effects
	if (source != NULL) {
		for (int i = 0; i < source->getCurrentStatus().size(); ++i)
		{
			Status* status = source->getCurrentStatus()[i];
			status->onPrePerformHit(this);
		}
	}
	for (int i = 0; i < target->getCurrentStatus().size(); ++i)
	{
		Status* status = target->getCurrentStatus()[i];
		status->onPreReactHit(this);
	}

	// Determine if roll was a success out of a 100%
	int roll = rand() % 100 + 1;
	success = roll <= chance;
	
	// Trigger Post on hit status effects
	for (int i = 0; i < target->getCurrentStatus().size(); ++i)
	{
		Status* status = target->getCurrentStatus()[i];
		status->onPostReactHit(this);
	}
	if (source != NULL) {
		for (int i = 0; i < source->getCurrentStatus().size(); ++i)
		{
			Status* status = source->getCurrentStatus()[i];
			status->onPostPerformHit(this);
		}
	}

	// If the event hit was a success, apply the damage, status effect, whatnot
	if (success) {
		if (damage != NULL)
			damage->apply();
		if (status != NULL)
			status->getEffect()->addStatus(status);
	}
}

void Event::print() const
{
	if (!damage && !status && ref->getAction() != EFFECT_TRIGGER) {
		cout << ref->getSource()->getName() << " readies " << ref->getName() << endl;
		return;
	}

    cout << ref->getSource()->getName() << "'s " << ref->getName();
    if (damage) {
		if (success) {
			cout << " does";
			damage->print();
		} else {
			cout << " misses " << damage->target->getName();
		}
    }
    if (status) {
		if (damage)
			cout << " and ";
		if (success) {
			cout << " applies " << status->getEffect()->getName() << " to " << status->getTarget()->getName();
		} else {
			cout << " fizzles on " << status->getTarget()->getName();
		}
    }
    cout << endl;
}

Event::~Event()
{
    delete damage;
    if (!success && status)
        delete status; // Only delete status if not linked to an Effect
}
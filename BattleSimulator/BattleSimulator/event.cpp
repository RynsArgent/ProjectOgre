#include "event.h"

#include "action.h"
#include "battle.h"
#include "damage.h"
#include "status.h"
#include "unit.h"
#include <cstdlib>

Event::Event(Action* ref, int chance)
	: ref(ref), chance(chance), success(true)
{
    if (ref) {
        ref->addEvent(this);
        ref->getBattle()->addToEventStack(this);
    }
}

void Event::determineSuccess()
{
	// Determine if roll was a success out of a 100%
	int roll = rand() % 100 + 1;
	success = roll <= chance;
}


void Event::determineSuccess(Unit* target)
{
	Unit* source = ref->getSource();

	// Trigger Pre on hit status effects
	if (source != NULL) {
		for (int i = 0; i < source->getCurrentStatus().size(); ++i)
		{
			Status* status = source->getCurrentStatus()[i];
			status->onPrePerformHit(this);
		}
	}
	if (target != NULL) {
		for (int i = 0; i < target->getCurrentStatus().size(); ++i)
		{
			Status* status = target->getCurrentStatus()[i];
			status->onPreReactHit(this);
		}
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
}

void Event::apply()
{
}

void Event::print() const
{
	if (ref->getAction() != EFFECT_TRIGGER)
		cout << ref->getSource()->getName() << " readies " << ref->getName() << endl;
}

Event::~Event()
{
}

void EventCauseDamage::apply()
{
	determineSuccess(damage->target);
	if (success)
		damage->apply();
}

void EventCauseDamage::print() const
{
    cout << ref->getSource()->getName() << "'s " << ref->getName();
    if (damage) {
		if (success) {
			damage->print();
		} else {
			cout << " misses " << damage->target->getName();
		}
	}
    cout << endl;
}

EventCauseDamage::~EventCauseDamage()
{
    delete damage;
}

void EventCauseStatus::apply()
{
	determineSuccess(status->getTarget());
	if (success)
		status->getEffect()->addStatus(status);
}

void EventCauseStatus::print() const
{
    cout << ref->getSource()->getName() << "'s " << ref->getName();
    if (status) {
		if (success) {
			cout << " applies " << status->getEffect()->getName() << " to " << status->getTarget()->getName();
		} else {
			cout << " fizzles on " << status->getTarget()->getName();
		}
    }
    cout << endl;
}

EventCauseStatus::~EventCauseStatus()
{
    if (!success && status)
        delete status; // Only delete status if not linked to an Effect
}

void EventRemoveStatus::apply()
{
	determineSuccess();
	if (!success && !target->isAvailable())
		return;

	// Removes the most recent status effect of the matching type
	for (int i = target->getCurrentStatus().size() - 1; i >= 0; --i) {
		Status* rstatus = target->getCurrentStatus()[i];
		if (rstatus->getBenefit() == removingType &&
			rstatus->isDispellable())
		{
			removedResult = rstatus;
			rstatus->onKill();
			break;
		}
	}
}

void EventRemoveStatus::print() const
{
    if (removedResult) {
		cout << ref->getSource()->getName() << "'s " << ref->getName();
		if (removedResult) {
			cout << " removes " << removedResult->getSubname() << " from " << target->getName();
		}
		cout << endl;
    }
}

EventRemoveStatus::~EventRemoveStatus()
{
}

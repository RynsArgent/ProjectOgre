#include "event.h"

#include "action.h"
#include "battle.h"
#include "damage.h"
#include "status.h"
#include "unit.h"
#include <cstdlib>

Event::Event(Action* ref, int chance)
	: ref(ref), chance(chance), success(true), desc("")
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

void Event::print(ostream& out) const
{
	if (ref->getAction() != EFFECT_TRIGGER)
		out << ref->getSource()->getName() << " readies " << ref->getName() << endl;
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

void EventCauseDamage::print(ostream& out) const
{
    out << ref->getSource()->getName() << "'s " << ref->getName();
    if (damage) {
		if (success) {
			damage->print(out);
		} else {
			out << " misses " << damage->target->getName();
		}
	}
    out << endl;
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

void EventCauseStatus::print(ostream& out) const
{
    out << ref->getSource()->getName() << "'s " << ref->getName();
    if (status) {
		if (success) {
			out << " applies " << status->getEffect()->getName() << " to " << status->getTarget()->getName();
		} else {
			out << " fizzles on " << status->getTarget()->getName();
		}
    }
    out << endl;
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
	vector<Status*> candidateStatusList = target->getDispellableStatusByBenefit(removingType);
	vector<Status*> targetStatusList;
	if (candidateStatusList.size() > 0)
	{
		string subname = candidateStatusList[candidateStatusList.size() - 1]->getSubname();
		vector<Status*> targetStatusList = target->getDispellableStatusBySubname(subname);
	}
	// Remove all status effects with the same name
	for (int i = 0; i < targetStatusList.size(); ++i)
		targetStatusList[i]->onKill();
}

void EventRemoveStatus::print(ostream& out) const
{
    if (removedResult) {
		out << ref->getSource()->getName() << "'s " << ref->getName();
		if (removedResult) {
			out << " removes " << removedResult->getSubname() << " from " << target->getName();
		}
		out << endl;
    }
}

EventRemoveStatus::~EventRemoveStatus()
{
}

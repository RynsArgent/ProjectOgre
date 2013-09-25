#include "event.h"

#include "action.h"
#include "battle.h"
#include "damage.h"
#include "status.h"
#include "unit.h"
#include <cstdlib>

Event::Event(Action* ref, const string & name, int chance)
	: ref(ref), name(name), chance(chance), success(true), desc("")
{
    if (ref) {
        ref->addEvent(this);
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
	Unit* source = NULL;
	if (ref != NULL)
	 source = ref->getSource();

	// Trigger Pre on hit status effects
	if (source != NULL)
		source->activateOnPrePerformHit(this);
	target->activateOnPreReactHit(this);

	// Determine if roll was a success out of a 100%
	int roll = rand() % 100 + 1;
	success = roll <= chance;
	
	// Trigger Post on hit status effects
	target->activateOnPostReactHit(this);
	if (source != NULL)
		source->activateOnPostPerformHit(this);
}

void Event::apply(Battle* battle)
{
	battle->addToEventStack(this);
}

void Event::print(ostream& out) const
{
	if (ref != NULL && ref->getAction() != EFFECT_TRIGGER)
		out << ref->getSource()->getName() << " readies " << name << endl;
}

Event::~Event()
{
}

void EventCauseDamage::apply(Battle* battle)
{
	Event::apply(battle);
	determineSuccess(damage->target);
	if (success)
		damage->apply();
}

void EventCauseDamage::print(ostream& out) const
{
	if (ref != NULL)
		out << ref->getSource()->getName() << "'s ";
	out << name;
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

void EventCauseStatus::apply(Battle* battle)
{
	Event::apply(battle);
	determineSuccess(status->getTarget());
	if (success)
		status->getEffect()->addStatus(status);
}

void EventCauseStatus::print(ostream& out) const
{
	if (ref != NULL)
		out << ref->getSource()->getName() << "'s ";
	out << name;
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

void EventRemoveStatus::apply(Battle* battle)
{
	Event::apply(battle);
	determineSuccess();
	if (!success && !target->isAvailable())
		return;
	
	// Removes the most recent status effect of the matching type
	vector<StatusGroup*> candidateStatusList = target->getDispellableStatusByBenefit(removingType);
	vector<StatusGroup*> targetStatusList;
	if (candidateStatusList.size() > 0)
	{
		removedResult = candidateStatusList[candidateStatusList.size() - 1];
		vector<Status*> instances = removedResult->getInstances();
		for (int j = 0; j < instances.size(); ++j)
			instances[j]->onKill();
	}
}

void EventRemoveStatus::print(ostream& out) const
{
    if (removedResult) {
		if (ref != NULL)
			out << ref->getSource()->getName() << "'s "; 
		out << name;
		if (removedResult) {
			out << " removes " << removedResult->getSubname() << " from " << target->getName();
		}
		out << endl;
    }
}

EventRemoveStatus::~EventRemoveStatus()
{
}

#include "event.h"

#include "action.h"
#include "battle.h"
#include "damage.h"
#include "status.h"
#include "unit.h"
#include "group.h"
#include <cstdlib>

Event::Event(Action* ref, const string & name, EventType type, int chance, bool hiddenSource)
	: ref(ref), name(name), type(type), chance(chance), success(true), desc(""), hiddenSource(hiddenSource)
{
    if (ref) {
        ref->addEvent(this);
    }
}

bool Event::isAutomaticSuccess() const
{
	return chance > 100;
}

bool Event::isAutomaticFailure() const
{
	return chance < 0;
}

void Event::determineSuccess()
{
	// Determine if roll was a success out of a 100%
	int roll = rand() % 100 + 1;
	success = roll <= chance;
}

void Event::apply(Battle* battle)
{
	battle->addToEventStack(this);
}

void Event::print(ostream& out) const
{
	if (!hiddenSource && ref != NULL && ref->getAction() != EFFECT_TRIGGER)
	{
		out << ref->getSource()->getName() << " readies " << name << endl;
	}
}

Event::~Event()
{
}

void EventAttack::determineSuccess(Unit* target)
{
	Unit* source = NULL;
	if (!hiddenSource && ref != NULL)
	 source = ref->getSource();

	// Trigger Pre on hit status effects
	if (source != NULL)
	{
		source->activateOnPrePerformHit(this);
		ref->getBattle()->getGlobalTrigger()->activateOnPrePerformHit(this);
	}
	target->activateOnPreReactHit(this);
	ref->getBattle()->getGlobalTrigger()->activateOnPreReactHit(this);

	// Determine if roll was a success out of a 100%
	int roll = rand() % 100 + 1;
	success = roll <= chance;
	
	// Trigger Post on hit status effects
	ref->getBattle()->getGlobalTrigger()->activateOnPostReactHit(this);
	target->activateOnPostReactHit(this);
	if (source != NULL)
	{
		ref->getBattle()->getGlobalTrigger()->activateOnPostPerformHit(this);
		source->activateOnPostPerformHit(this);
	}
}

void EventAttack::apply(Battle* battle)
{
	Event::apply(battle);
	determineSuccess(target);
}

void EventAttack::print(ostream& out) const
{
	if (!hiddenSource && ref != NULL)
		out << ref->getSource()->getName();
	out << " attacks";
	if (target)
		out << " " << target->getName();
	out << " with " << name;
	
	if (success) {
		out << " and hits";
	} else {
		out << " but fails";
	}
	out << endl;
}

EventAttack::~EventAttack()
{
}

void EventCauseDamage::apply(Battle* battle)
{
	Event::apply(battle);
	determineSuccess();
	if (success)
		damage->apply(battle);
}

void EventCauseDamage::print(ostream& out) const
{
	if (!hiddenSource && ref != NULL)
		out << ref->getSource()->getName() << "'s ";
	out << name;
    if (damage) {
		damage->print(out);
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
	determineSuccess();
	if (success)
		status->getEffect()->addStatus(status);
}

void EventCauseStatus::print(ostream& out) const
{
	if (!hiddenSource && ref != NULL)
		out << ref->getSource()->getName() << "'s ";
	out << name;
    if (status) {
		if (status->getTarget()->getName() == "")
			if (success)
				out << " is applied";
			else
				out << " fizzled";
		else 
			if (success)
				out << " applies " << status->getSubname() << " to " << status->getTarget()->getName();
			else
				out << " fizzles on " << status->getTarget()->getName();
    }
    out << endl;
}

EventCauseStatus::~EventCauseStatus()
{
	if (status)
	{
		if (!success)
			delete status; // Only delete status if not linked to an Effect
		else
			status->setFresh(false);
	}
}

void EventRemoveStatus::apply(Battle* battle)
{
	Event::apply(battle);
	determineSuccess();
	if (!success && !target->isAvailable()) {
		removedResult = NULL; // to notify failure
		return;
	}
	
	if (!removedResult)
	{
		// Removes the most recent status effect of the matching type
		vector<StatusGroup*> candidateStatusList = target->getDispellableStatusByBenefit(removingType);
		vector<StatusGroup*> targetStatusList;
		if (candidateStatusList.size() > 0)
		{
			removedResult = candidateStatusList[candidateStatusList.size() - 1];
		} 
	}

	if (removedResult)
	{
		vector<Status*> instances = removedResult->getInstances();
		for (int j = 0; j < instances.size(); ++j)
			instances[j]->onKill();
	}
}

void EventRemoveStatus::print(ostream& out) const
{
    if (removedResult) {
		if (!hiddenSource && ref != NULL)
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

void EventReposition::apply(Battle* battle)
{
	Event::apply(battle);
	determineSuccess();
	if (success) {
        pointless = target->getGridX() == destination.x && target->getGridY() == destination.y;
        
		Group* group = battle->getAllyGroup(target->getGrid());
		if (group->getUnitAt(destination) == NULL)
		{
			group->setUnitAt(destination, target);
			group->setUnitAt(target->getGridX(), target->getGridY(), NULL);
			target->setOnGrid(destination.x, destination.y);
		}
	}
}

void EventReposition::print(ostream& out) const
{
    if (success && !pointless) {
		if (!hiddenSource && ref != NULL)
			out << ref->getSource()->getName() << "'s "; 
		out << name;
		out << " repositions " << target->getName();
        out << endl;
	}
}
	
EventReposition::~EventReposition()
{
}

void EventRaise::apply(Battle* battle)
{
	Event::apply(battle);
	determineSuccess();
	if (success) {
		Group* allyGroup = battle->getAllyGroup(target->getGrid());

		target->setCurrentHealth(target->getMaxHealth() / 2);
		allyGroup->setUnitAt(destination, target);
		target->setOnGrid(destination.x, destination.y);

		allyGroup->eraseDead(target);
	}
}

void EventRaise::print(ostream& out) const
{
    if (success) {
		if (!hiddenSource && ref != NULL)
			out << ref->getSource()->getName() << "'s "; 
		out << name;
		out << " resurrects " << target->getName();
        out << endl;
	}
}
	
EventRaise::~EventRaise()
{
}
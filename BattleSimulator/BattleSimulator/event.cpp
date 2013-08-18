#include "event.h"

#include "action.h"
#include "battle.h"
#include "damage.h"
#include "status.h"
#include "unit.h"

Event::Event(Action* ref, Damage* damage, Status* status)
: ref(ref), damage(damage), status(status), success(false)
{
    if (ref) {
        ref->addEvent(this);
        ref->getBattle()->addToEventStack(this);
    }
}

void Event::apply()
{
	success = true;
	if (success) {
		if (damage != NULL)
			damage->apply();
		if (status != NULL)
			status->getEffect()->addStatus(status);
	}
}

void Event::print() const
{
    cout << ref->getSource()->getName() << "'s " << ref->getName();
    if (damage) {
        cout << "does ";
        damage->print();
    }
    if (status) {
        cout << " applies " << status->getEffect()->getName() << " to " << status->getTarget()->getName();
    }
    cout << endl;
}

Event::~Event()
{
    delete damage;
    if (!success && status)
        delete status; // Only delete status if not linked to an Effect
}
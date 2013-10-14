#include "action.h"

void Action::addEvent(Event* event)
{
    events.push_back(event);
}

void Action::addTargeter(Targeter* targeter)
{
	targeters.push_back(targeter);
}

Action::~Action()
{
}

#include "action.h"

void Action::addEvent(Event* event)
{
    events.push_back(event);
}

Action::~Action()
{
}

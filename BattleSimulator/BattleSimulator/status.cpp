#include "status.h"

void Status::processTurn(Unit* target, int roundLength) 
{	
	if (timed && timer > 0)
		--timer;
}

void StatusDamagePrevention::updateAtCall(Unit* target)
{
}
	
void StatusDamageOverTime::processTurn(Unit* target, int roundLength)
{
	if (hasExpired() && hasStacks())
	{
		--stacks;
		updateAtCall(target);
	}
	timer = roundLength;
}

void StatusDamageOverTime::updateAtCall(Unit* target)
{
	applyDamage(target, damageAmount, damageType);
}
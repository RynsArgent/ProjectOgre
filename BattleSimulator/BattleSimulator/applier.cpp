#include "applier.h"

#include "damage.h"
#include "status.h"

void Applier::apply()
{
	success = true;
	if (success) {
		if (damage != NULL)
			damage->apply();
		if (status != NULL)
			status->getEffect()->addStatus(status);
	}
}
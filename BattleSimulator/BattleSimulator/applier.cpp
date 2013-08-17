#include "applier.h"

#include "damage.h"

void Applier::print() const
{
	for (int i = 0; i < damages.size(); ++i)
		damages[i]->print();
}

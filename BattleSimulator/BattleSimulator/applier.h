#ifndef __APPLIER_H__
#define __APPLIER_H__

#include "pch.h"

#include "action.h"
#include <vector>

class Applier : public Action
{
private:
	vector<Damage*> damages;
	vector<Effect*> effects;
public:
	Applier() : Action() {}
	
    virtual void print() const;

	~Applier() {}
};

#endif
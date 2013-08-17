#ifndef __ACTION_H__
#define __ACTION_H__

#include "pch.h"

// Base class for battle events
class Action
{
protected:
public:
    Action() {}
    
    virtual void print() const = 0;
    
    ~Action() {}
};

#endif

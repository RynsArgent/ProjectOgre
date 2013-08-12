#ifndef __ACTION_H__
#define __ACTION_H__

#include "pch.h"

// Base class for battle events
class Action
{
protected:
    Unit* source;
    Battle* battle;
public:
    Action() : source(NULL), battle(NULL) {}
    
    Unit* getSource() const { return source; }
    Battle* getBattle() const { return battle; }
    
    virtual void print() const = 0;
    
    ~Action() {}
};

#endif

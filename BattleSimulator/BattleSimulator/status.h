#ifndef __STATUS_H__
#define __STATUS_H__

#include "pch.h"

#include <algorithm>
#include <fstream>
#include "action.h"
#include "event.h"
#include "unit.h"

// To add a status:
// -go to status.h and create the new derived class for the Status, set all the new attributes and select which event handlers to override.
// -go to status.cpp and define the overriden event handlers. onMerge must be handled.
// -go to renderer.cpp near the top of the file and add the new status effect with a color so it is rendered to the screen UNLESS your status is attached to a global trigger (not rendered)

// Status: This defines an instance of a certain status effect, all status effect types are
//			derived off of this to share similar information. A status effect registers itself
//			to base event handlers to perform specific needs. These are modifiable information
//
// StatusBenefit BENEFIT = { BUFF, DEBUFF, NEUTRAL };
// A description of a status effect to determine whether it helps/hurts/meh the target it is
// attached to.
//
// StatusMatch MATCH = { STATUS_UNMATCHABLE, STATUS_SELFMATCHABLE, STATUS_ALLMATCHABLE };
// Determines how the same status applied to the same target will behave. Unmatchable will
// have same name status not merge at all and appear as its own status on target. Self matchable
// will only merge with same name status. All matchable will merge any status with the same name.
// Note that the merge behavior can be defined uniquely in the onMerge event handler.
//
// bool DISPELLABLE = { false, true };
// A flag to determine whether a status can be removed by special means. 
//
// bool INSTANCING = { false, true };
// Determines whether a merging status should call onMerge, or become its own copy in
// a Status Group. This is particularly useful when it is desired that you need to group a set
// of status effects, but each need to keep their individual expiration timer.
//
// bool COLLECTIVE = { false, true }
// This is particularly important for status that do instancing 
// (any status that uses Status Group). calls to useStacks() will not use the instance's stacks
// but the total stacks that belong to Status Group. This allows only a single instance to
// only execute the important parts with access to all stacks 
// (i.e. don't need to show 5 Damage Events from 5 stacks of poison but only one)
//
// bool TIMED = { false, true };
// Tells whether a status effect will expire through a timer.
//
// int MAX_SINGLE_STACKS = { -inf, inf }
// A limit for Self Merging Status Effects.
//
// int MAX_GROUP_STACKS = { -inf, inf }
// A limit for Status Group total stacks (instancing), the most recent instances will overwrite
// the older instances.
//
//
//
// Room for improvement: all status effects are called on an event. This is inefficient.
// All status effect use O(N) to remove attachments to Unit, Effect, and Status Group.
// These can definitely be done better especially when we get to a greater scale battle system.
// Most of these O(N) functionalies come from the onKill event handler.
class Status
{
protected:
	Effect* effect;
	string subname;
	Unit* target;
	StatusBenefit benefit;
	StatusMatch match;
    StatusCategory category;
	bool instancing;
    bool dispellable;
	bool collective;

	int stacks;

	bool timed;
	int timer;

	// Used for efficient memory usage
	bool fresh;
	Status* merger;
	StatusGroup* grouplist;
	bool clean;
public:
	Status(Effect* effect, const string & subname, Unit* target = NULL, StatusBenefit benefit = NEUTRAL, StatusMatch match = STATUS_UNMATCHABLE, StatusCategory category = STATUS_UNCATEGORIZED, bool dispellable = true, bool instancing = false, bool collective = false, bool timed = false, int timer = 0, int stacks = 0)
		: effect(effect), subname(subname), target(target), benefit(benefit), match(match), category(category),
		dispellable(dispellable), instancing(instancing), collective(collective), stacks(stacks), timed(timed), timer(timer), fresh(true), merger(NULL), grouplist(NULL), clean(false)
	{
	}

	Effect* getEffect() const {
		return effect;
	}

	string getSubname() const {
		return subname;
	}

	StatusBenefit getBenefit() const {
		return benefit;
	}
    
	StatusMatch getMatch() const {
		return match;
	}
    
	StatusCategory getCategory() const {
		return category;
	}
	
	Unit* getTarget() const {
		return target;
	}
	
	void setStacks(int value) {
		stacks = value;
	}

	int getStacks() const {
		return stacks;
	}

	bool isInstancing() const {
		return instancing;
	}

	bool isDispellable() const {
		return dispellable;
	}
	
	bool isCollective() const {
		return collective;
	}

	void setTimed(bool value1, int value2) {
		timed = value1;
		timer = value2;
	}

	void setTimed(bool value) {
		timed = value;
	}   

	bool isTimed() const {
		return timed;
	}

	void setTimer(int value) {
		timer = value;
	}

	int getTimer() const {
		return timer;
	}
    
	void setFresh(bool value) {
		fresh = value;
	}

	bool isFresh() const {
		return fresh;
	}

	void setMerger(Status* value) {
		merger = value;
	}

	Status* getMerger() const {
		return merger;
	}

	StatusGroup* getGrouplist() const {
		return grouplist;
	}
	
	bool needsCleaning() const {
		return clean;
	}

	void setCleaning() {
		stacks = 0;
		timer = 0;
		clean = true;
        grouplist = NULL;
	}

	int useStacks() const;

	// Adds to numStacks and returns the actual number added (due to a max limit),
	// NOTE: the number returned is assumed that this object is added its stacks to the original
	int addStacks(int oldstacks);

	// Base function that at its most basic updates the timer, Status Effects will extend
	// the functions they need
	virtual bool hasExpired() const;
	virtual int getMaxSingleStacks() const = 0;
	virtual int getMaxGroupStacks() const = 0;
	
	// Looks for a matching status effect that can be a potential merging candidate
	void findSetMerger();
	// Checks to see if two status effects can be merged together. 
	// Assumption: The two status effects have the same target
	bool canMergeWith(Status* rhs) const;

	// Returns the actual amount of stacks being applied, does not necessarily need to be used if the status effect doesn't use stacks
	//
	// This merges by "Effect trigger unit" Statuses, If different
	// Poison trigger Status are applied for example, this is not used.
	virtual int onMerge(Status* status);

	// This deals with Status Effects that have effects that do something at the moment of creation or disappearance
	// (i.e. Doom III, Unit dies after it expires; would have to check timer == 0 in case of it activity on a dispel)
	// (i.e. Attribute modifiers, modifies stats and then restores them on expiration, specifically targeting Unit variables)
	//
	// Note: In order to support merging (stacking), onSpawn() must be implemented being called in the merging process as well
	//		onKill() must be implemented not being called in the merging process and can handle 2+ stacks in the final end process.
	virtual void onSpawn();
	virtual void onKill();
    
	// This deals with Status Effects that occur every round. If attached to globalTrigger, it is every turn.
	// (i.e. Poison/Burn/Bleed)
	// (i.e. Timed status update at the end of turns)
	virtual void onBeginRound();
	virtual void onEndRound();
    
	// This deals with events that can have a roller's chance to succeed or fail
	// (i.e. Blind will affect the chances of success for an attack)
	virtual void onPrePerformHit(EventAttack* evt);

	// This deals with events that have already determined a roller's chance of success or fail
	// (i.e. Convert a miss for an attack to an auto-hit)
	virtual void onPostPerformHit(EventAttack* evt);
	
	// This deals with events that can have a target be victim to a chance of success or fail
	// (i.e. Evasion increases chance of failure for an attack to succeed on self)
	virtual void onPreReactHit(EventAttack* evt);
	
	// This deals with events that have already a target be victim to a chance of success or fail
	// (i.e. Convert a successful hit to an auto-miss)
	virtual void onPostReactHit(EventAttack* evt);

	// This deals with the triggers after damage is applied. 
	// (i.e. Extra damage against beasts)
	virtual void onPreApplyDamage(Damage* applier);
    
	// This deals with the triggers before damage is about to be applied. 
	// (i.e. Damage dealt heals attacker)
	virtual void onPostApplyDamage(Damage* applier);
    
	// This deals with the triggers before damage is about to be applied. 
	// (i.e. Damage Prevention)
	virtual void onPreReceiveDamage(Damage* applier);
    
	// This deals with the triggers after damage is applied. 
	// (i.e. Heal half of damage taken)
	virtual void onPostReceiveDamage(Damage* applier);

	// This deals with the triggers when candidate units are found.
    // Targets are not exactly chosen yet.
    // (i.e. Confused target also adds all ally units as candidates and set to TARGET_RANDOM)
    // (i.e. Taunted unit adds taunters in candidate list to the priority list)
	virtual void onPreFindTarget(Targeter* system);
    
    // This deals with the triggers when chosen units are selected from
    // the candidate list
    // (i.e. Stun chosen target moment it is selected)
    virtual void onPostFindTarget(Targeter* system);
    
    // This deals with the triggers when a unit becomes a candidate target.
    // (i.e. Bard removes itself from the candidate list cause it does jack)
	virtual void onPreBecomeTarget(Targeter* system);
    
	// This deals with the triggers when a Unit is selected as a chosen target
	// (i.e. Bard removes itself as a target for the first time cause it's so friendly)
	virtual void onPostBecomeTarget(Targeter* system);
    
    // This deals with executing the status effects that need to be checked after triggers.
    // This is mostly for status effects that render a unit unable to act.
    // (i.e. cancel the ability if a unit is stunned (beginning, midway, end)
    virtual void onCheckpoint(Ability* ability);

	// This deals with selecting which ability to use
	// (i.e. Charmed and Confused casters will use their basic ability, Stunned casters ect... will use no ability)
    virtual void onSelectAbility(Unit* caster);
    
    // This deals with an ability just being generated and just
    // before execution.
    // (i.e. Rally causes the next ability to be uninterruptible despite defaults)
    // Note: CC effects like Stun and Sleep work here, however, they are better in onSelectAbility
	virtual void onExecuteAbility(Ability* ability);
    
    
	virtual ~Status() {}
	
	friend class Effect;
};

class StatusStun : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_CC;
	static const bool DISPELLABLE = false;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;
public:
    StatusStun(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Stun", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS)
    {}
    
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }

	// Main Function
	virtual void onCheckpoint(Ability* ability);
    virtual void onSelectAbility(Unit* caster);
	
    ~StatusStun() {}
};

class StatusParalyze : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_CC;
	static const bool DISPELLABLE = false;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;
public:
    StatusParalyze(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Paralyze", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS)
    {}
    
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }

	// Main Function
	virtual void onExecuteAbility(Ability* ability);
	
    ~StatusParalyze() {}
};

class StatusSleep : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_CC;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;
public:
    StatusSleep(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Sleep", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS)
    {}
    
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }

	// Main Function
	virtual void onPostReceiveDamage(Damage* applier);
	virtual void onCheckpoint(Ability* ability);
    virtual void onSelectAbility(Unit* caster);
    
    ~StatusSleep() {}
};

class StatusFlee : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_CC;
	static const bool DISPELLABLE = false;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;
public:
    StatusFlee(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Flee", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS)
    {}
    
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }

	// Main Function
	virtual void onSpawn();
	virtual void onCheckpoint(Ability* ability);
    virtual void onKill();
    virtual void onSelectAbility(Unit* caster);

    ~StatusFlee() {}
};

class StatusConfusion : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_CC;
	static const bool DISPELLABLE = true;    
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;
public:
    StatusConfusion(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Confusion", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS)
    {}
    
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }

	// Main Function
	virtual void onPreFindTarget(Targeter* system);
    virtual void onSelectAbility(Unit* caster);

    ~StatusConfusion() {}
};

class StatusDemoralize : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_CC;
	static const bool DISPELLABLE = false;    
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;
public:
    StatusDemoralize(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Demoralize", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS)
    {}
    
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }

	// Main Function
    virtual void onSelectAbility(Unit* caster);

    ~StatusDemoralize() {}
};

class StatusCharm : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_CC;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;
public:
    StatusCharm(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Charm", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS)
    {}
    
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }

	// Main Function
	virtual void onPostReceiveDamage(Damage* applier);
	virtual void onPreFindTarget(Targeter* system);
    virtual void onSelectAbility(Unit* caster);

    ~StatusCharm() {}
};

class StatusPoison : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_DOT;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 20;
	static const int MAX_GROUP_STACKS = 20;

	static const int TIMER = 5;
	static const int AMOUNT = 5;

	int amount;
public:
	StatusPoison(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Poison", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, stacks), amount(AMOUNT)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void applyTimedDamage();
	
	// Main Function
	virtual int onMerge(Status* status);
	virtual void onBeginRound();

	~StatusPoison() {}
};

class StatusBleed : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_DOT;
	static const bool DISPELLABLE = false;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 5;
	static const int MAX_GROUP_STACKS = 5;
	
	static const int TIMER = 3;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusBleed(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Bleed", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, stacks), amount(AMOUNT)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void applyTimedDamage();
	
	// Main Function
	virtual int onMerge(Status* status);
	virtual void onBeginRound();
    
	~StatusBleed() {}
};

class StatusBurn : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_DOT;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = false;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;
	
	static const int AMOUNT = 15;

	int amount;
public:
	StatusBurn(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Burn", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS), amount(AMOUNT)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void applyTimedDamage();
	
	// Main Function
	virtual int onMerge(Status* status);
	virtual void onBeginRound();
    
	~StatusBurn() {}
};

class StatusRegeneration : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_DOT;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 5;
	static const int MAX_GROUP_STACKS = 5;
	
	static const int TIMER = 3;
	static const int AMOUNT = 20;

	int amount;
public:
	StatusRegeneration(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Regeneration", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, stacks), amount(AMOUNT)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void applyTimedHeal();
	
	// Main Function
	virtual int onMerge(Status* status);
	virtual void onBeginRound();
    
	~StatusRegeneration() {}
};

class StatusBlind : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_CC;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;
public:
	StatusBlind(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Blind", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }

	// Main Function
	virtual void onPrePerformHit(EventAttack* evt);

	~StatusBlind() {}
};

class StatusPolymorph : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_CC;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;
public:
    StatusPolymorph(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Polymorph", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS)
    {}
    
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }

	// Main Function
	virtual void onCheckpoint(Ability* ability);
    virtual void onSelectAbility(Unit* caster);
    
    ~StatusPolymorph() {}
};

class StatusMortality : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_STAT;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 20;
	static const int MAX_GROUP_STACKS = 20;

	static const int TIMER = 5;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusMortality(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Mortality", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS), amount(AMOUNT)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void modifyAttributes(int dvalue);

	// Main Function
	virtual int onMerge(Status* status);
	virtual void onSpawn();
	virtual void onKill();

	~StatusMortality() {}
};

class StatusVitality : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_STAT;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 20;
	static const int MAX_GROUP_STACKS = 20;

	static const int TIMER = 5;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusVitality(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Vitality", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS), amount(AMOUNT)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void modifyAttributes(int dvalue);

	// Main Function
	virtual int onMerge(Status* status);
	virtual void onSpawn();
	virtual void onKill();

	~StatusVitality() {}
};

class StatusShield : public Status
{
private:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;

	static const int TIMER = 1;

	bool applied; 
	int amount;
public:
	StatusShield(Effect* effect, Unit* target, int amount)
        : Status(effect, "Shield", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS), applied(false), amount(amount)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	virtual bool hasExpired() const;
	void applyDamagePrevention(Damage* applier);
	
	// Main Function
	virtual void onPreReceiveDamage(Damage* applier);

	~StatusShield() {}
};

class StatusShell : public Status
{
private:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;

	static const int TIMER = 1;

	bool applied; 
	int amount;
public:
	StatusShell(Effect* effect, Unit* target, int amount)
        : Status(effect, "Shell", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS), applied(false), amount(amount)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	virtual bool hasExpired() const;
	void applyDamagePrevention(Damage* applier);
	
	// Main Function
	virtual void onPreReceiveDamage(Damage* applier);

	~StatusShell() {}
};

class StatusBlock : public Status
{
private:
	static const StatusBenefit BENEFIT = NEUTRAL;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = false;
	static const bool INSTANCING = false;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;

	static const int TIMER = 1;
public:
	StatusBlock(Effect* effect, Unit* target)
        : Status(effect, "Block", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	
	// Main Function
	virtual void onPreReactHit(EventAttack* evt);

	~StatusBlock() {}
};

class StatusBarrier : public Status
{
private:
	static const StatusBenefit BENEFIT = NEUTRAL;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = false;
	static const bool INSTANCING = false;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;

	static const int TIMER = 1;
public:
	StatusBarrier(Effect* effect, Unit* target)
        : Status(effect, "Barrier", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	
	// Main Function
	virtual void onPreReactHit(EventAttack* evt);

	~StatusBarrier() {}
};

class StatusBlink : public Status
{
private:
	static const StatusBenefit BENEFIT = NEUTRAL;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = false;
	static const bool INSTANCING = false;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;

	static const int TIMER = 1;
public:
	StatusBlink(Effect* effect, Unit* target)
    : Status(effect, "Blink", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	
	// Main Function
	virtual void onPreReactHit(EventAttack* evt);
    
	~StatusBlink() {}
};

class StatusWounding : public Status
{
private:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = true;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;
public:
	StatusWounding(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Wounding", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, stacks, MAX_SINGLE_STACKS)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void applyHealingReduction(Damage* applier);
	
	// Main Function
	virtual void onPreReceiveDamage(Damage* applier);

	~StatusWounding() {}
};

class StatusProvoke : public Status
{
protected:
	static const StatusBenefit BENEFIT = NEUTRAL;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_CC;
	static const bool DISPELLABLE = false;
	static const bool INSTANCING = false;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;

	static const int TIMER = 1;

	Unit* focus; 
public:
	StatusProvoke(Effect* effect, Unit* target, Unit* focus)
        : Status(effect, "Provoke", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS), focus(focus)
	{
	}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void addToPriorityList(Targeter* system) const;
	
	// Main Function
	virtual void onPreFindTarget(Targeter* system);

	~StatusProvoke() {}
};

class StatusMarked : public Status
{
protected:
	static const StatusBenefit BENEFIT = NEUTRAL;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_CC;
	static const bool DISPELLABLE = false;
	static const bool INSTANCING = false;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;

	static const int TIMER = 1;
public:
	StatusMarked(Effect* effect, Unit* target, Unit* focus)
        : Status(effect, "Marked", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS)
	{
	}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void addToPriorityList(Targeter* system) const;
	
	// Main Function
	virtual void onPreBecomeTarget(Targeter* system);

	~StatusMarked() {}
};

class StatusBattleShout : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_STAT;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 3;

	static const int TIMER = 1;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusBattleShout(Effect* effect, Unit* target, int stacks)
        : Status(effect, "BattleShout", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, stacks), amount(AMOUNT)
	{
	}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void modifyAttributes(int dvalue);
	
	// Main Function
	virtual int onMerge(Status* status);
	virtual void onSpawn();
	virtual void onKill();

	~StatusBattleShout() {}
};

class StatusDetermination : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_STAT;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 3;

	static const int TIMER = 1;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusDetermination(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Determination", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, stacks), amount(AMOUNT)
	{
	}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void modifyAttributes(int dvalue);
	
	// Main Function
	virtual int onMerge(Status* status);
	virtual void onSpawn();
	virtual void onKill();

	~StatusDetermination() {}
};

class StatusWeaken : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_STAT;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 3;

	static const int TIMER = 1;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusWeaken(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Weaken", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, stacks), amount(AMOUNT)
	{
	}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void modifyAttributes(int dvalue);
	
	// Main Function
	virtual int onMerge(Status* status);
	virtual void onSpawn();
	virtual void onKill();

	~StatusWeaken() {}
};

class StatusResistance : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_STAT;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 3;

	static const int TIMER = 1;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusResistance(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Resistance", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, stacks), amount(AMOUNT)
	{
	}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void modifyAttributes(int dvalue);
	
	// Main Function
	virtual int onMerge(Status* status);
	virtual void onSpawn();
	virtual void onKill();

	~StatusResistance() {}
};

class StatusVulnerability : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_STAT;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 3;

	static const int TIMER = 1;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusVulnerability(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Vulnerability", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, stacks), amount(AMOUNT)
	{
	}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void modifyAttributes(int dvalue);
	
	// Main Function
	virtual int onMerge(Status* status);
	virtual void onSpawn();
	virtual void onKill();

	~StatusVulnerability() {}
};

class StatusHaste : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_STAT;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 5;
	static const int MAX_GROUP_STACKS = 5;

	static const int TIMER = 3;
	static const int AMOUNT = 1;

	int amount;
public:
	StatusHaste(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Haste", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, stacks), amount(AMOUNT)
	{
	}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void modifyAttributes(int dvalue);
	
	// Main Function
	virtual int onMerge(Status* status);
	virtual void onSpawn();
	virtual void onKill();

	~StatusHaste() {}
};

class StatusChill : public Status
{
protected:
	static const StatusBenefit BENEFIT = DEBUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_STAT;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 5;
	static const int MAX_GROUP_STACKS = 5;

	static const int TIMER = 3;
	static const int AMOUNT = 1;

	int amount;
public:
	StatusChill(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Chill", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, stacks), amount(AMOUNT)
	{
	}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	void modifyAttributes(int dvalue);
	
	// Main Function
	virtual int onMerge(Status* status);
	virtual void onSpawn();
	virtual void onKill();

	~StatusChill() {}
};

class StatusScope : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 3;

	static const int TIMER = 1;
	static const int AMOUNT = 10;

	int amount;
public:
	StatusScope(Effect* effect, Unit* target, int stacks)
        : Status(effect, "Scope", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, stacks), amount(AMOUNT)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	
	// Main Function
	virtual void onPreApplyDamage(Damage* applier);

	~StatusScope() {}
};

class StatusTangleTrap : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 3;

	static const int TIMER = 1;
	static const int AMOUNT = 10;
public:
	StatusTangleTrap(Effect* effect, Unit* target)
        : Status(effect, "TangleTrap", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }

	// Main Function
	virtual void onPreReactHit(EventAttack* evt);

	~StatusTangleTrap() {}
};

class StatusRally : public Status
{
private:
	static const StatusBenefit BENEFIT = NEUTRAL;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = false;
	static const bool INSTANCING = false;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;

	static const int TIMER = 1;
public:
	StatusRally(Effect* effect, Unit* target)
    : Status(effect, "Rally", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	
	// Main Function
	virtual void onExecuteAbility(Ability* ability);
    
	~StatusRally() {}
};

class StatusFeint : public Status
{
private:
	static const StatusBenefit BENEFIT = NEUTRAL;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = false;
	static const bool INSTANCING = false;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;

	static const int TIMER = 1;
public:
	StatusFeint(Effect* effect, Unit* target)
    : Status(effect, "Feint", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	
	// Main Function
	virtual void onExecuteAbility(Ability* ability);
    
	~StatusFeint() {}
};

class StatusQuickNock : public Status
{
private:
	static const StatusBenefit BENEFIT = NEUTRAL;
	static const StatusMatch MATCH = STATUS_SELFMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = false;
	static const bool INSTANCING = false;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 1;

	static const int TIMER = 1;
public:
	StatusQuickNock(Effect* effect, Unit* target)
    : Status(effect, "Quick Nock", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }
	
	// Main Function
	virtual void onExecuteAbility(Ability* ability);
    
	~StatusQuickNock() {}
};

class StatusConfuseTrap : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 3;

	static const int TIMER = 1;
	static const int AMOUNT = 30;
public:
	StatusConfuseTrap(Effect* effect, Unit* target)
        : Status(effect, "ConfuseTrap", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }

	// Main Function
	virtual void onPreReactHit(EventAttack* evt);

	~StatusConfuseTrap() {}
};

class StatusCharmTrap : public Status
{
protected:
	static const StatusBenefit BENEFIT = BUFF;
	static const StatusMatch MATCH = STATUS_ALLMATCHABLE;
	static const StatusCategory CATEGORY = STATUS_UNCATEGORIZED;
	static const bool DISPELLABLE = true;
	static const bool INSTANCING = true;
	static const bool COLLECTIVE = false;
	static const bool TIMED = true;
	static const int MAX_SINGLE_STACKS = 1;
	static const int MAX_GROUP_STACKS = 3;

	static const int TIMER = 1;
	static const int AMOUNT = 10;
public:
	StatusCharmTrap(Effect* effect, Unit* target)
        : Status(effect, "CharmTrap", target, BENEFIT, MATCH, CATEGORY, DISPELLABLE, INSTANCING, COLLECTIVE, TIMED, TIMER, MAX_SINGLE_STACKS)
	{}
	
	// Helper Functions
	virtual int getMaxSingleStacks() const { return MAX_SINGLE_STACKS; }
	virtual int getMaxGroupStacks() const { return MAX_GROUP_STACKS; }

	// Main Function
	virtual void onPreReactHit(EventAttack* evt);

	~StatusCharmTrap() {}
};

class StatusGroup
{
private:
	// Shared info for all instances
	string subname;
	Unit* target;
	StatusBenefit benefit;
	StatusMatch match;
	StatusCategory category;
    bool dispellable;
	bool instancing;
	bool collective;
	int maxSingleStacks;
	int maxGroupStacks;
	// The list of instances
	vector<Status*> instances;
	int totalStacks;
	bool executed;
public:
	StatusGroup(const string & subname, Unit* target = NULL, StatusBenefit benefit = NEUTRAL, StatusMatch match = STATUS_UNMATCHABLE, StatusCategory category = STATUS_UNCATEGORIZED,
		bool dispellable = true, bool instancing = false, bool collective = false, int maxSingleStacks = 0, int maxGroupStacks = 0)
		: subname(subname), target(target), benefit(benefit), 
		match(match), category(category), dispellable(dispellable), instancing(instancing), collective(collective), maxSingleStacks(maxSingleStacks), maxGroupStacks(maxGroupStacks),
		instances(), totalStacks(0), executed(false)
	{}

	string getSubname() const {
		return subname;
	}

	StatusBenefit getBenefit() const {
		return benefit;
	}
    
	StatusMatch getMatch() const {
		return match;
	}
    
	StatusCategory getCategory() const {
		return category;
	}
	
	Unit* getTarget() const {
		return target;
	}

	bool isDispellable() const {
		return dispellable;
	}
	
	bool isInstancing() const {
		return instancing;
	}
	
	bool isCollective() const {
		return collective;
	}

	vector<Status*> getInstances() const {
		return instances;
	}

	int getTotalStacks() const {
		return totalStacks;
	}

	void setTotalStacks(int value) {
		totalStacks = value;
	}
	
	bool isExecuted() const {
		return executed;
	}
	
	void setExecuted(bool value) {
		executed = value;
	}

	Status* getMatchingStatus(Status* value) const {
		for (int i = 0; i < instances.size(); ++i) {
			if (instances[i]->canMergeWith(value))
				return instances[i];
		}
		return NULL;
	}

	// Returns the additional amount of stacks actually being applied (since there is a maximum)
	//
	// Removes least recent stacks first
	int addInstance(Status* nstatus) {
		instances.push_back(nstatus);
		totalStacks += nstatus->getStacks();
		int totalRemove = 0;

		// Remove stacks until there is no more overflow
		for (int i = 0; i < instances.size() && totalStacks > maxGroupStacks; ++i) {
			int remove = instances[i]->getStacks();
			if (totalStacks - remove < maxGroupStacks)
				remove = totalStacks - maxGroupStacks;
			instances[i]->setStacks(instances[i]->getStacks() - remove);

			totalRemove += remove;
			totalStacks -= remove;
		}

		// Re-update the set of instances
		vector<Status*> temp;
		for (int i = 0; i < instances.size(); ++i)
		{
			if (instances[i]->getStacks() > 0)
				temp.push_back(instances[i]);
			else
				instances[i]->setCleaning();
		}
		instances = temp;

		return nstatus->getStacks() - totalRemove;
	}

	void removeInstance(Status* status) {
		totalStacks -= status->getStacks();
		
		instances.erase(find(instances.begin(), instances.end(), status));
	}

	~StatusGroup() {}
};

// Keeps track of ongoing Status Effects, also allows multiple status effects to be associated under one name
class Effect : public Action
{
private:
	vector<Status*> status;

	Unit* trigger; // Unit that processes Effects on its turn
public:
	Effect(Unit* source = NULL, Battle* battle = NULL, const string & name = "", Unit* trigger = NULL)
		: Action(name, EFFECT_TRIGGER, ABILITY_NONE, source, battle), status(), trigger(trigger)
	{
	}

	bool isExpired() const {
		return status.size() <= 0;
	}
	
    Unit* getTrigger() const {
        return trigger;
    }
    
	void addStatus(Status* stat) {
		status.push_back(stat);
	}

	// Each ongoing status effect is processed every round starting from the origin.
	// For example, most effects that are timed will be updated here.
	void processBeginRound() {
		for (int i = 0; i < status.size(); ++i) {
			// Process the Status effect
			status[i]->onBeginRound();
            if (status[i]->grouplist)
                status[i]->grouplist->setExecuted(true);
		}
		// Executed is reset in function that called it (diff status in groups in diff effects)
	}
	
	void processEndRound() {
		for (int i = 0; i < status.size(); ++i) {
			// Process the Status effect
			status[i]->onEndRound();
            if (status[i]->grouplist)
                status[i]->grouplist->setExecuted(true);

			// Sets the clean flag and on End effects
			if (status[i]->hasExpired())
				status[i]->onKill();
		}
		// Executed is reset in function that called it (diff status in groups in diff effects)
	}

	void resetStatusFlag() {
		// Reset Status Group has executed flag back to false
		for (int i = 0; i < status.size(); ++i) {
			if (!status[i]->hasExpired())
				status[i]->grouplist->setExecuted(false);
		}
	}

	// Cleans up any expired status effects, removing links and trigger onKill
	void cleanStatus() 
	{
		// Clean up all expired status effects
		vector<Status*> nstatus(status.size());
		int c = 0;
		for (int i = 0; i < status.size(); ++i) {
			if (!status[i]->needsCleaning()) {
				nstatus[c] = status[i];
				++c;
			}
			else {
				delete status[i];
			}
		}
		nstatus.resize(c);
		status = nstatus;
	}

	void add(Status* nstatus) {
		nstatus->onSpawn();
	}

	void merge(Status* dominant, Status* recessive) {
		dominant->onMerge(recessive);
	}

	void applyEffect()
	{
		// Find any status that can be merged on target unit
		for (int i = 0; i < status.size(); ++i) {
			status[i]->findSetMerger();
		}
		
		// Merge or add the new status effect
		for (int i = 0; i < status.size(); ++i)
		{
			if (status[i]->getMerger() != NULL)
				merge(status[i], status[i]->getMerger());
			else
				add(status[i]);
		}

		// Add the status effect to the trigger list for onRound processing
		if (trigger != NULL) trigger->addEffect(this);
	}
    
	virtual void print(ostream& out) const;

	~Effect() {}
};	

#endif

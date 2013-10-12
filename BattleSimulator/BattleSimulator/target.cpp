#include "target.h"

#include "unit.h"
#include "group.h"
#include "status.h"
#include "battle.h"

// Comparator functions to determine strongest/weakest unit
bool compareCurrentLifePreferLess(Unit* lhs, Unit* rhs) {
	return lhs->getCurrentHealth() < rhs->getCurrentHealth() ||
		(lhs->getCurrentHealth() == rhs->getCurrentHealth() && lhs->getRValue() < rhs->getRValue());
}
bool compareCurrentLifePreferMore(Unit* lhs, Unit* rhs) {
	return lhs->getCurrentHealth() > rhs->getCurrentHealth() ||
		(lhs->getCurrentHealth() == rhs->getCurrentHealth() && lhs->getRValue() > rhs->getRValue());
}
bool comparePercentLifePreferLess(Unit* lhs, Unit* rhs) {
	return lhs->getPercentHealth() < rhs->getPercentHealth() ||
		(lhs->getPercentHealth() == rhs->getPercentHealth() && lhs->getRValue() < rhs->getRValue());
}
bool comparePercentLifePreferMore(Unit* lhs, Unit* rhs) {
	return lhs->getPercentHealth() > rhs->getPercentHealth() ||
		(lhs->getPercentHealth() == rhs->getPercentHealth() && lhs->getRValue() > rhs->getRValue());
}
bool compareNumBuffsLess(Unit* lhs, Unit* rhs) {
	return lhs->getNumBuffs() < rhs->getNumBuffs() ||
		(lhs->getNumBuffs() == rhs->getNumBuffs() && lhs->getRValue() < rhs->getRValue());
}
bool compareNumBuffsMore(Unit* lhs, Unit* rhs) {
	return lhs->getNumBuffs() > rhs->getNumBuffs() ||
		(lhs->getNumBuffs() == rhs->getNumBuffs() && lhs->getRValue() > rhs->getRValue());
}
bool compareNumDebuffsLess(Unit* lhs, Unit* rhs) {
	return lhs->getNumDebuffs() < rhs->getNumDebuffs() ||
		(lhs->getNumDebuffs() == rhs->getNumDebuffs() && lhs->getRValue() < rhs->getRValue());
}
bool compareNumDebuffsMore(Unit* lhs, Unit* rhs) {
	return lhs->getNumDebuffs() > rhs->getNumDebuffs() ||
		(lhs->getNumDebuffs() == rhs->getNumDebuffs() && lhs->getRValue() > rhs->getRValue());
}

// This scans for all possible targets. In a valid battle, it should always return at least 1 unit.
// It first starts usually at a column range of 1 (startingAdjacencyRange). 
// Row range is used as the maximum number of units to count starting from the front.
// If no units exist, the column range extends 1 step further and scans the new rows for units again.
// This repeats until maximum column range.
vector<Unit*> Targeter::searchForFrontTargets(Unit* current, Battle* battle, Group* allyGroup, Group* enemyGroup, int startingAdjacencyRange, int rowRange)
{
	int adjacencyRange = startingAdjacencyRange;
	int xmin = current->getGridX() - adjacencyRange;
	int xmax = current->getGridX() + adjacencyRange;
	vector<Unit*> targets = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmin, xmax, rowRange);
	while (targets.size() <= 0 && (xmin >= 0 || xmax < enemyGroup->getWidth() - 1))
	{
		++adjacencyRange;
		xmin = current->getGridX() - adjacencyRange;
		xmax = current->getGridX() + adjacencyRange;
		vector<Unit*> uLeft = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmin, xmin, rowRange);
		vector<Unit*> uRight = allyGroup->enemyUnitsFurthestInFront(enemyGroup, xmax, xmax, rowRange);
		for (int i = 0; i < uLeft.size(); ++i)
			targets.push_back(uLeft[i]);
		for (int i = 0; i < uRight.size(); ++i)
			targets.push_back(uRight[i]);
	} 
	return targets;
}

bool Targeter::canReach(Unit* current, Unit* target, Battle* battle, int startingAdjacencyRange, int rowRange)
{
	if (!current || !target)
		return false;
	if (current->getGrid() == target->getGrid()) // If on same group, it is always reachable
		return true;
	Group* allyGroup = battle->getAllyGroup(current->getGrid());
	Group* enemyGroup = battle->getEnemyGroup(current->getGrid());

	vector<Unit*> reachables = Targeter::searchForFrontTargets(current, battle, allyGroup, enemyGroup, startingAdjacencyRange, rowRange);
	return find(reachables.begin(), reachables.end(), target) != reachables.end();
}

void Targeter::set(Battle* battle, int n) 
{
	chosen.clear();
    
	// OnPreTarget Methods
    for (int i = 0; i < candidates.size(); ++i)
	{
		candidates[i]->activateOnPreBecomeTarget(this);
		battle->getGlobalTrigger()->activateOnPreBecomeTarget(this);
	}
	ref->getSource()->activateOnPreFindTarget(this);
	battle->getGlobalTrigger()->activateOnPreFindTarget(this);

	vector<Unit*> cand = candidates;
	vector<int> prio = priorities;

	// Add all priority targets to the chosen list (taunt targets)
	// uses TARGET_RANDOM if multiple priorities exist
    int ni = 0;
	for (; ni < n && ni < prio.size() && method != TARGET_CONFUSED; ++ni) {
		int randIndex = rand() % prio.size();
		int candIndex = prio[randIndex];
		chosen.push_back(cand[candIndex]);
		prio[randIndex] = prio[prio.size() - 1];
		cand[candIndex] = cand[cand.size() - 1];
		prio.pop_back();
		cand.pop_back();
	}

	// Choose candidates
	switch (method)
	{
    case TARGET_CONFUSED:
        {
			for (; ni < n && cand.size() > 0; ++ni) {
				int randIndex = rand() % cand.size();
				chosen.push_back(cand[randIndex]);
				cand[randIndex] = cand[cand.size() - 1];
				cand.pop_back();
			}
			break;
        }
    case TARGET_CHARMED:
        {
			for (; ni < n && cand.size() > 0; ++ni) {
				int randIndex = rand() % cand.size();
				chosen.push_back(cand[randIndex]);
				cand[randIndex] = cand[cand.size() - 1];
				cand.pop_back();
			}
			break;
        }
	case TARGET_RANDOM:
		{
			for (; ni < n && cand.size() > 0; ++ni) {
				int randIndex = rand() % cand.size();
				chosen.push_back(cand[randIndex]);
				cand[randIndex] = cand[cand.size() - 1];
				cand.pop_back();
			}
			break;
		}
	case TARGET_WEAKEST:
		{
			for (int i = 0; i < cand.size(); ++i)
				cand[i]->setRValue();
			sort(cand.begin(), cand.end(), compareCurrentLifePreferLess);
			for (; ni < n && cand.size() > 0; ++ni) {
				chosen.push_back(cand[ni]);
			}
			break;
		}
	case TARGET_STRONGEST:
		{
			for (int i = 0; i < cand.size(); ++i)
				cand[i]->setRValue();
			sort(cand.begin(), cand.end(), compareCurrentLifePreferMore);
			for (; ni < n && cand.size() > 0; ++ni)
				chosen.push_back(cand[ni]);
			break;
		}
	case TARGET_LEADER:
		{
			// Select the leader first from the candidate list
			for (int i = 0; i < cand.size(); ++i)
				if (cand[i]->isLeader())
				{
					chosen.push_back(cand[i]);
					cand[i] = cand[cand.size() - 1];
					cand.pop_back();
					++ni;

					break;
				}
			for (; ni < n && cand.size() > 0; ++ni) {
				int randIndex = rand() % cand.size();
				chosen.push_back(cand[randIndex]);
				cand[randIndex] = cand[cand.size() - 1];
				cand.pop_back();
			}
			break;
		}
	case TARGET_HEALING:
		{
			for (int i = 0; i < cand.size(); ++i)
				cand[i]->setRValue();
			sort(cand.begin(), cand.end(), comparePercentLifePreferLess);
			for (; ni < n && cand.size() > 0; ++ni)
				chosen.push_back(cand[ni]);
			break;
		}
	case TARGET_MOST_DEBUFFS:
		{
			for (int i = 0; i < cand.size(); ++i)
				cand[i]->setRValue();
			sort(cand.begin(), cand.end(), compareNumDebuffsMore);
			for (; ni < n && cand.size() > 0; ++ni)
				chosen.push_back(cand[ni]);
			break;
		}
	case TARGET_MOST_BUFFS:
		{
			for (int i = 0; i < cand.size(); ++i)
				cand[i]->setRValue();
			sort(cand.begin(), cand.end(), compareNumBuffsMore);
			for (; ni < n && cand.size() > 0; ++ni)
				chosen.push_back(cand[ni]);
			break;
		}
	}
            
    // OnPostTarget Methods
	battle->getGlobalTrigger()->activateOnPostFindTarget(this);
	ref->getSource()->activateOnPostFindTarget(this);
    for (int i = 0; i < chosen.size(); ++i)
	{
		battle->getGlobalTrigger()->activateOnPostBecomeTarget(this);
		chosen[i]->activateOnPostBecomeTarget(this);
	}
}

void Targeter::print() const
{
    cout << "Targeted: ";
    for (int i = 0; i < chosen.size(); ++i)
        cout << chosen[i]->getName() << " ";
    cout << endl;
}

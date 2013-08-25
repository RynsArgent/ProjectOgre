#include "target.h"

#include "unit.h"
#include "group.h"
#include "status.h"

// Comparator functions to determine strongest/weakest unit
bool compareLifePreferLess(Unit* lhs, Unit* rhs) {
	return lhs->getCurrentHealth() < rhs->getCurrentHealth() ||
		(lhs->getCurrentHealth() == rhs->getCurrentHealth() && lhs->getRValue() < rhs->getRValue());
}
bool compareLifePreferMore(Unit* lhs, Unit* rhs) {
	return lhs->getCurrentHealth() > rhs->getCurrentHealth() ||
		(lhs->getCurrentHealth() == rhs->getCurrentHealth() && lhs->getRValue() > rhs->getRValue());
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

void Targeter::set(int n) 
{
	chosen.clear();
    
	// OnPreTarget Methods
    for (int i = 0; i < candidates.size(); ++i)
    {
        for (int j = 0; j < candidates[i]->getCurrentStatus().size(); ++j)
        {
            Status* status = candidates[i]->getCurrentStatus()[j];
            status->onPreBecomeTarget(this);
        }
    }
	for (int i = 0; i < ref->getSource()->getCurrentStatus().size(); ++i)
	{
		Status* status = ref->getSource()->getCurrentStatus()[i];
		status->onPreFindTarget(this);
	}

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
			sort(cand.begin(), cand.end(), compareLifePreferLess);
			for (; ni < n && cand.size() > 0; ++ni) {
				chosen.push_back(cand[ni]);
			}
			break;
		}
	case TARGET_STRONGEST:
		{
			for (int i = 0; i < cand.size(); ++i)
				cand[i]->setRValue();
			sort(cand.begin(), cand.end(), compareLifePreferMore);
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
	for (int i = 0; i < ref->getSource()->getCurrentStatus().size(); ++i)
	{
		Status* status = ref->getSource()->getCurrentStatus()[i];
		status->onPostFindTarget(this);
	}
    for (int i = 0; i < chosen.size(); ++i)
    {
        for (int j = 0; j < chosen[i]->getCurrentStatus().size(); ++j)
        {
            Status* status = chosen[i]->getCurrentStatus()[j];
            status->onPostBecomeTarget(this);
        }
    }
}

void Targeter::print() const
{
    cout << "Targeted: ";
    for (int i = 0; i < chosen.size(); ++i)
        cout << chosen[i]->getName() << " ";
    cout << endl;
}

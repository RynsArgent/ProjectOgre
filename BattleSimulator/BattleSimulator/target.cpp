#include "target.h"

#include "unit.h"
#include "status.h"

// Comparator functions to determine strongest/weakest unit
bool compareLifePreferLess(Unit* lhs, Unit* rhs) {
	return lhs->getCurrentHealth() < rhs->getCurrentHealth();
}
bool compareLifePreferMore(Unit* lhs, Unit* rhs) {
	return lhs->getCurrentHealth() > rhs->getCurrentHealth();
}

void Targeter::set(int n) {
	chosen.clear();

	// OnPreTarget Methods
	for (int i = 0; i < caster->getCurrentStatus().size(); ++i)
	{
		Status* status = caster->getCurrentStatus()[i];
		status->onPreTarget(this);
	}

	vector<Unit*> cand = candidates;
	vector<int> prio = priorities;

	for (int i = 0; i < n && i < prio.size(); ++i) {
		int randIndex = rand() % prio.size();
		int candIndex = prio[randIndex];
		chosen.push_back(cand[candIndex]);
		prio[randIndex] = prio[prio.size() - 1];
		cand[candIndex] = cand[cand.size() - 1];
		prio.pop_back();
		cand.pop_back();
	}

	switch (method)
	{
	case TARGET_RANDOM:
		{
			for (int i = 0; i < n && cand.size() > 0; ++i) {
				int randIndex = rand() % cand.size();
				chosen.push_back(cand[randIndex]);
				cand[randIndex] = cand[cand.size() - 1];
				cand.pop_back();
			}
			break;
		}
	case TARGET_WEAKEST:
		{
			sort(cand.begin(), cand.end(), compareLifePreferLess);
			for (int i = 0; i < n && i < cand.size(); ++i)
				chosen.push_back(cand[i]);
			break;
		}
	case TARGET_STRONGEST:
		{
			sort(cand.begin(), cand.end(), compareLifePreferMore);
			for (int i = 0; i < n && i < cand.size(); ++i)
				chosen.push_back(cand[i]);
			break;
		}
	}

	// OnPostTarget Methods
	for (int i = 0; i < caster->getCurrentStatus().size(); ++i)
	{
		Status* status = caster->getCurrentStatus()[i];
		status->onPostTarget(this);
	}
}

#ifndef __FORMATION_H__
#define __FORMATION_H__

#include "pch.h"

#include "character.h"
#include "gridpoint.h"
#include <vector>

// This is setup for organizing Characters without being instanced

class Formation
{
private:
	static const int FORMATION_WIDTH = 3;
	static const int FORMATION_HEIGHT = 3;

	int width;
	int height;

	vector<vector<Character*> > grid;
	GridPoint leaderPosition;
	TargetType targetOrder;
public:
	Formation();

	int getWidth() const {
		return width;
	}
	
	int getHeight() const {
		return height;
	}

	Character* getCharacterAt(int x, int y) const {
		return grid[x][y];
	}
    
	Character* getCharacterAt(const GridPoint & p) const {
		return grid[p.x][p.y];
	}
    
	void setCharacterAt(int x, int y, Character* character, int backSkillIndex = 0, int midSkillIndex = 0, int frontSkillIndex = 0, int basicSkillIndex = 0) {
		grid[x][y] = character;
		if (character == NULL)
			return;
		character->setBackSkillIndex(backSkillIndex);
		character->setMidSkillIndex(midSkillIndex);
		character->setFrontSkillIndex(frontSkillIndex);
		character->setBasicSkillIndex(basicSkillIndex);
	}
    
	void setCharacterAt(const GridPoint & p, Character* character, int backSkillIndex = 0, int midSkillIndex = 0, int frontSkillIndex = 0, int basicSkillIndex = 0) {
		grid[p.x][p.y] = character;
		if (character == NULL)
			return;
		character->setBackSkillIndex(backSkillIndex);
		character->setMidSkillIndex(midSkillIndex);
		character->setFrontSkillIndex(frontSkillIndex);
		character->setBasicSkillIndex(basicSkillIndex);
	}
	
	GridPoint getLeaderPosition() const {
		return leaderPosition;
	}
	
	void setLeaderPosition(int x, int y) {
		leaderPosition = GridPoint(x, y);
	}

	void setLeaderPosition(const GridPoint & value) {
		leaderPosition = value;
	}

	vector<Character*> getCharacters() const {
		vector<Character*> ret;
		for (int i = 0; i < grid.size(); ++i)
			for (int j = 0; j < grid[i].size(); ++j) {
				if (grid[i][j] != NULL)
					ret.push_back(grid[i][j]);
			}
		return ret;
	}

	Character* getLeader() const {
		if (leaderPosition.x < 0 || leaderPosition.y < 0)
			return NULL;
		return grid[leaderPosition.x][leaderPosition.y];
	}
    
	void setTargetOrder(TargetType targetOrder) {
		this->targetOrder = targetOrder;
	}

	TargetType getTargetOrder() const {
		return targetOrder;
	}

	~Formation() {}
};

#endif

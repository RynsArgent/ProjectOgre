#ifndef __FORMATION_H__
#define __FORMATION_H__

#include "pch.h"

#include <vector>
#include "character.h"
#include "gridpoint.h"

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
    
	void setCharacterAt(int x, int y, Character* character, Skill backSkill, Skill midSkill, Skill frontSkill) {
		grid[x][y] = character;
		character->setBackSkill(backSkill);
		character->setMidSkill(midSkill);
		character->setFrontSkill(frontSkill);
	}
    
	void setCharacterAt(const GridPoint & p, Character* character, Skill backSkill, Skill midSkill, Skill frontSkill) {
		grid[p.x][p.y] = character;
		character->setBackSkill(backSkill);
		character->setMidSkill(midSkill);
		character->setFrontSkill(frontSkill);
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

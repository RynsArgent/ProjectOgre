#ifndef __FORMATION_H__
#define __FORMATION_H__

#include "pch.h"

#include <vector>
#include "character.h"

class Formation
{
private:
	static const int FORMATION_WIDTH = 3;
	static const int FORMATION_HEIGHT = 3;

	int width;
	int height;

	vector<vector<Character*> > grid;
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

	void setCharacterAt(int x, int y, Character* character) {
		grid[x][y] = character;
	}
	
	void setCharacterAt(int x, int y, Character* character, Skill backSkill, Skill midSkill, Skill frontSkill) {
		grid[x][y] = character;
		character->setBackSkill(backSkill);
		character->setMidSkill(midSkill);
		character->setFrontSkill(frontSkill);
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

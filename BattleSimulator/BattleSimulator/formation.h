#pragma once

#include <vector>
#include "character.h"

class Formation
{
private:
	static const int FORMATION_WIDTH = 3;
	static const int FORMATION_HEIGHT = 3;

	int width;
	int height;

	std::vector<std::vector<Character*> > grid;
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
};

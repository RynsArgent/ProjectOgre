#include "formation.h"

Formation::Formation()
	: width(FORMATION_WIDTH), height(FORMATION_HEIGHT), grid(width, std::vector<Character*>(height, NULL))
{
}
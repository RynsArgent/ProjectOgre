#pragma once

#include "group.h"

class Battle
{
private:
	Group group1;
	Group group2;
public:
	Battle(Formation *form1, Formation *form2);
};


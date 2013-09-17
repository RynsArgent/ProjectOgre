#ifndef __SETUP_H__
#define __SETUP_H__

#include "pch.h"

class Setup
{
private:
	Formation* formA;
	Formation* formB;
public:
	Setup(Formation* A, Formation* B) 
		: formA(A), formB(B) 
	{
	}

	~Setup() {}

	friend class Renderer;
};

#endif

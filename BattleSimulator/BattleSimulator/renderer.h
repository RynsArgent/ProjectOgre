#pragma once

#ifdef IN_WINDOWS
#include <Windows.h>
#endif

#ifdef IN_MAC 
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "pch.h"

class Renderer
{
private:
	double edgeLeft;
	double edgeRight;
	double edgeTop;
	double edgeBottom;
public:
	//Outputs a string in graphics to point p with str and color (r,g,b) and font
	static void GLoutputString(double x, double y, const string & str, 
							double cr = 1.0, double cg = 1.0, double cb = 1.0,
							void* font = GLUT_BITMAP_HELVETICA_18);

	Renderer(double edgeLeft, double edgeRight, double edgeTop, double edgeBottom);
	
	double width() const { return edgeBottom - edgeTop; }
	double height() const { return edgeRight - edgeLeft; }

	void render(Unit* unit, double centerx, double centery, double width, double height) const;
	void render(Group* group, Direction dir, double centerx, double centery, double width, double height) const;
	void render(Battle* battle) const;


	~Renderer();
};

#include "renderer.h"

#include "unit.h"
#include "group.h"
#include "battle.h"
#include "event.h"
#include "action.h"
#include <sstream>
#include <cmath>
#include <vector>

Renderer::Renderer(double edgeLeft, double edgeRight, double edgeTop, double edgeBottom)
	: edgeLeft(edgeLeft), edgeRight(edgeRight), edgeTop(edgeTop), edgeBottom(edgeBottom), battle(NULL)
{
}

//Outputs a string in graphics to point p with str and color (r,g,b) and font
void Renderer::GLoutputString(double x, double y, const string & str, 
								double cr, double cg, double cb,
								void* font)
{
	glColor3d(cr, cg, cb);
	glRasterPos2f(x, y);
	for (unsigned i = 0; i < str.length(); i++) 
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void Renderer::GLoutputString(double x, double y, const string & str, 
								const Color & col,
								void* font)
{
	glColor3d(col.r, col.g, col.b);
	glRasterPos2f(x, y);
	for (unsigned i = 0; i < str.length(); i++) 
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void Renderer::render(Unit* unit, double centerx, double centery, double width, double height)
{
	double xpos, ypos;

	xpos = centerx - width / 2;
	ypos = centery - height / 2;
	double barWidth = width;
	double barHeight = height / 4;
	double percentHealth = static_cast<double>(unit->getCurrentHealth()) / unit->getMaxHealth();
	double scaledWidth = barWidth * percentHealth;
	glBegin(GL_QUADS);
	glColor4d(1.0, 1.0, 1.0, 0.0);
	glVertex2d(centerx - width / 2, centery - height / 2);
	glVertex2d(centerx + width / 2, centery - height / 2);
	glVertex2d(centerx + width / 2, centery + height / 2);
	glVertex2d(centerx - width / 2, centery + height / 2);
	if (percentHealth >= 0.5)
		glColor4d(1 - (percentHealth - 0.5) / 0.5, 1.0, 0.0, 0.0);
	else
		glColor4d(1.0, percentHealth / 0.5, 0.0, 0.0);
	glVertex2d(xpos, ypos);
	glVertex2d(xpos + scaledWidth, ypos);
	glVertex2d(xpos + scaledWidth, ypos + barHeight);
	glVertex2d(xpos, ypos + barHeight);
	glEnd();

	Color colText;
	if (battle->mainUnit && battle->mainUnit == unit)
		colText = Color(0.0, 1.0, 0.0);
	else
	{
		if (unit->isDone())
			colText = Color(0.2, 0.2, 0.2);
		else
			colText = Color(0.0, 0.0, 1.0);
	}
	string descLeader = unit->isLeader() ? "***" : "";
	string descHP = toStringInt(unit->getCurrentHealth()) + "/" + toStringInt(unit->getMaxHealth());
	string descName = unit->getName();
	GLoutputString(xpos + max(barWidth * 0.30, scaledWidth) / 2 - width / 10, ypos + 1 * barHeight, descLeader, colText);
	GLoutputString(xpos + width / 10, ypos + 2 * barHeight, descHP, colText);
	GLoutputString(xpos + width / 10, ypos + 3 * barHeight, descName, colText);
}

void Renderer::render(Group* group, Direction dir, double centerx, double centery, double width, double height)
{
	vector<Unit*> units = group->allyUnits();
	double dx = width / (group->getWidth());
	double dy = height / (group->getHeight());
	double unitwidth = dx / 1.20;
	double unitheight = dy / 1.20;
	double startingx = centerx - width / 2;
	double startingy = centery - height / 2;
	for (int i = 0; i < units.size(); ++i)
	{
		Unit* unit = units[i];
		int xmod;
		int ymod;
		switch (dir)
		{
			case DIRECTION_NORTH:
				xmod = unit->getGridX();
				ymod = unit->getGridY();
				break;
			case DIRECTION_WEST:
				xmod = unit->getGridY();
				ymod = group->getHeight() - unit->getGridX() - 1;
				break;
			case DIRECTION_EAST:
				xmod = group->getWidth() - unit->getGridY() - 1;
				ymod = unit->getGridX();
				break;
			case DIRECTION_SOUTH:
				xmod = group->getWidth() - unit->getGridX() - 1;
				ymod = group->getHeight() - unit->getGridY() - 1;
				break;
			default:
				xmod = unit->getGridX();
				ymod = unit->getGridY();
				break;
		}
		double xpos = startingx + xmod * dx + dx / 2;
		double ypos = startingy + ymod * dy + dx / 2;
		render(unit, xpos, ypos, unitwidth, unitheight);
	}
}

void Renderer::render(Battle* battle)
{
	this->battle = battle;

	render(battle->group1, DIRECTION_NORTH, 0.75, 0.75, 0.40, 0.40);
	render(battle->group2, DIRECTION_SOUTH, 0.75, 0.25, 0.40, 0.40);

	for (int i = 0; i < battle->eventStack.size(); ++i) {
		stringstream ss;
		battle->eventStack[i]->print(ss);
		GLoutputString(0, 0.05 + 0.05 * i, ss.str(), 1.0, 1.0, 1.0, GLUT_BITMAP_HELVETICA_12);
	}
	GLoutputString(0.01, 0.99, "seed: " + toStringInt(battle->seed), 1.0, 1.0, 1.0, GLUT_BITMAP_HELVETICA_12);
}

Renderer::~Renderer()
{
}

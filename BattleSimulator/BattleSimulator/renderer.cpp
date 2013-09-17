#include "renderer.h"

#include "character.h"
#include "formation.h"
#include "unit.h"
#include "group.h"
#include "battle.h"
#include "event.h"
#include "action.h"
#include <sstream>
#include <cmath>
#include <vector>

extern bool change;

void InfoBox::setHighlighted(bool value)
{
	if (highlighted == value)
		return;
	highlighted = value;
	change = true;
}

Renderer::Renderer(double edgeLeft, double edgeRight, double edgeTop, double edgeBottom)
	: edgeLeft(edgeLeft), edgeRight(edgeRight), edgeTop(edgeTop), edgeBottom(edgeBottom),
	selected(NULL)
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

///////////////////////
///	Setup Functions ///
///////////////////////
void Renderer::setCharacterBox(CharacterInfoBox* container, Character* character, double centerx, double centery, double width, double height)
{
	container->info = character;
	double xpos = centerx - width / 2;
	double ypos = centery - height / 2;
	container->box = Rect2D(Point2D(xpos, ypos), width, height, WHITE, true);
}

void Renderer::setFormationBox(FormationInfoBox* container, Formation* form, Direction dir, double centerx, double centery, double width, double height)
{
	container->info = form;

	double dx = width / (form->getWidth());
	double dy = height / (form->getHeight());
	double unitwidth = dx / 1.20;
	double unitheight = dy / 1.20;
	double startingx = centerx - width / 2;
	double startingy = centery - height / 2;

	container->box = Rect2D(Point2D(startingx, startingy), width, height);
	if (dir == DIRECTION_SOUTH)
		container->box.color = Color(1.0, 0.5, 0.5);
	else if (dir == DIRECTION_NORTH)
		container->box.color = Color(0.5, 0.5, 1.0);
	container->box.filled = true;
	
	container->characterInfos = vector<vector<CharacterInfoBox> >(form->getWidth(), vector<CharacterInfoBox>(form->getHeight()));
	for (int i = 0; i < form->getWidth(); ++i)
		for (int j = 0; j < form->getHeight(); ++j)
		{
			// can be null
			Character* character = form->getCharacterAt(i, j);

			int xmod;
			int ymod;
			switch (dir)
			{
				case DIRECTION_NORTH:
					xmod = i;
					ymod = j;
					break;
				case DIRECTION_WEST:
					xmod = j;
					ymod = form->getHeight() - i - 1;
					break;
				case DIRECTION_EAST:
					xmod = form->getWidth() - j - 1;
					ymod = i;
					break;
				case DIRECTION_SOUTH:
					xmod = form->getWidth() - i - 1;
					ymod = form->getHeight() - j - 1;
					break;
				default:
					xmod = i;
					ymod = j;
					break;
			}
			double xpos = startingx + xmod * dx + dx / 2;
			double ypos = startingy + ymod * dy + dx / 2;
			setCharacterBox(&container->characterInfos[i][j], character, xpos, ypos, unitwidth, unitheight);
		}
}

void Renderer::initSetupRenderer(Setup* setup)
{
	setupInfo.info = setup;

	double w = (edgeRight - edgeLeft) / 2;
	double h = (edgeBottom - edgeTop);
	setupInfo.box = Rect2D(Point2D((edgeLeft + edgeRight) / 2, edgeTop), w, h, GRAY, true); 
	
	setFormationBox(&setupInfo.formAInfo, setup->formA, DIRECTION_NORTH, 0.75, 0.75, 0.40, 0.40);
	setFormationBox(&setupInfo.formBInfo, setup->formB, DIRECTION_SOUTH, 0.75, 0.25, 0.40, 0.40);
}

void Renderer::renderSetup(CharacterInfoBox* characterBox)
{
	Rect2D box = characterBox->box;
	Character* character = characterBox->info;
	
	box.render();
	if (selected == characterBox)
		Rect2D(characterBox->box.p, characterBox->box.width, characterBox->box.height, RED, false).render();
	else if (characterBox->highlighted) 
		Rect2D(characterBox->box.p, characterBox->box.width, characterBox->box.height, YELLOW, false).render();

	if (character != NULL) {
		Color colText = BLUE;

		string descName = character->getName();
		GLoutputString(box.p.x + box.width / 10, box.p.y + 3 * (box.height / 10), descName, colText);
	}
}

void Renderer::renderSetup(FormationInfoBox* formBox)
{
	Rect2D box = formBox->box;
	Formation* form = formBox->info;

	box.render();
	if (formBox->highlighted)
		Rect2D(formBox->box.p, formBox->box.width, formBox->box.height, YELLOW, false).render();
	for (int i = 0; i < formBox->characterInfos.size(); ++i)
		for (int j = 0; j < formBox->characterInfos[i].size(); ++j)
		{
			renderSetup(&formBox->characterInfos[i][j]);
		}
}

void Renderer::renderSetup()
{
	Rect2D box = setupInfo.box;
	Setup* setup = setupInfo.info;

	box.render();
	if (setupInfo.highlighted)
		Rect2D(box.p, box.width, box.height, YELLOW, false).render();

	renderSetup(&setupInfo.formAInfo);
	renderSetup(&setupInfo.formBInfo);
}

void Renderer::processMouseClickSetup(const Point2D & loc)
{
	for (int i = 0; i < setupInfo.formAInfo.characterInfos.size(); ++i)
		for (int j = 0; j < setupInfo.formAInfo.characterInfos[i].size(); ++j)
		{
			setupInfo.formAInfo.characterInfos[i][j].setHighlighted(
				setupInfo.formAInfo.characterInfos[i][j].box.contains(loc));
			if (setupInfo.formAInfo.characterInfos[i][j].highlighted) {
				if (selected == &setupInfo.formAInfo.characterInfos[i][j])
					selected = NULL;
				else
					selected = &setupInfo.formAInfo.characterInfos[i][j];
			}
		}
				
	for (int i = 0; i < setupInfo.formBInfo.characterInfos.size(); ++i)
		for (int j = 0; j < setupInfo.formBInfo.characterInfos[i].size(); ++j)
		{
			setupInfo.formBInfo.characterInfos[i][j].setHighlighted(
				setupInfo.formBInfo.characterInfos[i][j].box.contains(loc));
			if (setupInfo.formBInfo.characterInfos[i][j].highlighted) {
				if (selected == &setupInfo.formBInfo.characterInfos[i][j])
					selected = NULL;
				else
					selected = &setupInfo.formBInfo.characterInfos[i][j];
			}
		}
}

void Renderer::processMouseMoveSetup(const Point2D & loc)
{
	setupInfo.setHighlighted(setupInfo.box.contains(loc));
	setupInfo.formAInfo.setHighlighted(setupInfo.formAInfo.box.contains(loc));
	setupInfo.formBInfo.setHighlighted(setupInfo.formBInfo.box.contains(loc));
	
	for (int i = 0; i < setupInfo.formAInfo.characterInfos.size(); ++i)
		for (int j = 0; j < setupInfo.formAInfo.characterInfos[i].size(); ++j)
			setupInfo.formAInfo.characterInfos[i][j].setHighlighted(
				setupInfo.formAInfo.characterInfos[i][j].box.contains(loc));
				
	for (int i = 0; i < setupInfo.formBInfo.characterInfos.size(); ++i)
		for (int j = 0; j < setupInfo.formBInfo.characterInfos[i].size(); ++j)
			setupInfo.formBInfo.characterInfos[i][j].setHighlighted(
				setupInfo.formBInfo.characterInfos[i][j].box.contains(loc));
}

////////////////////////
///	Battle Functions ///
////////////////////////

void Renderer::setUnitBox(UnitInfoBox* container, Unit* unit, double centerx, double centery, double width, double height)
{
	container->info = unit;
	double xpos = centerx - width / 2;
	double ypos = centery - height / 2;
	container->box = Rect2D(Point2D(xpos, ypos), width, height, WHITE, true);
}

void Renderer::setGroupBox(GroupInfoBox* container, Group* group, Direction dir, double centerx, double centery, double width, double height)
{
	container->info = group;

	double dx = width / (group->getWidth());
	double dy = height / (group->getHeight());
	double unitwidth = dx / 1.20;
	double unitheight = dy / 1.20;
	double startingx = centerx - width / 2;
	double startingy = centery - height / 2;

	container->box = Rect2D(Point2D(startingx, startingy), width, height);
	if (dir == DIRECTION_SOUTH)
		container->box.color = Color(1.0, 0.5, 0.5);
	else if (dir == DIRECTION_NORTH)
		container->box.color = Color(0.5, 0.5, 1.0);
	container->box.filled = true;
	
	container->unitInfos = vector<vector<UnitInfoBox> >(group->getWidth(), vector<UnitInfoBox>(group->getHeight()));
	for (int i = 0; i < group->getWidth(); ++i)
		for (int j = 0; j < group->getHeight(); ++j)
		{
			// can be null
			Unit* unit = group->getUnitAt(i, j);

			int xmod;
			int ymod;
			switch (dir)
			{
				case DIRECTION_NORTH:
					xmod = i;
					ymod = j;
					break;
				case DIRECTION_WEST:
					xmod = j;
					ymod = group->getHeight() - i - 1;
					break;
				case DIRECTION_EAST:
					xmod = group->getWidth() - j - 1;
					ymod = i;
					break;
				case DIRECTION_SOUTH:
					xmod = group->getWidth() - i - 1;
					ymod = group->getHeight() - j - 1;
					break;
				default:
					xmod = i;
					ymod = j;
					break;
			}
			double xpos = startingx + xmod * dx + dx / 2;
			double ypos = startingy + ymod * dy + dx / 2;
			setUnitBox(&container->unitInfos[i][j], unit, xpos, ypos, unitwidth, unitheight);
		}
}

void Renderer::initBattleRenderer(Battle* battle)
{
	battleInfo.info = battle;

	battleInfo.box = Rect2D(Point2D(edgeLeft, edgeTop), edgeRight - edgeLeft, edgeBottom - edgeTop, GRAY, true); 
	
	setGroupBox(&battleInfo.groupAInfo, battle->group1, DIRECTION_NORTH, 0.75, 0.75, 0.40, 0.40);
	setGroupBox(&battleInfo.groupBInfo, battle->group2, DIRECTION_SOUTH, 0.75, 0.25, 0.40, 0.40);
}

void Renderer::renderBattle(UnitInfoBox* unitBox)
{
	Rect2D box = unitBox->box;
	Unit* unit = unitBox->info;

	if (unit != NULL && unit->isAvailable()) {
		box.render();

		double barWidth = box.width;
		double barHeight = box.height / 4;
		double percentHealth = static_cast<double>(unit->getCurrentHealth()) / unit->getMaxHealth();
		double scaledWidth = barWidth * percentHealth;

		glBegin(GL_QUADS);
		if (percentHealth >= 0.5)
			glColor4d(1 - (percentHealth - 0.5) / 0.5, 1.0, 0.0, 0.0);
		else
			glColor4d(1.0, percentHealth / 0.5, 0.0, 0.0);
		glVertex2d(box.p.x, box.p.y);
		glVertex2d(box.p.x + scaledWidth, box.p.y);
		glVertex2d(box.p.x + scaledWidth, box.p.y + barHeight);
		glVertex2d(box.p.x, box.p.y + barHeight);
		glEnd();

		Color colText;
		if (battleInfo.info->mainUnit && battleInfo.info->mainUnit == unit)
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
		GLoutputString(box.p.x + max(barWidth * 0.30, scaledWidth) / 2 - box.width / 10, box.p.y + 1 * barHeight, descLeader, colText);
		GLoutputString(box.p.x + box.width / 10, box.p.y + 2 * barHeight, descHP, colText);
		GLoutputString(box.p.x + box.width / 10, box.p.y + 3 * barHeight, descName, colText);
	}
}

void Renderer::renderBattle(GroupInfoBox* groupBox)
{
	Rect2D box = groupBox->box;
	Group* group = groupBox->info;

	box.render();
	for (int i = 0; i < groupBox->unitInfos.size(); ++i)
		for (int j = 0; j < groupBox->unitInfos[i].size(); ++j)
		{
			renderBattle(&groupBox->unitInfos[i][j]);
		}
}

void Renderer::renderBattle()
{
	Rect2D box = battleInfo.box;
	Battle* battle = battleInfo.info;

	box.render();

	renderBattle(&battleInfo.groupAInfo);
	renderBattle(&battleInfo.groupBInfo);

	for (int i = 0; i < battleInfo.info->eventStack.size(); ++i) {
		stringstream ss;
		battle->eventStack[i]->print(ss);
		GLoutputString(0, 0.05 + 0.05 * i, ss.str(), 1.0, 1.0, 1.0, GLUT_BITMAP_HELVETICA_12);
	}
	GLoutputString(0.01, 0.99, "seed: " + toStringInt(battle->seed), 1.0, 1.0, 1.0, GLUT_BITMAP_HELVETICA_12);
}

Renderer::~Renderer()
{
}

#ifndef __RENDERER_H__
#define __RENDERER_H__

#ifdef IN_WINDOWS
#include <Windows.h>
#endif

#ifdef IN_MAC 
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "pch.h"
#include "object.h"
#include "battle.h"
#include "setup.h"

#include <vector>

struct InfoBox
{
	Rect2D box;
	bool highlighted;

	void setHighlighted(bool value);

	InfoBox() : box(), highlighted() {}
};

struct CharacterInfoBox : public InfoBox
{
	Character* info;

	CharacterInfoBox() : InfoBox(), info(NULL) {}
};

struct FormationInfoBox : public InfoBox
{
	Formation* info;
	vector<vector<CharacterInfoBox> > characterInfos;

	FormationInfoBox() : InfoBox(), info(NULL), characterInfos() {}
};

struct UnitInfoBox : public InfoBox
{
	Unit* info;
	
	UnitInfoBox() : InfoBox(), info(NULL) {}
};

struct GroupInfoBox : public InfoBox
{
	Group* info;
	vector<vector<UnitInfoBox> > unitInfos;
	
	GroupInfoBox() : InfoBox(), info(NULL), unitInfos() {}
};

struct SetupInfoBox : public InfoBox
{
	Rect2D box;
	Setup* info;

	FormationInfoBox formAInfo;
	FormationInfoBox formBInfo;

	SetupInfoBox() : InfoBox(), info(NULL), formAInfo(), formBInfo() {}
};

struct BattleInfoBox : public InfoBox
{
	Battle* info;
	GroupInfoBox groupAInfo;
	GroupInfoBox groupBInfo;

	BattleInfoBox() : InfoBox(), info(NULL), groupAInfo(), groupBInfo() {}
};

class Renderer
{
protected:
	double edgeLeft;
	double edgeRight;
	double edgeTop;
	double edgeBottom;
	
	SetupInfoBox setupInfo;
	BattleInfoBox battleInfo;

	InfoBox* selected;
public:
	//Outputs a string in graphics to point p with str and color (r,g,b) and font
	static void GLoutputString(double x, double y, const string & str, 
							double cr = 1.0, double cg = 1.0, double cb = 1.0,
							void* font = GLUT_BITMAP_HELVETICA_18);
	static void GLoutputString(double x, double y, const string & str, 
							const Color & col = Color(),
							void* font = GLUT_BITMAP_HELVETICA_18);

	Renderer(double edgeLeft, double edgeRight, double edgeTop, double edgeBottom);
	
	double width() const { return edgeBottom - edgeTop; }
	double height() const { return edgeRight - edgeLeft; }
	
	// Setup Function
	void setCharacterBox(CharacterInfoBox* container, Character* character, double centerx, double centery, double width, double height);
	void setFormationBox(FormationInfoBox* container, Formation* form, Direction dir, double centerx, double centery, double width, double height);
	void initSetupRenderer(Setup* setup);

	void renderSetup(CharacterInfoBox* characterBox);
	void renderSetup(FormationInfoBox* formBox);
	void renderSetup();

	void processMouseClickSetup(const Point2D & loc);
	void processMouseMoveSetup(const Point2D & loc);

	// Battle Function
	void setUnitBox(UnitInfoBox* container, Unit* unit, double centerx, double centery, double width, double height);
	void setGroupBox(GroupInfoBox* container, Group* group, Direction dir, double centerx, double centery, double width, double height);
	void initBattleRenderer(Battle* battle);

	void renderBattle(UnitInfoBox* unitBox);
	void renderBattle(GroupInfoBox* groupBox);
	void renderBattle();

	~Renderer();
};

#endif

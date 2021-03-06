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

struct Renderer;

Color getColor(StatusGroup* status);

struct InfoBox
{
	Rect2D box;
	bool highlighted;
	
	void setHighlighted(bool value);
	
	InfoBox() : box(), highlighted() {}
	void renderOutline(const Color & col) const;
	virtual void render(Renderer* renderer) const; 
	virtual ~InfoBox() {}
};

struct StatusInfoBox : public InfoBox
{
	StatusGroup* info;

	StatusInfoBox() : InfoBox(), info(NULL) {}
	virtual void render(Renderer* renderer) const; 
	~StatusInfoBox() {}
};

struct TextInfoBox : public InfoBox
{
	string text;
	Color textColor;
	
	TextInfoBox() : InfoBox(), text(""), textColor() {}

	virtual void render(Renderer* renderer) const; 
	~TextInfoBox() {}
};

struct JobInfoBox : public InfoBox
{
	JobType info;
	Color textColor;
	
	JobInfoBox() : InfoBox(), info(JOB_NONE), textColor() {}

	virtual void render(Renderer* renderer) const; 
	~JobInfoBox() {}
};

struct CharacterInfoBox : public InfoBox
{
	Character* info;
	int x;
	int y;
	bool form;
	bool leader;

	CharacterInfoBox() : InfoBox(), info(NULL), x(-1), y(-1), form(false), leader(false) {}
	virtual void render(Renderer* renderer) const; 
	~CharacterInfoBox() {}
};

struct FormationInfoBox : public InfoBox
{
	Formation* info;
	vector<vector<CharacterInfoBox> > characterInfos;

	TextInfoBox targetInfo;

	FormationInfoBox() : InfoBox(), info(NULL), characterInfos(), targetInfo() {}
	virtual void render(Renderer* renderer) const; 
	~FormationInfoBox() {}
};

struct UnitInfoBox : public InfoBox
{
	Unit* info;
	
	vector<StatusInfoBox> statusInfos;

	UnitInfoBox() : InfoBox(), info(NULL), statusInfos() {}
	virtual void render(Renderer* renderer) const; 
	~UnitInfoBox() {}
};

struct GroupInfoBox : public InfoBox
{
	Group* info;
	vector<vector<UnitInfoBox> > unitInfos;
	
	GroupInfoBox() : InfoBox(), info(NULL), unitInfos() {}
	virtual void render(Renderer* renderer) const; 
	~GroupInfoBox() {}
};

struct SideboardInfoBox : public InfoBox
{
	static const int SB_WIDTH = 3;
	static const int SB_HEIGHT = 4;

	static const int ELE_WIDTH = 6;
	
	static const int JOB_WIDTH = 2;
	static const int JOB_HEIGHT = 4;

	Character* info;
	
	vector<vector<TextInfoBox> > skillboxes;
	vector<InfoBox> elementboxes;

	vector<vector<JobInfoBox> > jobboxes;
	int jobindex;
	TextInfoBox leftbox;
	TextInfoBox rightbox;

	SideboardInfoBox() : InfoBox(), info(NULL), skillboxes(), elementboxes(), jobboxes(), jobindex(0), leftbox(), rightbox() {}
	virtual void render(Renderer* renderer) const;
	~SideboardInfoBox() {}
};

struct SetupInfoBox : public InfoBox
{
	Setup* info;
	SideboardInfoBox sideboardInfo;
	FormationInfoBox formAInfo;
	FormationInfoBox formBInfo;
	
	TextInfoBox playBox;
	TextInfoBox loadBox;
	TextInfoBox saveBox;

	bool done;

	SetupInfoBox() : InfoBox(), info(NULL), sideboardInfo(), formAInfo(), formBInfo(), playBox(), done(false) {}
	virtual void render(Renderer* renderer) const;
	~SetupInfoBox() {} 
};

struct BattleInfoBox : public InfoBox
{
	Battle* info;
	SideboardInfoBox sideboardInfo;
	GroupInfoBox groupAInfo;
	GroupInfoBox groupBInfo;

	BattleInfoBox() : InfoBox(), info(NULL),  sideboardInfo(), groupAInfo(), groupBInfo() {}
	virtual void render(Renderer* renderer) const; 
	~BattleInfoBox() {} 
};

struct Renderer
{
	int pixelWidth;
	int pixelHeight;

	double edgeLeft;
	double edgeRight;
	double edgeTop;
	double edgeBottom;
	
	SetupInfoBox setupInfo;
	BattleInfoBox battleInfo;
	
	InfoBox* mouseover;
	InfoBox* selected;

	//Outputs a string in graphics to point p with str and color (r,g,b) and font
	void GLoutputString12(double x, double y, const string & str,
							double cr = 1.0, double cg = 1.0, double cb = 1.0, bool centered = false);
	void GLoutputString12(const Point2D & p, const string & str, 
							const Color & col = Color(), bool centered = false);
	void GLoutputString18(double x, double y, const string & str,
							double cr = 1.0, double cg = 1.0, double cb = 1.0, bool centered = false);
	void GLoutputString18(const Point2D & p, const string & str, 
							const Color & col = Color(), bool centered = false);

	Renderer(int pixelWidth, int pixelHeight, double edgeLeft, double edgeRight, double edgeTop, double edgeBottom);
	
	double width() const { return edgeBottom - edgeTop; }
	double height() const { return edgeRight - edgeLeft; }
	
	// Setup Function
	void setSideboardBox(SideboardInfoBox* container, Character* character);
	void setCharacterBox(CharacterInfoBox* container, Character* character, double centerx, double centery, double width, double height);
	void setFormationBox(FormationInfoBox* container, Formation* form, Direction dir, double centerx, double centery, double width, double height);
	void initSetupRenderer(Setup* setup);
	void renderSetup();

	void load(const string & filename);
	void save(const string & filename);

	void processMouseLeftClickSetup(const Point2D & loc);
	void processMouseRightClickSetup(const Point2D & loc);
	void processMouseMoveSetup(const Point2D & loc);

	// Battle Function
	void setUnitBox(UnitInfoBox* container, Unit* unit, double centerx, double centery, double width, double height);
	void setGroupBox(GroupInfoBox* container, Group* group, Direction dir, double centerx, double centery, double width, double height);
	void initBattleRenderer(Battle* battle);
	void renderBattle();
	
	void processMouseMoveBattle(const Point2D & loc);

	~Renderer();
};

#endif

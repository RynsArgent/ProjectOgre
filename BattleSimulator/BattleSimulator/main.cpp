#ifdef IN_WINDOWS
#include <Windows.h>
#endif

#ifdef IN_MAC 
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "pch.h"

#include <ctime>
#include <cstdlib>
#include "character.h"
#include "formation.h"
#include "unit.h"
#include "group.h"
#include "battle.h"
#include "setup.h"
#include "object.h"
#include "renderer.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const double VIEWPORT_LEFT = 0.0;
const double VIEWPORT_RIGHT = 1.0;
const double VIEWPORT_TOP = 0.0;
const double VIEWPORT_BOTTOM = 1.0;

bool change = false;

Formation* formA = new Formation();
Formation* formB = new Formation();

enum Mode { MODE_SETUP, MODE_BATTLE };
Mode mode = MODE_SETUP;

Setup* setup = NULL;

Group* groupA = NULL;
Group* groupB = NULL;

Battle* battle = NULL;

Renderer* renderer = new Renderer(VIEWPORT_LEFT, VIEWPORT_RIGHT, VIEWPORT_TOP, VIEWPORT_BOTTOM);

void initialize() {
	//int seed = 1377511528;
	int seed = time(0);
	srand(seed);
	/*
		*********GROUP A*********
		FRONT	[0,0] [1,0] [2,0]
		MIDDLE	[0,1] [1,1] [2,1]
		BACK	[0,2] [1,2] [2,2]
	*/
	formA->setCharacterAt(0, 0, new Acolyte("acolyte1B"), 0, 0, 0);
	formA->setCharacterAt(2, 0, new Acolyte("acolyte1C"), 0, 0, 0);
	formA->setCharacterAt(1, 0, new Scout("scout1A"), 0, 0, 0);
	formA->setCharacterAt(1, 1, new Scout("scout2A"), 0, 0, 0);
	formA->setCharacterAt(1, 2, new Acolyte("acolyte1A"), 0, 0, 0);
	formA->setLeaderPosition(1, 2);
	formA->setTargetOrder(TARGET_LEADER);

	/*
		*********GROUP B*********
		BACK	[2,2] [1,2] [0,2]
		MIDDLE	[2,1] [1,1] [0,1]
		FRONT	[2,0] [1,0] [0,0]
	*/
	formB->setCharacterAt(1, 2, new Fighter("fighter1B"), 1, 0, 0);
	formB->setCharacterAt(0, 0, new Mage("mage1B", ELEMENT_WATER), 0, 0, 0);
	formB->setCharacterAt(2, 0, new Mage("mage2B", ELEMENT_WATER), 0, 0, 0);
	formB->setCharacterAt(0, 1, new Mage("mage3B", ELEMENT_WATER), 0, 0, 0);
	formB->setCharacterAt(2, 1, new Mage("mage4B", ELEMENT_WATER), 0, 0, 0);
	formB->setLeaderPosition(1, 2);
	formB->setTargetOrder(TARGET_LEADER);
	
	groupA = new Group(formA);
	groupB = new Group(formB);

	setup = new Setup(formA, formB);
	renderer->initSetupRenderer(setup);
	battle = new Battle(seed, groupA, groupB);
}

//Converts the provided point, p, from screen coordinates to OpenGL coordinate system 
void GLscreenToWindowCoordinates(double x, double y, double & rx, double & ry)
{
	//Obtain conversion values
	double model[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	double projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);

	double temp;
	//viewport[3] - p.y because of the flip on the y axis
	gluUnProject(x, viewport[3] - y, 0, model, projection, viewport, 
		&rx, &ry, &temp);
}

void GLrender();
void GLprocessMouseClick(int button, int state, int x, int y);
void GLprocessMouseMove(int x, int y);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("My Application");
	glutDisplayFunc(GLrender);
	glutMouseFunc(GLprocessMouseClick);
	glutPassiveMotionFunc(GLprocessMouseMove);
	gluOrtho2D(VIEWPORT_LEFT, VIEWPORT_RIGHT, VIEWPORT_BOTTOM, VIEWPORT_TOP);

	initialize();
	battle->print();
	
	glutMainLoop();

	return 0;
}

void GLrender()
{
	if (mode == MODE_BATTLE)
		renderer->initBattleRenderer(battle);

	glClear(GL_COLOR_BUFFER_BIT); 

	//This is where we draw
	if (mode == MODE_SETUP)
		renderer->renderSetup();
	else if (mode == MODE_BATTLE)
		renderer->renderBattle();

	glFlush();	
	glutSwapBuffers();
}

void GLprocessMouseClick(int button, int state, int x, int y)
{
	Point2D loc; 
	GLscreenToWindowCoordinates(x, y, loc.x, loc.y);
	if (state == GLUT_DOWN)
	{
		if (mode == MODE_SETUP)
		{
			renderer->processMouseClickSetup(loc);
			glutPostRedisplay();
		}
		else if (mode == MODE_BATTLE)
		{
			battle->cleanupTurn();
			battle->executeTurn();
			glutPostRedisplay();
		}
	}
}

void GLprocessMouseMove(int x, int y)
{
	Point2D loc; 
	GLscreenToWindowCoordinates(x, y, loc.x, loc.y);
	if (mode == MODE_SETUP) {
		renderer->processMouseMoveSetup(loc);
	}
	if (change)
		glutPostRedisplay();
	change = false;
}
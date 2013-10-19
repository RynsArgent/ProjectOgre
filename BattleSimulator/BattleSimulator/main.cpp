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

int seed = 0;
bool change = false;

Formation* formA = new Formation();
Formation* formB = new Formation();

enum Mode { MODE_SETUP, MODE_BATTLE };
Mode mode = MODE_SETUP;

Setup* setup = NULL;

Group* groupA = NULL;
Group* groupB = NULL;

Battle* battle = NULL;

Renderer* renderer = new Renderer(WINDOW_WIDTH, WINDOW_HEIGHT, VIEWPORT_LEFT, VIEWPORT_RIGHT, VIEWPORT_TOP, VIEWPORT_BOTTOM);

void initialize() {
	seed = 0; //time(NULL);
	srand(seed);

	/*
		*********GROUP A*********
		FRONT	[0,0] [1,0] [2,0]
		MIDDLE	[0,1] [1,1] [2,1]
		BACK	[0,2] [1,2] [2,2]
	*/
	formA->setCharacterAt(1, 0, new Character("Fighter1A", JOB_FIGHTER, ELEMENT_PHYSICAL), 0, 0, 0);
	formA->setCharacterAt(1, 1, new Character("Acolyte2A", JOB_ACOLYTE, ELEMENT_PHYSICAL), 0, 0, 0);
	formA->setCharacterAt(0, 2, new Character("Scout1A", JOB_SCOUT, ELEMENT_PHYSICAL), 0, 0, 0);
	formA->setCharacterAt(2, 2, new Character("Scout2A", JOB_SCOUT, ELEMENT_PHYSICAL), 0, 0, 0);
	formA->setCharacterAt(1, 2, new Character("Mage1A", JOB_MAGE, ELEMENT_EARTH), 0, 0, 0);
	formA->setLeaderPosition(1, 2);
	formA->setTargetOrder(TARGET_LEADER);

	/*
		*********GROUP B*********
		BACK	[2,2] [1,2] [0,2]
		MIDDLE	[2,1] [1,1] [0,1]
		FRONT	[2,0] [1,0] [0,0]
	*/
	formB->setCharacterAt(0, 0, new Character("Fighter1B", JOB_FIGHTER, ELEMENT_PHYSICAL), 0, 0, 0);
	formB->setCharacterAt(2, 0, new Character("Fighter2B", JOB_FIGHTER, ELEMENT_PHYSICAL), 0, 0, 0);
	formB->setCharacterAt(0, 1, new Character("Acolyte1B", JOB_ACOLYTE, ELEMENT_PHYSICAL), 0, 1, 0);
	formB->setCharacterAt(2, 1, new Character("Acolyte2B", JOB_ACOLYTE, ELEMENT_PHYSICAL), 0, 0, 0);
	formB->setCharacterAt(1, 2, new Character("Fighter3B",JOB_FIGHTER, ELEMENT_PHYSICAL), 0, 0, 0);
	formB->setLeaderPosition(1, 2);
	formB->setTargetOrder(TARGET_LEADER);
	
	setup = new Setup(formA, formB);
	renderer->initSetupRenderer(setup);
	renderer->load("data.txt");
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
void GLprocessKeyboardPress(unsigned char key, int x, int y);
void GLprocessMouseClick(int button, int state, int x, int y);
void GLprocessMouseMove(int x, int y);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("My Application");
	glutDisplayFunc(GLrender);
	glutKeyboardFunc(GLprocessKeyboardPress);
	glutMouseFunc(GLprocessMouseClick);
	glutPassiveMotionFunc(GLprocessMouseMove);
	gluOrtho2D(VIEWPORT_LEFT, VIEWPORT_RIGHT, VIEWPORT_BOTTOM, VIEWPORT_TOP);

	initialize();
	
	glutMainLoop();

	return 0;
}

void GLrender()
{
	glClear(GL_COLOR_BUFFER_BIT); 

	//This is where we draw
	if (mode == MODE_SETUP)
		renderer->renderSetup();
	else if (mode == MODE_BATTLE)
		renderer->renderBattle();

	glFlush();	
	glutSwapBuffers();
}

void GLprocessKeyboardPress(unsigned char key, int x, int y)
{
	if (key == 27) //esc
	{
		if (mode == MODE_SETUP) {
			exit(0);
		} else {
			delete battle;
			mode = MODE_SETUP;
			renderer->setupInfo.done = false;
			glutPostRedisplay();
		}
	}
}

void GLprocessMouseClick(int button, int state, int x, int y)
{
	Point2D loc; 
	GLscreenToWindowCoordinates(x, y, loc.x, loc.y);
	if (state == GLUT_DOWN)
	{
		if (mode == MODE_SETUP)
		{
			if (button == GLUT_LEFT_BUTTON)
			{
				renderer->processMouseLeftClickSetup(loc);
				if (renderer->setupInfo.done) {
					mode = MODE_BATTLE;
					groupA = new Group(formA);
					groupB = new Group(formB);
					battle = new Battle(seed, groupA, groupB);
					renderer->initBattleRenderer(battle);
					battle->preprint();
					battle->postprint();
				}
				glutPostRedisplay();
			}
			else if (button == GLUT_RIGHT_BUTTON)
			{
				renderer->processMouseRightClickSetup(loc);
			}
		}
		else if (mode == MODE_BATTLE)
		{
			battle->preprint();
			do
			{
				battle->cleanupTurn();
				battle->executeTurn();
			} while (battle->getEventStack().size() <= 0 && !battle->isBattleOver());
			battle->postprint();
			renderer->initBattleRenderer(battle);
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
	} else {
		renderer->processMouseMoveBattle(loc);
	}
	if (change)
		glutPostRedisplay();
	change = false;
}
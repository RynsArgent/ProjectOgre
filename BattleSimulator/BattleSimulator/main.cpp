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
#include "renderer.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const double VIEWPORT_LEFT = 0.0;
const double VIEWPORT_RIGHT = 1.0;
const double VIEWPORT_TOP = 0.0;
const double VIEWPORT_BOTTOM = 1.0;

Formation* formA = new Formation();
Formation* formB = new Formation();

Group* groupA = NULL;
Group* groupB = NULL;
Battle* battle = NULL;

Renderer* renderer = new Renderer(VIEWPORT_LEFT, VIEWPORT_RIGHT, VIEWPORT_TOP, VIEWPORT_BOTTOM);

void initialize() {
	/*
		*********GROUP A*********
		FRONT	[0,0] [1,0] [2,0]
		MIDDLE	[0,1] [1,1] [2,1]
		BACK	[0,2] [1,2] [2,2]
		
		        100Bl       100Bl
		        Shoot Shoot Shoot
					  Scope       
	*/
	formA->setCharacterAt(0, 0, new Fighter("fighter1A"), HUNDRED_BLADES, STRIKE, TAUNT);
	formA->setCharacterAt(2, 0, new Fighter("fighter2A"), HUNDRED_BLADES, STRIKE, TAUNT);
	formA->setCharacterAt(0, 1, new Scout("scout1A"), SHOOT, SHOOT, SCOPE);
	formA->setCharacterAt(1, 1, new Scout("scout2A"), SHOOT, SHOOT, SCOPE);
	formA->setCharacterAt(2, 1, new Scout("scout3A"), SHOOT, SHOOT, SCOPE);
	formA->setCharacterAt(1, 2, new Scout("scout4A"), SHOOT, SHOOT, SCOPE);
	formA->setTargetOrder(TARGET_RANDOM);
	groupA = new Group(formA);

	/*
		*********GROUP B*********
		BACK	[2,2] [1,2] [0,2]
		MIDDLE	[2,1] [1,1] [0,1]
		FRONT	[2,0] [1,0] [0,0]
		
		              BShout
		        Shoot		Shoot
		        TgTrp Block TgTrp
	*/

	formB->setCharacterAt(1, 0, new Fighter("fighter1B"), BLOCK, STRIKE, TAUNT);
	formB->setCharacterAt(1, 2, new Fighter("fighter2B"), BLOCK, STRIKE, BATTLE_SHOUT);
	formB->setCharacterAt(2, 0, new Scout("scout1B"), TANGLE_TRAP, SHOOT, SCOPE);
	formB->setCharacterAt(0, 0, new Scout("scout2B"), TANGLE_TRAP, SHOOT, SCOPE);
	formB->setCharacterAt(2, 1, new Scout("scout3B"), HASTE, SHOOT, SCOPE);
	formB->setCharacterAt(0, 1, new Scout("scout4B"), HASTE, SHOOT, SCOPE);
	formB->setTargetOrder(TARGET_RANDOM);
	groupB = new Group(formB);

	battle = new Battle(groupA, groupB);
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
void GLprocessMouse(int button, int state, int x, int y);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("My Application");
	glutDisplayFunc(GLrender);
	glutMouseFunc(GLprocessMouse);
	gluOrtho2D(VIEWPORT_LEFT, VIEWPORT_RIGHT, VIEWPORT_BOTTOM, VIEWPORT_TOP);

	srand(0);
	initialize();
	battle->print();

	glutMainLoop();

	return 0;
}

void GLrender()
{
	glClear(GL_COLOR_BUFFER_BIT); 

	//This is where we draw
	renderer->render(battle);

	glFlush();	
	glutSwapBuffers();
}


void GLprocessMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		battle->executeTurn();
		glutPostRedisplay();
	}
}

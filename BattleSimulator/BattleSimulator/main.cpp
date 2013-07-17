#include <Windows.h>
#include <gl\glut.h>

#include "battle.h"

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 800;

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
	gluOrtho2D(0.0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0);

	glutMainLoop();

	return 0;
}

void GLrender()
{
	glClear(GL_COLOR_BUFFER_BIT); 

	//This is where we draw

	glFlush();	
	glutSwapBuffers();
}


void GLprocessMouse(int button, int state, int x, int y)
{
}
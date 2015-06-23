#include "ProjectionWindow.h"

void projectionLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//---------------------------
	//	�����ŕ`��

	glutSwapBuffers();
}

void projectionReshapeEvent(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(PROJ_FOVY_X, (double)w / h, 0.1, 100.0);

	gluLookAt(
		0, 0, 0,
		0, 0, 1,
		0, 1, 0);
}

void projectionKeyEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 0x1B:		//	Esc
		exit(0);
		break;
	default:
		break;
	}
}
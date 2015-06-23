#include "ProjectionWindow.h"

int projButton;
int projXBegin = 0, projYBegin = 0;
static GLfloat objRoll = 0, objPitch = 0, objYaw = 0;
static GLfloat objTx = 0 , objTy = 0, objTz = 2.0;

void projectionLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//---------------------------
	//	ここで描画

	glPushMatrix();
	{
		glTranslated(objTx, objTy, objTz);
		glRotated(objPitch, 1.0, 0.0, 0.0);
		glRotated(objYaw, 0.0, 1.0, 0.0);
		glRotated(objRoll, 0.0, 0.0, 1.0);
		drawGlobalXYZ(0.3, 2.0);
		glColor3d(0.6, 0.8, 0.4);
		myBox(0.15, 0.175, 0.105);
	}
	glPopMatrix();

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
	case 'l':		//	Load Model And Texture
		break;
	case 'r':		//	reset
		objRoll = 0; objPitch = 0; objYaw = 0;
		objTx = 0; objTy = 0; objTz = 2.0;
		break;
	case 't':		//	tracking(現在はフィッティング済みの位置姿勢を入力)
		objRoll = 121.75; objPitch = -138.25; objYaw = -5.75;
		objTx = -0.014; objTy = 0.060; objTz = 1.557;
		break;
	default:
		break;
	}
}

void projectionMouseEvent(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		switch (button)
		{
		case GLUT_RIGHT_BUTTON:
		case GLUT_MIDDLE_BUTTON:
		case GLUT_LEFT_BUTTON:
			projButton = button;
			break;
		}
		projXBegin = x;
		projYBegin = y;
	}

}

void projectionMotionEvent(int x, int y)
{
	int xDisp, yDisp;
	xDisp = x - projXBegin;
	yDisp = y - projYBegin;
	switch (projButton)
	{
	case GLUT_LEFT_BUTTON:
		objYaw += (float)xDisp / 4.0;
		objPitch -= (float)yDisp / 4.0;
		break;
	case GLUT_MIDDLE_BUTTON:
		objTx -= (float)xDisp / 1000.0;	//	mm単位
		objTy -= (float)yDisp / 1000.0;
		break;
	case GLUT_RIGHT_BUTTON:
		objRoll -= (float)xDisp / 4.0;
		objTz += yDisp / 1000.0;
		break;
	}
	projXBegin = x;
	projYBegin = y;

}
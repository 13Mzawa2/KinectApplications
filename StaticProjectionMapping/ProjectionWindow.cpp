#include "ProjectionWindow.h"

int projButton;
int projXBegin = 0, projYBegin = 0;
static GLfloat objRoll = 0, objPitch = 0, objYaw = 0;
static GLfloat objTx = 0 , objTy = 0, objTz = 2.0;
static int xyzDrawFlag = 0;
static Quaternion current = Quaternion(1.0, 0.0, 0.0, 0.0);

void projectionLoop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		0, 0, 0,
		0, 0, 1,
		0, 1, 0);

	//---------------------------
	//	ここで描画

	glPushMatrix();
	{
		glTranslated(objTx, objTy, objTz);
		glMultMatrixd(current.rotate());
		drawGlobalXYZ(0.3, 2.0, xyzDrawFlag);
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
		current = Quaternion(1.0, 0.0, 0.0, 0.0);
		objTx = 0; objTy = 0; objTz = 2.0;
		break;
	case 't':		//	tracking(現在はフィッティング済みの位置姿勢を入力)
		current = Quaternion(-0.078, 0.351, -0.865, -0.351);
		objTx = -0.006; objTy = 0.083; objTz = 1.555;
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

	double dx = (double)xDisp / PROJ_WIN_WIDTH, dy = (double)yDisp / PROJ_WIN_HEIGHT;
	double length = sqrt(dx*dx + dy*dy);	//	クォータニオンの長さ
	double rad;
	Quaternion after;
	
	switch (projButton)
	{
	case GLUT_LEFT_BUTTON:
		rad = length * GL_PI;
		after = Quaternion(cos(rad), -sin(rad) * dy / length, sin(rad) * dx / length, 0.0);
		current = after * current;
		break;
	case GLUT_MIDDLE_BUTTON:
		xyzDrawFlag |= ((xDisp != 0)*DRAW_FLAG_X | (yDisp != 0)*DRAW_FLAG_Y);
		objTx -= (float)xDisp / 1000.0;	//	mm単位
		objTy -= (float)yDisp / 1000.0;
		break;
	case GLUT_RIGHT_BUTTON:
		break;
	}
	projXBegin = x;
	projYBegin = y;

	projectionLoop();
	xyzDrawFlag &= DRAW_FLAG_Z;
}

void projectionMouseWheelEvent(int wheelNum, int direction, int x, int y)
{
	xyzDrawFlag |= DRAW_FLAG_Z;
	if (direction == 1)
		objTz += 1.0 / 200.0;
	else
		objTz -= 1.0 / 200.0;
	projectionLoop();
	xyzDrawFlag &= (DRAW_FLAG_X | DRAW_FLAG_Y);
}
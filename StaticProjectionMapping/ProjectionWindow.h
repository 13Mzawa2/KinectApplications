#ifndef PROJECTIONWINDOW_H_
#define PROJECTIONWINDOW_H_

#include <OpenCVAdapter.hpp>
#include <gl/glut.h>
#include "OpenGLDrawing.h"

#define PROJ_WIN_POS_X	1600
#define PROJ_WIN_POS_Y	0
#define PROJ_WIN_WIDTH	1050
#define PROJ_WIN_HEIGHT	1400

//	�R�[���o�b�N�֐�
void projectionLoop();
void projectionKeyEvent(unsigned char key, int x, int y);
void projectionReshapeEvent(int w, int h);

#endif //	PROJECTIONWINDOW_H_

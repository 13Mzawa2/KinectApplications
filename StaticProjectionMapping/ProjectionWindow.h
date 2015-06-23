#ifndef PROJECTIONWINDOW_H_
#define PROJECTIONWINDOW_H_

#include <OpenCVAdapter.hpp>
#include "OpenGLDrawing.h"
#include <gl/freeglut.h>
#//include "CalibrationEngine.h"

#define PROJ_WIN_POS_X	1600
#define PROJ_WIN_POS_Y	0
#define PROJ_WIN_WIDTH	768
#define PROJ_WIN_HEIGHT	1024
#define PROJ_FOVY_X		21.24

//	コールバック関数
void projectionLoop();
void projectionKeyEvent(unsigned char key, int x, int y);
void projectionReshapeEvent(int w, int h);

#endif //	PROJECTIONWINDOW_H_

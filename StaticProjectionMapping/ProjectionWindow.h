//	プロジェクタ側に投影する全画面表示のウィンドウ

#ifndef PROJECTIONWINDOW_H_
#define PROJECTIONWINDOW_H_

#include <OpenCVAdapter.hpp>
#include "OpenGLDrawing.h"
#include <gl/freeglut.h>
#include "GlobalVariables.h"
#//include "CalibrationEngine.h"

#define PROJ_WIN_POS_X	1600
#define PROJ_WIN_POS_Y	0
#define PROJ_WIN_WIDTH	768
#define PROJ_WIN_HEIGHT	1024
#define PROJ_FOVY_X		21.24

//	Projection Windowのためのパラメータ
extern int projButton;
extern int projXBegin;
extern int projYBegin;

//	コールバック関数
void projectionLoop();
void projectionKeyEvent(unsigned char key, int x, int y);
void projectionReshapeEvent(int w, int h);
void projectionMouseEvent(int button, int state, int x, int y);
void projectionMotionEvent(int x, int y);
void projectionMouseWheelEvent(int wheelNum, int dir, int x, int y);

#endif //	PROJECTIONWINDOW_H_

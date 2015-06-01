#ifndef MAIN_H_
#define MAIN_H_

#include "KinectV1Adapter.h"
#include "CalibrationEngine.h"
#include <gl\glut.h>
#include "FullscreenWindow.h"

KinectV1 kSensor;
CalibrationEngine cEngine;
Mat cameraImg;
Mat depthImg;
Mat depthGrayImg;
Mat cloudImg;

//	距離に関するパラメータ　単位：m
const double glZNear = 0.001;		//	カメラからの最小距離
const double glZFar = 10.0;		//	カメラからの最大距離
const int glFovy = 45;			//	カメラの視野角(degree)

//	OpenGL操作のためのパラメータ
int FormWidth = 640;
int FormHeight = 480;
int mButton;
float twist, elevation, azimuth;
float cameraDistance = 0, cameraX = 0, cameraY = 0;
int xBegin, yBegin;

void polarview();
void mainLoop();
void reshape(int w, int h);
void glutKeyEvent(unsigned char key, int x, int y);
void glutMouseEvent(int button, int state, int x, int y);
void glutMotionEvent(int x, int y);
void glutIdleEvent();

#endif //	MAIN_H_
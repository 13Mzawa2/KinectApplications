#ifndef MAIN_H_
#define MAIN_H_

#include "KinectV1Adapter.h"
#include "PCLAdapter.h"
#include "OpenCVAdapter.hpp"
#include "CalibrationEngine.h"
#include "TrackingEngine.h"
#include <gl\glut.h>
#include "FullscreenWindow.h"

KinectV1 kSensor;
CalibrationEngine cEngine;
TrackingEngine tEngine;
cv::Mat cameraImg;
cv::Mat depthImg;
cv::Mat depthGrayImg;
cv::Mat cloudImg;
cv::Mat projectImg;

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

//	フラグ
bool isTracking = false;

void polarview();
void mainLoop();
void reshape(int w, int h);
void glutKeyEvent(unsigned char key, int x, int y);
void glutMouseEvent(int button, int state, int x, int y);
void glutMotionEvent(int x, int y);
void glutIdleEvent();

//	UIのための関数
void setcam();
bool import3DFile(string filename);

#endif //	MAIN_H_
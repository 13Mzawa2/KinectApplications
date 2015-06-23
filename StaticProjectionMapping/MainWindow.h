#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <OpenCVAdapter.hpp>
#include <KinectV1Adapter.h>
#include "OpenGLDrawing.h"
#include <gl/freeglut.h>
#include "CalibrationEngine.h"
#include "HomographicImage.h"
#include "TextureMappingEngine.h"

//	自作処理エンジン
extern KinectV1 kSensor;
extern CalibrationEngine ce;
extern TextureMappingEngine tme;

//	必要な画像群
extern cv::Mat cameraImg;
extern cv::Mat depthImg;
extern cv::Mat depthGrayImg;
extern cv::Mat cloudImg;

//	距離に関するパラメータ　単位：m
extern const double glZNear;		//	カメラからの最小距離
extern const double glZFar;		//	カメラからの最大距離
extern const int glFovy;			//	カメラの視野角(degree)

//	MainWindow操作のためのパラメータ
extern int FormWidth;			//	フォームの幅
extern int FormHeight;			//	フォームの高さ
extern int mButton;					//	ドラッグしているボタンID
extern float twist, elevation, azimuth;						//	roll, pitch, yaw
extern float cameraDistance, cameraX, cameraY;		//	カメラ原点からのZ, X, Y 距離
extern int xBegin, yBegin;				//	ドラッグ開始位置
extern bool cameraLoop;

//	UIのための関数群
void polarview();
void textureMapping();
//	コールバック関数
void mainLoop();
void mainReshapeEvent(int w, int h);
void mainKeyEvent(unsigned char key, int x, int y);
void mainMouseEvent(int button, int state, int x, int y);
void mainMotionEvent(int x, int y);

#endif //	MAINWINDOW_H_
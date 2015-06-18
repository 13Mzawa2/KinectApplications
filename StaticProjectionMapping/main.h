#ifndef MAIN_H_
#define MAIN_H_

#include <OpenCVAdapter.hpp>
#include <KinectV1Adapter.h>
#include <gl/glut.h>

//	自作処理エンジン
KinectV1 kSensor;
//	必要な画像群
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
int FormWidth = 640;			//	フォームの幅
int FormHeight = 480;			//	フォームの高さ
int mButton;					//	ドラッグしているボタンID
float twist, elevation, azimuth;						//	roll, pitch, yaw
float cameraDistance = 0, cameraX = 0, cameraY = 0;		//	カメラ原点からのZ, X, Y 距離
int xBegin, yBegin;				//	ドラッグ開始位置

//	UIのための関数群
void polarview();
//	GLUT用関数
void mainLoop();
void reshapeEvent(int w, int h);
void glutKeyEvent(unsigned char key, int x, int y);
void glutMouseEvent(int button, int state, int x, int y);
void glutMotionEvent(int x, int y);
void glutIdleEvent();


#endif // MAIN_H_
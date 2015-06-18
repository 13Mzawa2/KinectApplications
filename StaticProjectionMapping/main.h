#ifndef MAIN_H_
#define MAIN_H_

#include <OpenCVAdapter.hpp>
#include <KinectV1Adapter.h>
#include <gl/glut.h>

//	���쏈���G���W��
KinectV1 kSensor;
//	�K�v�ȉ摜�Q
cv::Mat cameraImg;
cv::Mat depthImg;
cv::Mat depthGrayImg;
cv::Mat cloudImg;
cv::Mat projectImg;

//	�����Ɋւ���p�����[�^�@�P�ʁFm
const double glZNear = 0.001;		//	�J��������̍ŏ�����
const double glZFar = 10.0;		//	�J��������̍ő勗��
const int glFovy = 45;			//	�J�����̎���p(degree)

//	OpenGL����̂��߂̃p�����[�^
int FormWidth = 640;			//	�t�H�[���̕�
int FormHeight = 480;			//	�t�H�[���̍���
int mButton;
float twist, elevation, azimuth;		//	
float cameraDistance = 0, cameraX = 0, cameraY = 0;
int xBegin, yBegin;

//	UI�̂��߂̊֐��Q
void polarview();
//	GLUT�p�֐�
void mainLoop();
void reshapeEvent(int w, int h);
void glutKeyEvent(unsigned char key, int x, int y);
void glutMouseEvent(int button, int state, int x, int y);
void glutMotionEvent(int x, int y);
void glutIdleEvent();


#endif // MAIN_H_
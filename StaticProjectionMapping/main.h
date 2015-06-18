#ifndef MAIN_H_
#define MAIN_H_

#include <OpenCVAdapter.hpp>
#include <KinectV1Adapter.h>
#include <gl/glut.h>
#include "OpenGLDrawing.h"
#include "CalibrationEngine.h"
#include "HomographicImage.h"
#include "TextureMappingEngine.h"

//	���쏈���G���W��
KinectV1 kSensor;
CalibrationEngine ce;
TextureMappingEngine tme;

//	�K�v�ȉ摜�Q
cv::Mat cameraImg;
cv::Mat depthImg;
cv::Mat depthGrayImg;
cv::Mat cloudImg;
//cv::Mat projectImg = cv::Mat(Size(PROJECTOR_WINDOW_WIDTH, PROJECTOR_WINDOW_HEIGHT), CV_8U);

//	�����Ɋւ���p�����[�^�@�P�ʁFm
const double glZNear = 0.001;		//	�J��������̍ŏ�����
const double glZFar = 10.0;		//	�J��������̍ő勗��
const int glFovy = 45;			//	�J�����̎���p(degree)

//	OpenGL����̂��߂̃p�����[�^
int FormWidth = 640;			//	�t�H�[���̕�
int FormHeight = 480;			//	�t�H�[���̍���
int mButton;					//	�h���b�O���Ă���{�^��ID
float twist, elevation, azimuth;						//	roll, pitch, yaw
float cameraDistance = 0, cameraX = 0, cameraY = 0;		//	�J�������_�����Z, X, Y ����
int xBegin, yBegin;				//	�h���b�O�J�n�ʒu

//	UI�̂��߂̊֐��Q
void polarview();
void textureMapping();
//	GLUT�p�֐�
void mainLoop();
void reshapeEvent(int w, int h);
void glutKeyEvent(unsigned char key, int x, int y);
void glutMouseEvent(int button, int state, int x, int y);
void glutMotionEvent(int x, int y);
void glutIdleEvent();


#endif // MAIN_H_
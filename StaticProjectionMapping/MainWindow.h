#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <OpenCVAdapter.hpp>
#include <KinectV1Adapter.h>
#include "OpenGLDrawing.h"
#include <gl/freeglut.h>
#include "CalibrationEngine.h"
#include "HomographicImage.h"
#include "TextureMappingEngine.h"

//	���쏈���G���W��
extern KinectV1 kSensor;
extern CalibrationEngine ce;
extern TextureMappingEngine tme;

//	�K�v�ȉ摜�Q
extern cv::Mat cameraImg;
extern cv::Mat depthImg;
extern cv::Mat depthGrayImg;
extern cv::Mat cloudImg;

//	�����Ɋւ���p�����[�^�@�P�ʁFm
extern const double glZNear;		//	�J��������̍ŏ�����
extern const double glZFar;		//	�J��������̍ő勗��
extern const int glFovy;			//	�J�����̎���p(degree)

//	MainWindow����̂��߂̃p�����[�^
extern int FormWidth;			//	�t�H�[���̕�
extern int FormHeight;			//	�t�H�[���̍���
extern int mButton;					//	�h���b�O���Ă���{�^��ID
extern float twist, elevation, azimuth;						//	roll, pitch, yaw
extern float cameraDistance, cameraX, cameraY;		//	�J�������_�����Z, X, Y ����
extern int xBegin, yBegin;				//	�h���b�O�J�n�ʒu
extern bool cameraLoop;

//	UI�̂��߂̊֐��Q
void polarview();
void textureMapping();
//	�R�[���o�b�N�֐�
void mainLoop();
void mainReshapeEvent(int w, int h);
void mainKeyEvent(unsigned char key, int x, int y);
void mainMouseEvent(int button, int state, int x, int y);
void mainMotionEvent(int x, int y);

#endif //	MAINWINDOW_H_
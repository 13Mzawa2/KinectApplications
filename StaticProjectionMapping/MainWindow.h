//	PC��ʂ̃��C���E�B���h�E

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <OpenCVAdapter.hpp>
#include <KinectV1Adapter.h>
#include "OpenGLDrawing.h"
#include "CalibrationEngine.h"
#include "HomographicImage.h"
#include "TextureMappingEngine.h"
#include "Quaternion.h"
#include "GlobalVariables.h"

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
extern double glFovy;			//	�J�����̎���p(degree)

//	MainWindow����̂��߂̃p�����[�^
extern int FormWidth;			//	�t�H�[���̕�
extern int FormHeight;			//	�t�H�[���̍���
extern int mButton;					//	�h���b�O���Ă���{�^��ID
extern float twist, elevation, azimuth;						//	roll, pitch, yaw
extern float cameraDistance, cameraX, cameraY;		//	�J�������_�����Z, X, Y ����
extern int xBegin, yBegin;				//	�h���b�O�J�n�ʒu
extern bool cameraLoop;				//	Kinect�ւ̃A�N�Z�X����
extern bool projectorView;			//	�v���W�F�N�^���W�n�ɐ؂�ւ�

//	�����܂Ƃ�
void getFrames();
void showFPS();
//	UI�̂��߂̊֐��Q
void polarview();
void textureMapping();
void changeViewPoint();		//	�J�������_ <--> �v���W�F�N�^���_��؂�ւ���
//	�R�[���o�b�N�֐�
void mainLoop();
void mainReshapeEvent(int w, int h);
void mainKeyEvent(unsigned char key, int x, int y);
void mainMouseEvent(int button, int state, int x, int y);
void mainMotionEvent(int x, int y);

#endif //	MAINWINDOW_H_
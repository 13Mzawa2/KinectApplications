#pragma once

#include "KinectV1Adapter.h"
#include "FullscreenWindow.h"

#define PROJECTOR_WINDOW_WIDTH			1400
#define PROJECTOR_WINDOW_HEIGHT			1050

#define CALIB_CB_CORNER_COLS			10
#define CALIB_CB_CORNER_ROWS			7
#define CALIB_CB_SIZE					26			//	���ۂ̃`�F�X�{�[�h��1�ӂ̃T�C�Y

#define CALIB_DEFAULT_CHESS_ORIGIN_X	100
#define CALIB_DEFAULT_CHESS_ORIGIN_Y	100
#define CALIB_DEFAULT_CHESS_SIZE		100			//	�v���W�F�N�V��������`�F�X�{�[�h��1�ӂ̃T�C�Y

class CalibrationEngine
{
public:
	cv::Size numChessPoint;							//	�J�E���g�͍��ォ�牺->�E�̏�
	cv::Size projectorWindowSize;
	int chessSize;								//	�`�F�X�{�[�h���΂̖ڃT�C�Y
	cv::Point chessRectPoint;						//	�`�F�X�{�[�h�̍�����W
	cv::Mat chessYellow;
	cv::Mat calibrationWindow;
	FullscreenWindow calib_fw;					//	�v���W�F�N�^�[�Ń��j���[�o�[���������邽�߂̏����G���W��
	cv::Mat homographyProCam;						//	�v���W�F�N�^�ɓ��e����摜�i�摜�T�C�Y�̓J�����Ɠ����j�ƁC������B�e�����摜�̍��W���ʂ���v����悤�ɕϊ�����z���O���t�B�s��

	cv::Mat projectionMat;			//	3x4 �������e�s�� C,  p = CP
	cv::Mat cameraInnerMat;			//	3x3 �J���������s�� A = [[fu, fk, u0],[0, fv, v0],[0, 0, 1]]
	cv::Mat rotationMat;			//	3x3 ��]�s��
	cv::Mat translationMat;			//	4x1 ���i�x�N�g��
	cv::Point3d transVector;		//	���i�x�N�g���@OpenGL�p
	cv::Point3d eulerAngles;		//	�I�C���[�p�@OpenGL�p

	CalibrationEngine();
	~CalibrationEngine();
	void setup();
	void calibrateProKinect(KinectV1 kinect);			//	Kinect��p�����J�����p�����[�^����i�����C���{�j
	void calibrateProCam(KinectV1 kinect);			//	���ʂɃ`�F�b�J�[�p�^�[���𓊉e���ăz���O���t�B�s��𓾂�
	void warpCam2Pro(cv::Mat &camImg, cv::Mat &proImg);		//	�J�����摜���v���W�F�N�^���e�p�̉摜�ɕϊ�����
	void createChessPattern(cv::Mat &chess, cv::Scalar color, cv::Scalar backcolor = cv::Scalar(0,0,0));
	void splitChessPattern(cv::Mat &srcImg, cv::Mat &chessPro, cv::Mat &chessCam);
	bool getChessPoints(cv::Mat chessImg, vector<cv::Point2f> &corners);
	double distance(cv::Point2f p1, cv::Point2f p2);
	void destroyAllCalibrationWindows(void);
	double getProCamRatio();
};


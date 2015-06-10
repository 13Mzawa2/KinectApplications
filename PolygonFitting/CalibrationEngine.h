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

	CalibrationEngine();
	~CalibrationEngine();
	void setup();
	void calibrateProCam(KinectV1 kinect);			//	�`�F�b�J�[�p�^�[���𓊉e���ăz���O���t�B�s��𓾂�
	void warpCam2Pro(cv::Mat &camImg, cv::Mat &proImg);		//	�J�����摜���v���W�F�N�^���e�p�̉摜�ɕϊ�����
	void createChessPattern(cv::Mat &chess, cv::Scalar color, cv::Scalar backcolor = cv::Scalar(0,0,0));
	void splitChessPattern(cv::Mat &srcImg, cv::Mat &chessPro, cv::Mat &chessCam);
	bool getChessPoints(cv::Mat chessImg, vector<cv::Point2f> &corners);
	double distance(cv::Point2f p1, cv::Point2f p2);
	void destroyAllCalibrationWindows(void);
	double getProCamRatio();
};


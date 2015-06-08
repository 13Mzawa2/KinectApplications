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
	Size numChessPoint;							//	�J�E���g�͍��ォ�牺->�E�̏�
	Size projectorWindowSize;
	int chessSize;								//	�`�F�X�{�[�h���΂̖ڃT�C�Y
	Point chessRectPoint;						//	�`�F�X�{�[�h�̍�����W
	Mat chessYellow;
	Mat calibrationWindow;
	FullscreenWindow calib_fw;					//	�v���W�F�N�^�[�Ń��j���[�o�[���������邽�߂̏����G���W��
	Mat homographyProCam;						//	�v���W�F�N�^�ɓ��e����摜�i�摜�T�C�Y�̓J�����Ɠ����j�ƁC������B�e�����摜�̍��W���ʂ���v����悤�ɕϊ�����z���O���t�B�s��

	CalibrationEngine();
	~CalibrationEngine();
	void setup();
	void calibrateProCam(KinectV1 kinect);		//	Cyan�F�`�F�b�J�[�̕��ʔł�p�����v���W�F�N�^�L�����u���[�V����(���c��)
	void rescaleChessPattern(KinectV1 kinect);
	void createChessPattern(Mat &chess, Scalar color, Scalar backcolor = Scalar(0,0,0));
	void splitChessPattern(Mat &srcImg, Mat &chessPro, Mat &chessCam);
	bool getChessPoints(Mat chessImg, vector<Point2f> &corners);
	double distance(Point2f p1, Point2f p2);
	void destroyAllCalibrationWindows(void);
	double getProCamRatio();
};


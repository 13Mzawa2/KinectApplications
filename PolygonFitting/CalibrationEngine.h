#pragma once

#include "KinectV1Adapter.h"
#include "FullscreenWindow.h"

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

	CalibrationEngine();
	~CalibrationEngine();
	void calibrateProCam(KinectV1 kinect);		//	Cyan�F�`�F�b�J�[�̕��ʔł�p�����v���W�F�N�^�L�����u���[�V����(���c��)
	void createChessPattern(Mat &chess, Scalar color, Scalar backcolor = Scalar(0,0,0));
	void splitChessPattern(Mat &srcImg, Mat &chessPro, Mat &chessCam);
	bool getChessPoints(Mat chessImg, vector<Point2f> &corners);
	double distance(Point2f p1, Point2f p2);
	void destroyAllCalibrationWindows(void);
	double getProCamRatio();
};


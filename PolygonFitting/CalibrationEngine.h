#pragma once

#include "KinectV1Adapter.h"

class CalibrationEngine
{
public:
	const Point numChessPoint = Point(10, 7);
	Size projectorWindowSize = Size(1024, 768);
	const int chessSize = 50;
	Mat chessYellow;
	Mat calibrationWindow;

	CalibrationEngine();
	~CalibrationEngine();
	void calibrateProCam(KinectV1 kinect);		//	Cyan�F�̕��ʔł�p�����v���W�F�N�^�L�����u���[�V����(���c��)
	void createChessPattern(Mat &chess, Scalar color, Scalar backcolor = Scalar(0,0,0));
	void splitChessPattern(Mat &srcImg, Mat &chessPro, Mat &chessCam);
};


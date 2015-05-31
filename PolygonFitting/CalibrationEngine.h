#pragma once

#include "KinectV1Adapter.h"

class CalibrationEngine
{
public:
	const Size numChessPoint = Size(10, 7);		//	�J�E���g�͍��ォ�牺->�E�̏�
	Size projectorWindowSize = Size(1024, 768);
	int chessSize = 50;							//	�`�F�X�{�[�h���΂̖ڃT�C�Y
	Point chessRectPoint = Point(100, 100);		//	�`�F�X�{�[�h�̍�����W
	Mat chessYellow;
	Mat calibrationWindow;

	CalibrationEngine();
	~CalibrationEngine();
	void calibrateProCam(KinectV1 kinect);		//	Cyan�F�`�F�b�J�[�̕��ʔł�p�����v���W�F�N�^�L�����u���[�V����(���c��)
	void createChessPattern(Mat &chess, Scalar color, Scalar backcolor = Scalar(0,0,0));
	void splitChessPattern(Mat &srcImg, Mat &chessPro, Mat &chessCam);
	void getChessPoints(Mat chessImg, vector<Point2f> corners);
	double distance(Point2f p1, Point2f p2);
};


#pragma once

#include "KinectV1Adapter.h"

class CalibrationEngine
{
public:
	const Size numChessPoint = Size(10, 7);		//	カウントは左上から下->右の順
	Size projectorWindowSize = Size(1024, 768);
	int chessSize = 50;							//	チェスボードの賽の目サイズ
	Point chessRectPoint = Point(100, 100);		//	チェスボードの左上座標
	Mat chessYellow;
	Mat calibrationWindow;

	CalibrationEngine();
	~CalibrationEngine();
	void calibrateProCam(KinectV1 kinect);		//	Cyan色チェッカーの平面版を用いたプロジェクタキャリブレーション(清田ら)
	void createChessPattern(Mat &chess, Scalar color, Scalar backcolor = Scalar(0,0,0));
	void splitChessPattern(Mat &srcImg, Mat &chessPro, Mat &chessCam);
	void getChessPoints(Mat chessImg, vector<Point2f> corners);
	double distance(Point2f p1, Point2f p2);
};


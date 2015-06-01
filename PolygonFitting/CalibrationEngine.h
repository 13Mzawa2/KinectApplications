#pragma once

#include "KinectV1Adapter.h"
#include "FullscreenWindow.h"

class CalibrationEngine
{
public:
	Size numChessPoint;							//	カウントは左上から下->右の順
	Size projectorWindowSize;
	int chessSize;								//	チェスボードの賽の目サイズ
	Point chessRectPoint;						//	チェスボードの左上座標
	Mat chessYellow;
	Mat calibrationWindow;
	FullscreenWindow calib_fw;					//	プロジェクターでメニューバーを消去するための処理エンジン

	CalibrationEngine();
	~CalibrationEngine();
	void calibrateProCam(KinectV1 kinect);		//	Cyan色チェッカーの平面版を用いたプロジェクタキャリブレーション(清田ら)
	void createChessPattern(Mat &chess, Scalar color, Scalar backcolor = Scalar(0,0,0));
	void splitChessPattern(Mat &srcImg, Mat &chessPro, Mat &chessCam);
	bool getChessPoints(Mat chessImg, vector<Point2f> &corners);
	double distance(Point2f p1, Point2f p2);
	void destroyAllCalibrationWindows(void);
	double getProCamRatio();
};


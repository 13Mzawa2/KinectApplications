#pragma once

#include "KinectV1Adapter.h"
#include "FullscreenWindow.h"

#define PROJECTOR_WINDOW_WIDTH			1400
#define PROJECTOR_WINDOW_HEIGHT			1050

#define CALIB_CB_CORNER_COLS			10
#define CALIB_CB_CORNER_ROWS			7
#define CALIB_CB_SIZE					26			//	実際のチェスボードの1辺のサイズ

#define CALIB_DEFAULT_CHESS_ORIGIN_X	100
#define CALIB_DEFAULT_CHESS_ORIGIN_Y	100
#define CALIB_DEFAULT_CHESS_SIZE		100			//	プロジェクションするチェスボードの1辺のサイズ

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
	void setup();
	void calibrateProCam(KinectV1 kinect);		//	Cyan色チェッカーの平面版を用いたプロジェクタキャリブレーション(清田ら)
	void rescaleChessPattern(KinectV1 kinect);
	void createChessPattern(Mat &chess, Scalar color, Scalar backcolor = Scalar(0,0,0));
	void splitChessPattern(Mat &srcImg, Mat &chessPro, Mat &chessCam);
	bool getChessPoints(Mat chessImg, vector<Point2f> &corners);
	double distance(Point2f p1, Point2f p2);
	void destroyAllCalibrationWindows(void);
	double getProCamRatio();
};


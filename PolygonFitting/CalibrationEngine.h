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
	cv::Size numChessPoint;							//	カウントは左上から下->右の順
	cv::Size projectorWindowSize;
	int chessSize;								//	チェスボードの賽の目サイズ
	cv::Point chessRectPoint;						//	チェスボードの左上座標
	cv::Mat chessYellow;
	cv::Mat calibrationWindow;
	FullscreenWindow calib_fw;					//	プロジェクターでメニューバーを消去するための処理エンジン
	cv::Mat homographyProCam;						//	プロジェクタに投影する画像（画像サイズはカメラと同じ）と，それを撮影した画像の座標平面が一致するように変換するホモグラフィ行列

	cv::Mat projectionMat;			//	3x4 透視投影行列 C,  p = CP
	cv::Mat cameraInnerMat;			//	3x3 カメラ内部行列 A = [[fu, fk, u0],[0, fv, v0],[0, 0, 1]]
	cv::Mat rotationMat;			//	3x3 回転行列
	cv::Mat translationMat;			//	4x1 並進ベクトル
	cv::Point3d transVector;		//	並進ベクトル　OpenGL用
	cv::Point3d eulerAngles;		//	オイラー角　OpenGL用(pitch, roll, yaw)
	double rotate[16];

	CalibrationEngine();
	~CalibrationEngine();
	void setup();
	void calibrateProKinect(KinectV1 kinect);			//	Kinectを用いたカメラパラメータ推定（長岡，橋本）
	void calibrateProCam(KinectV1 kinect);			//	平面にチェッカーパターンを投影してホモグラフィ行列を得る
	void warpCam2Pro(cv::Mat &camImg, cv::Mat &proImg);		//	カメラ画像をプロジェクタ投影用の画像に変換する
	void createChessPattern(cv::Mat &chess, cv::Scalar color, cv::Scalar backcolor = cv::Scalar(0,0,0));
	void splitChessPattern(cv::Mat &srcImg, cv::Mat &chessPro, cv::Mat &chessCam);
	bool getChessPoints(cv::Mat chessImg, vector<cv::Point2f> &corners);
	double distance(cv::Point2f p1, cv::Point2f p2);
	void destroyAllCalibrationWindows(void);
	double getProCamRatio();
};


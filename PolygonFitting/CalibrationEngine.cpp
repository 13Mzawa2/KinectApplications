#include "CalibrationEngine.h"


CalibrationEngine::CalibrationEngine()
{
	calibrationWindow = Mat(projectorWindowSize, CV_8UC3, Scalar(0, 0, 0));
}


CalibrationEngine::~CalibrationEngine()
{
}

void CalibrationEngine::calibrateProCam(KinectV1 kinect)
{
	//	1. 投影パターンサイズの自動調節
	cout << "Starting Calibration..." << endl << endl;
	//	初期チェスパターンを作成
	createChessPattern(chessYellow, Scalar(0, 255, 255));
	Rect roi_rect(Point(100, 100), chessYellow.size());
	Mat roiWindow = calibrationWindow(roi_rect);
	chessYellow.copyTo(roiWindow);
	imshow("calibWindow", calibrationWindow);

	//	投影したチェスパターンを撮影しながら位置と大きさを変更
	cout << "投影パターンと平面板の両方が画角に収まっていることを確認してください．" << endl
		<< "なるべくカメラ全体に平面板が写るように近づけてください．" << endl
		<< "確認が終わったらspaceキーを押してください．" << endl;
	Mat colorImg, chessPro, chessCam;
	while (waitKey(10) != ' ')
	{
		kinect.waitFrames();
		kinect.getColorFrame(colorImg);
		imshow("確認用", colorImg);
		kinect.releaseFrames();
	}
	splitChessPattern(colorImg, chessPro, chessCam);
	//	2. 入力画像の撮影
	//	3. 入力画像の分離
	//	4. カメラの内部校正
	//	5. プロジェクタの内部校正
	//	6. カメラプロジェクタ間の外部構成の高精度化
}

void CalibrationEngine::createChessPattern(Mat &chess, Scalar color, Scalar backcolor)
{
	chess = Mat(cv::Size((numChessPoint.x + 1)*chessSize, (numChessPoint.y + 1)*chessSize), CV_8UC3, backcolor);
	for (int i = 0; i < numChessPoint.x + 1; i += 2)
	{
		for (int j = 0; j < numChessPoint.y + 1; j += 2)
		{
			rectangle(
				chess,
				Point(i*chessSize, j*chessSize), Point((i + 1)*chessSize, (j + 1)*chessSize),
				color, -1);
		}
	}
	for (int i = 1; i < numChessPoint.x + 1; i += 2)
	{
		for (int j = 1; j < numChessPoint.y + 1; j += 2)
		{
			rectangle(
				chess,
				Point(i*chessSize, j*chessSize), Point((i + 1)*chessSize, (j + 1)*chessSize),
				color, -1);
		}
	}

}
void CalibrationEngine::splitChessPattern(Mat &src, Mat &chessPro, Mat &chessCam)
{
	vector<Mat> planes;
	split(src, planes);
	adaptiveThreshold(planes[0], chessCam, 128, 255, ADAPTIVE_THRESH_GAUSSIAN_C, src.rows / 12, 30);
	adaptiveThreshold(planes[2], chessPro, 128, 255, ADAPTIVE_THRESH_GAUSSIAN_C, src.rows / 12, 30);
}
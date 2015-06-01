#include "CalibrationEngine.h"


CalibrationEngine::CalibrationEngine()
{
	chessRectPoint = Point(100, 100);
	projectorWindowSize = Size(1400, 1050);
	numChessPoint = Size(10, 7);
	chessSize = 50;

	calibrationWindow = Mat(projectorWindowSize, CV_8UC3, Scalar(255,255,255));
	namedWindow("calibWindow");
	calib_fw.setFullscreen("calibWindow");
}

CalibrationEngine::~CalibrationEngine()
{
}

void CalibrationEngine::calibrateProCam(KinectV1 kinect)
{
	//	1. 投影パターンサイズの自動調節
	cout << "Starting Calibration..." << endl << endl;
	//	初期チェスパターンを作成
	createChessPattern(chessYellow, Scalar(0, 255, 255), Scalar(255,255,255));
	Rect roi_rect(chessRectPoint, chessYellow.size());
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
		flip(colorImg, colorImg, 1);
		imshow("確認用", colorImg);
		kinect.releaseFrames();
	}
	//	チェスパターンの2値化
	splitChessPattern(colorImg, chessPro, chessCam);
	adaptiveThreshold(chessPro, chessPro, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 161, 10);
	adaptiveThreshold(chessCam, chessCam, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 161, 10);
	imshow("Projector", chessPro);
	imshow("Camera", chessCam);
	//	チェスパターンのカメラ座標取得 基本的に平面板がより大きく映っている
	vector<Point2f> chessProCorners, chessCamCorners;
	getChessPoints(chessPro, chessProCorners);
	getChessPoints(chessCam, chessCamCorners);
	if (chessProCorners.empty() || chessCamCorners.empty())
	{
		cout << "チェスパターンが見つかりませんでした．" << endl;
		waitKey(0);
		destroyAllCalibrationWindows();
		return;
	}
	//	投影チェスパターンの位置・大きさを調節
	chessRectPoint = Point(
		(chessCamCorners.at(0).x + chessProCorners.at(0).x - chessRectPoint.x - chessSize / getProCamRatio()) * getProCamRatio(),
		(chessCamCorners.at(0).y + chessProCorners.at(0).y - chessRectPoint.y - chessSize / getProCamRatio()) * getProCamRatio());
	chessSize = (int)(chessSize * getProCamRatio()
		* distance(
		chessCamCorners.at(3 * 10 + 3),
		chessCamCorners.at(3 * 10 + 4))
		/ distance(
		chessProCorners.at(3 * 10 + 3),
		chessProCorners.at(3 * 10 + 4)));
	cout
		<< "初期位置: " << chessRectPoint << endl
		<< "プロジェクタのウインドウサイズ: " << projectorWindowSize << endl
		<< "チェスボードのコーナー数: " << numChessPoint << endl
		<< "チェスボードの初期サイズ: " << chessSize << endl;
	waitKey(0);
	//	新しいチェスパターンを作成して表示
	calibrationWindow = Mat(projectorWindowSize, CV_8UC3, Scalar(255, 255, 255));
	createChessPattern(chessYellow, Scalar(0, 255, 255), Scalar(255,255,255));
	roi_rect = Rect(chessRectPoint, chessYellow.size());
	roiWindow = calibrationWindow(roi_rect);
	chessYellow.copyTo(roiWindow);
	imshow("calibWindow", calibrationWindow);
	//	show current image
	kinect.waitFrames();
	kinect.getColorFrame(colorImg);
	flip(colorImg, colorImg, 1);
	imshow("確認用", colorImg);
	waitKey(0);
	destroyAllCalibrationWindows();

	//	2. 入力画像の撮影
	//	3. 入力画像の分離
	//	4. カメラの内部校正
	//	5. プロジェクタの内部校正
	//	6. カメラプロジェクタ間の外部構成の高精度化
}

void CalibrationEngine::createChessPattern(Mat &chess, Scalar color, Scalar backcolor)
{
	chess = Mat(cv::Size((numChessPoint.width + 1)*chessSize, (numChessPoint.height + 1)*chessSize), CV_8UC3, backcolor);
	for (int i = 0; i < numChessPoint.width + 1; i += 2)
	{
		for (int j = 0; j < numChessPoint.height + 1; j += 2)
		{
			rectangle(
				chess,
				Point(i*chessSize, j*chessSize), Point((i + 1)*chessSize, (j + 1)*chessSize),
				color, -1);
		}
	}
	for (int i = 1; i < numChessPoint.width + 1; i += 2)
	{
		for (int j = 1; j < numChessPoint.height + 1; j += 2)
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
	chessPro = planes[0].clone();		//	投影色は黄色 --> 赤で分離可能
	chessCam = planes[2].clone();		//	撮影色はシアン --> 青で分離可能
}
void CalibrationEngine::getChessPoints(Mat chessImg, vector<Point2f> &corners)
{
	bool found = findChessboardCorners(chessImg, numChessPoint, corners,
		CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);
	if (found)
		cornerSubPix(chessImg, corners, Size(11, 11), Size(-1, -1),
			TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
}

double CalibrationEngine::distance(Point2f p1, Point2f p2)
{
	return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

void CalibrationEngine::destroyAllCalibrationWindows()
{
	destroyWindow("calibWindow");
	destroyWindow("確認用");
	destroyWindow("Projector");
	destroyWindow("Camera");
}

double CalibrationEngine::getProCamRatio()
{
	return (double)projectorWindowSize.width / KINECT_CAM_WIDTH;
}
#include "CalibrationEngine.h"

using namespace cv;

CalibrationEngine::CalibrationEngine()
{
}

CalibrationEngine::~CalibrationEngine()
{
}

void CalibrationEngine::setup()
{
	chessRectPoint = Point(CALIB_DEFAULT_CHESS_ORIGIN_X, CALIB_DEFAULT_CHESS_ORIGIN_Y);
	projectorWindowSize = Size(PROJECTOR_WINDOW_WIDTH, PROJECTOR_WINDOW_HEIGHT);
	numChessPoint = Size(CALIB_CB_CORNER_COLS, CALIB_CB_CORNER_ROWS);
	chessSize = CALIB_DEFAULT_CHESS_SIZE;

	calibrationWindow = Mat(projectorWindowSize, CV_8UC3, Scalar(255, 255, 255));
	//	使用するウィンドウの予約
	namedWindow("calibWindow");
	namedWindow("確認用");
	namedWindow("Projector");
	//namedWindow("Camera");
	namedWindow("ProjectionImg");
	//	タイトルバーの消去
	cout << "キャリブレーションを始めたいプロジェクタにウィンドウを移動してください．" << endl
		<< "移動が終わったら何かキーを押してください．" << endl;
	waitKey(0);
	calib_fw.setFullscreen("calibWindow");

}

void CalibrationEngine::calibrateProCam(KinectV1 kinect)
{
	setup();
	cout << "Starting Calibration..." << endl << endl;
	//	初期チェスパターンを作成
	createChessPattern(chessYellow, Scalar(0, 255, 255), Scalar(255,255,255));
	Rect roi_rect(chessRectPoint, chessYellow.size());
	Mat roiWindow = calibrationWindow(roi_rect);
	chessYellow.copyTo(roiWindow);
	imshow("calibWindow", calibrationWindow);
	//	プロジェクタ用に拡大する前の画像
	Mat imgHomography;
	resize(calibrationWindow, imgHomography, Size(640, 480));
	imshow("ProjectionImg", imgHomography);
	//	1. 入力画像の撮影
	//	投影したチェスパターンを撮影しながら位置と大きさを変更
	cout << "投影パターン全体がカメラの画角に収まっていることを確認してください．" << endl
		<< "確認が終わったらspaceキーを押してください．" << endl;
	Mat colorImg, chessPro, chessCam;
	while (1)
	{
		Mat frame;
		kinect.waitFrames();
		kinect.getColorFrame(frame);
		flip(frame, colorImg, 1);
		imshow("確認用", colorImg);
		kinect.releaseFrames();
		if (waitKey(10) == ' ') break;
	}
	//	2. 入力画像の分離
	//	チェスパターンの2値化
	splitChessPattern(colorImg, chessPro, chessCam);
	adaptiveThreshold(chessPro, chessPro, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 141, 20);
	morphologyEx(chessPro, chessPro, MORPH_OPEN, Mat(3, 3, CV_8U, cv::Scalar::all(255)), Point(-1, -1), 3);	//	opening
	//	チェスパターンのカメラ座標取得 基本的に平面板がより大きく映っている
	vector<Point2f> chessProCorners, chessCamCorners;		//	カメラ座標系から見た投影コーナーと実コーナー
	bool proFound = getChessPoints(chessPro, chessProCorners);
	cout << "チェスパターンを描画します．" << endl;
	cvtColor(chessPro, chessPro, CV_GRAY2BGR);
	drawChessboardCorners(chessPro, numChessPoint, chessProCorners, proFound);
	imshow("Projector", chessPro);
	if (!proFound)
	{
		cout << "チェスパターンが見つかりませんでした．" << endl << endl;
		waitKey(0);
		destroyAllCalibrationWindows();
		return;
	}
	//	歪み補正は中略
	//	3. カメラの内部校正
	//vector<Point3f> objectPoints(CALIB_CB_CORNER_COLS*CALIB_CB_CORNER_ROWS);		//	実空間チェスボードの点群座標
	//for (int i = 0; i < CALIB_CB_CORNER_ROWS; i++)
	//{
	//	for (int j = 0; j < CALIB_CB_CORNER_COLS; j++)
	//	{
	//		objectPoints.at(i * CALIB_CB_CORNER_COLS + j).x = j * CALIB_CB_SIZE;
	//		objectPoints.at(i * CALIB_CB_CORNER_COLS + j).y = i * CALIB_CB_SIZE;
	//		objectPoints.at(i * CALIB_CB_CORNER_COLS + j).z = 0.0;
	//	}
	//}
	//	4. プロジェクタの内部校正


	//	5. カメラプロジェクタ間の外部校正
	//	本当は3,4をやらないと2重に歪みの影響をうけてしまうが，次回までの課題とする
	cout << "カメラ‐プロジェクタ間のホモグラフィ行列を計算します．" << endl;
	vector<Point2f>	chessCorners(CALIB_CB_CORNER_COLS*CALIB_CB_CORNER_ROWS);				//	プロジェクタ画面座標でのコーナー座標
	float scaleProCam = (float)PROJECTOR_WINDOW_WIDTH / colorImg.cols;
	for (int i = 0; i < CALIB_CB_CORNER_ROWS; i++)
	{
		for (int j = 0; j < CALIB_CB_CORNER_COLS; j++)
		{
			chessCorners.at(i * CALIB_CB_CORNER_COLS + j).x = ((j + 1) * chessSize + CALIB_DEFAULT_CHESS_ORIGIN_X);
			chessCorners.at(i * CALIB_CB_CORNER_COLS + j).y = ((i + 1) * chessSize + CALIB_DEFAULT_CHESS_ORIGIN_Y);
		}
	}
	Mat homography;				//	Cam -> Pro のホモグラフィ行列
	vector<Point2f> 
		chess4Corners(4),		//	プロジェクタ画面座標系の歪む前の四隅
		chess4CornersFromCam(4);	//	カメラ画面座標系から見た歪んだ投影パターンの四隅
	chess4Corners.at(0) = chessCorners.at(0);
	chess4CornersFromCam.at(0) = chessProCorners.at(0);
	
	chess4Corners.at(1) = chessCorners.at(CALIB_CB_CORNER_COLS -1);
	chess4CornersFromCam.at(1) = chessProCorners.at(CALIB_CB_CORNER_COLS - 1);
	
	chess4Corners.at(3) = chessCorners.at((CALIB_CB_CORNER_ROWS - 1)*CALIB_CB_CORNER_COLS);
	chess4CornersFromCam.at(3) = chessProCorners.at((CALIB_CB_CORNER_ROWS - 1)*CALIB_CB_CORNER_COLS);
	
	chess4Corners.at(2) = chessCorners.at(CALIB_CB_CORNER_COLS*CALIB_CB_CORNER_ROWS - 1);
	chess4CornersFromCam.at(2) = chessProCorners.at(CALIB_CB_CORNER_COLS*CALIB_CB_CORNER_ROWS - 1);
	
	//homography = getPerspectiveTransform(chess4CornersFromCam, chess4Corners);
	homography = findHomography(chessProCorners, chessCorners, CV_RANSAC);
	homographyProCam = homography.clone();
	if (!homography.empty())
	{
		warpPerspective(imgHomography, calibrationWindow, homography, projectorWindowSize);
		imshow("calibWindow", calibrationWindow);
	}
	//	show current image
	while (1)
	{
		kinect.waitFrames();
		kinect.getColorFrame(colorImg);
		flip(colorImg, colorImg, 1);
		for (int i = 0; i < 4; i++)
		{
			circle(colorImg, chess4Corners.at(i), 3, Scalar(0, 0, 255));
			circle(colorImg, chess4CornersFromCam.at(i), 3, Scalar(255, 0, 0));
		}
		imshow("確認用", colorImg);
		kinect.releaseFrames();
		if (waitKey(10) == ' ') break;
	}
	destroyAllCalibrationWindows();

}

void CalibrationEngine::warpCam2Pro(Mat &camImg, Mat &proImg)
{
	if (!homographyProCam.empty())
	{
		warpPerspective(camImg, proImg, homographyProCam, projectorWindowSize);
	}
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
bool CalibrationEngine::getChessPoints(Mat chessImg, vector<Point2f> &corners)
{
	bool found = findChessboardCorners(chessImg, numChessPoint, corners,
		CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);
	if (found)
	{
		cornerSubPix(chessImg, corners, Size(11, 11), Size(-1, -1),
			TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
	}
	return found;
}

double CalibrationEngine::distance(Point2f p1, Point2f p2)
{
	return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

void CalibrationEngine::destroyAllCalibrationWindows()
{
	//destroyWindow("calibWindow");
	destroyWindow("確認用");
	destroyWindow("Projector");
	destroyWindow("Camera");
	destroyWindow("ProjectionImg");
}

double CalibrationEngine::getProCamRatio()
{
	return (double)projectorWindowSize.width / KINECT_CAM_WIDTH;
}
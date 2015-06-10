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
	//	�g�p����E�B���h�E�̗\��
	namedWindow("calibWindow");
	namedWindow("�m�F�p");
	namedWindow("Projector");
	//namedWindow("Camera");
	namedWindow("ProjectionImg");
	//	�^�C�g���o�[�̏���
	cout << "�L�����u���[�V�������n�߂����v���W�F�N�^�ɃE�B���h�E���ړ����Ă��������D" << endl
		<< "�ړ����I������牽���L�[�������Ă��������D" << endl;
	waitKey(0);
	calib_fw.setFullscreen("calibWindow");

}

void CalibrationEngine::calibrateProCam(KinectV1 kinect)
{
	setup();
	cout << "Starting Calibration..." << endl << endl;
	//	�����`�F�X�p�^�[�����쐬
	createChessPattern(chessYellow, Scalar(0, 255, 255), Scalar(255,255,255));
	Rect roi_rect(chessRectPoint, chessYellow.size());
	Mat roiWindow = calibrationWindow(roi_rect);
	chessYellow.copyTo(roiWindow);
	imshow("calibWindow", calibrationWindow);
	//	�v���W�F�N�^�p�Ɋg�傷��O�̉摜
	Mat imgHomography;
	resize(calibrationWindow, imgHomography, Size(640, 480));
	imshow("ProjectionImg", imgHomography);
	//	1. ���͉摜�̎B�e
	//	���e�����`�F�X�p�^�[�����B�e���Ȃ���ʒu�Ƒ傫����ύX
	cout << "���e�p�^�[���S�̂��J�����̉�p�Ɏ��܂��Ă��邱�Ƃ��m�F���Ă��������D" << endl
		<< "�m�F���I�������space�L�[�������Ă��������D" << endl;
	Mat colorImg, chessPro, chessCam;
	while (1)
	{
		Mat frame;
		kinect.waitFrames();
		kinect.getColorFrame(frame);
		flip(frame, colorImg, 1);
		imshow("�m�F�p", colorImg);
		kinect.releaseFrames();
		if (waitKey(10) == ' ') break;
	}
	//	2. ���͉摜�̕���
	//	�`�F�X�p�^�[����2�l��
	splitChessPattern(colorImg, chessPro, chessCam);
	adaptiveThreshold(chessPro, chessPro, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 141, 20);
	morphologyEx(chessPro, chessPro, MORPH_OPEN, Mat(3, 3, CV_8U, cv::Scalar::all(255)), Point(-1, -1), 3);	//	opening
	//	�`�F�X�p�^�[���̃J�������W�擾 ��{�I�ɕ��ʔ����傫���f���Ă���
	vector<Point2f> chessProCorners, chessCamCorners;		//	�J�������W�n���猩�����e�R�[�i�[�Ǝ��R�[�i�[
	bool proFound = getChessPoints(chessPro, chessProCorners);
	cout << "�`�F�X�p�^�[����`�悵�܂��D" << endl;
	cvtColor(chessPro, chessPro, CV_GRAY2BGR);
	drawChessboardCorners(chessPro, numChessPoint, chessProCorners, proFound);
	imshow("Projector", chessPro);
	if (!proFound)
	{
		cout << "�`�F�X�p�^�[����������܂���ł����D" << endl << endl;
		waitKey(0);
		destroyAllCalibrationWindows();
		return;
	}
	//	�c�ݕ␳�͒���
	//	3. �J�����̓����Z��
	//vector<Point3f> objectPoints(CALIB_CB_CORNER_COLS*CALIB_CB_CORNER_ROWS);		//	����ԃ`�F�X�{�[�h�̓_�Q���W
	//for (int i = 0; i < CALIB_CB_CORNER_ROWS; i++)
	//{
	//	for (int j = 0; j < CALIB_CB_CORNER_COLS; j++)
	//	{
	//		objectPoints.at(i * CALIB_CB_CORNER_COLS + j).x = j * CALIB_CB_SIZE;
	//		objectPoints.at(i * CALIB_CB_CORNER_COLS + j).y = i * CALIB_CB_SIZE;
	//		objectPoints.at(i * CALIB_CB_CORNER_COLS + j).z = 0.0;
	//	}
	//}
	//	4. �v���W�F�N�^�̓����Z��


	//	5. �J�����v���W�F�N�^�Ԃ̊O���Z��
	//	�{����3,4�����Ȃ���2�d�ɘc�݂̉e���������Ă��܂����C����܂ł̉ۑ�Ƃ���
	cout << "�J�����]�v���W�F�N�^�Ԃ̃z���O���t�B�s����v�Z���܂��D" << endl;
	vector<Point2f>	chessCorners(CALIB_CB_CORNER_COLS*CALIB_CB_CORNER_ROWS);				//	�v���W�F�N�^��ʍ��W�ł̃R�[�i�[���W
	float scaleProCam = (float)PROJECTOR_WINDOW_WIDTH / colorImg.cols;
	for (int i = 0; i < CALIB_CB_CORNER_ROWS; i++)
	{
		for (int j = 0; j < CALIB_CB_CORNER_COLS; j++)
		{
			chessCorners.at(i * CALIB_CB_CORNER_COLS + j).x = ((j + 1) * chessSize + CALIB_DEFAULT_CHESS_ORIGIN_X);
			chessCorners.at(i * CALIB_CB_CORNER_COLS + j).y = ((i + 1) * chessSize + CALIB_DEFAULT_CHESS_ORIGIN_Y);
		}
	}
	Mat homography;				//	Cam -> Pro �̃z���O���t�B�s��
	vector<Point2f> 
		chess4Corners(4),		//	�v���W�F�N�^��ʍ��W�n�̘c�ޑO�̎l��
		chess4CornersFromCam(4);	//	�J������ʍ��W�n���猩���c�񂾓��e�p�^�[���̎l��
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
		imshow("�m�F�p", colorImg);
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
	chessPro = planes[0].clone();		//	���e�F�͉��F --> �Ԃŕ����\
	chessCam = planes[2].clone();		//	�B�e�F�̓V�A�� --> �ŕ����\
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
	destroyWindow("calibWindow");
	destroyWindow("�m�F�p");
	destroyWindow("Projector");
	destroyWindow("Camera");
	destroyWindow("ProjectionImg");
}

double CalibrationEngine::getProCamRatio()
{
	return (double)projectorWindowSize.width / KINECT_CAM_WIDTH;
}
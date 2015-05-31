#include "CalibrationEngine.h"


CalibrationEngine::CalibrationEngine()
{
	calibrationWindow = Mat(projectorWindowSize, CV_8UC3, Scalar(128, 128, 128));
}


CalibrationEngine::~CalibrationEngine()
{
}

void CalibrationEngine::calibrateProCam(KinectV1 kinect)
{
	//	1. ���e�p�^�[���T�C�Y�̎�������
	cout << "Starting Calibration..." << endl << endl;
	//	�����`�F�X�p�^�[�����쐬
	createChessPattern(chessYellow, Scalar(0, 255, 255), Scalar(128, 128, 128));
	Rect roi_rect(chessRectPoint, chessYellow.size());
	Mat roiWindow = calibrationWindow(roi_rect);
	chessYellow.copyTo(roiWindow);
	imshow("calibWindow", calibrationWindow);

	//	���e�����`�F�X�p�^�[�����B�e���Ȃ���ʒu�Ƒ傫����ύX
	cout << "���e�p�^�[���ƕ��ʔ̗�������p�Ɏ��܂��Ă��邱�Ƃ��m�F���Ă��������D" << endl
		<< "�Ȃ�ׂ��J�����S�̂ɕ��ʔ��ʂ�悤�ɋ߂Â��Ă��������D" << endl
		<< "�m�F���I�������space�L�[�������Ă��������D" << endl;
	Mat colorImg, chessPro, chessCam;
	while (waitKey(10) != ' ')
	{
		kinect.waitFrames();
		kinect.getColorFrame(colorImg);
		imshow("�m�F�p", colorImg);
		kinect.releaseFrames();
	}
	//	�`�F�X�p�^�[����2�l��
	splitChessPattern(colorImg, chessPro, chessCam);
	imshow("Projector", chessPro);
	imshow("Camera", chessCam);
	//	�`�F�X�p�^�[���̃J�������W�擾 ��{�I�ɕ��ʔ����傫���f���Ă���
	vector<Point2f> chessProCorners, chessCamCorners;
	getChessPoints(chessPro, chessProCorners);
	getChessPoints(chessCam, chessCamCorners);
	if (chessProCorners.empty() || chessCamCorners.empty())
	{
		cout << "�`�F�X�p�^�[����������܂���ł����D" << endl;
		destroyWindow("calibWindow");
		destroyWindow("�m�F�p");
		return;
	}
	//	���e�`�F�X�p�^�[���̈ʒu�E�傫���𒲐�
	chessSize = (int)(0.8 * distance(
		chessCamCorners.at(3 * numChessPoint.width + 3),
		chessCamCorners.at(4 * numChessPoint.width + 3)));
	chessRectPoint = Point(
		chessCamCorners.at(0).x + chessProCorners.at(0).x - 150,
		chessCamCorners.at(0).y + chessProCorners.at(0).y - 150);
	//	�V�����`�F�X�p�^�[�����쐬���ĕ\��
	createChessPattern(chessYellow, Scalar(0, 255, 255), Scalar(128, 128, 128));
	roi_rect = Rect(chessRectPoint, chessYellow.size());
	roiWindow = calibrationWindow(roi_rect);
	chessYellow.copyTo(roiWindow);
	imshow("calibWindow", calibrationWindow);
	waitKey(0);
	destroyWindow("calibWindow");
	destroyWindow("�m�F�p");
	//	2. ���͉摜�̎B�e
	//	3. ���͉摜�̕���
	//	4. �J�����̓����Z��
	//	5. �v���W�F�N�^�̓����Z��
	//	6. �J�����v���W�F�N�^�Ԃ̊O���\���̍����x��
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
void CalibrationEngine::getChessPoints(Mat chessImg, vector<Point2f> corners)
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
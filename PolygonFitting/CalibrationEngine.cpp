#include "CalibrationEngine.h"


CalibrationEngine::CalibrationEngine()
{
	chessRectPoint = Point(100, 100);
	projectorWindowSize = Size(1400, 1050);
	numChessPoint = Size(10, 7);
	chessSize = 100;

	calibrationWindow = Mat(projectorWindowSize, CV_8UC3, Scalar(255,255,255));
	namedWindow("calibWindow");
	calib_fw.setFullscreen("calibWindow");
}

CalibrationEngine::~CalibrationEngine()
{
}

void CalibrationEngine::calibrateProCam(KinectV1 kinect)
{
	//	1. ���e�p�^�[���T�C�Y�̎�������
	cout << "Starting Calibration..." << endl << endl;
	//	�����`�F�X�p�^�[�����쐬
	createChessPattern(chessYellow, Scalar(0, 255, 255), Scalar(255,255,255));
	Rect roi_rect(chessRectPoint, chessYellow.size());
	Mat roiWindow = calibrationWindow(roi_rect);
	chessYellow.copyTo(roiWindow);
	imshow("calibWindow", calibrationWindow);

	//	���e�����`�F�X�p�^�[�����B�e���Ȃ���ʒu�Ƒ傫����ύX
	cout << "���e�p�^�[���ƕ��ʔ̗�������p�Ɏ��܂��Ă��邱�Ƃ��m�F���Ă��������D" << endl
		<< "�Ȃ�ׂ��J�����S�̂ɕ��ʔ��ʂ�悤�ɋ߂Â��Ă��������D" << endl
		<< "�m�F���I�������space�L�[�������Ă��������D" << endl;
	Mat colorImg, chessPro, chessCam;
	while (1)
	{
		kinect.waitFrames();
		kinect.getColorFrame(colorImg);
		flip(colorImg, colorImg, 1);
		imshow("�m�F�p", colorImg);
		kinect.releaseFrames();
		if (waitKey(10) == ' ') break;
	}
	//	�`�F�X�p�^�[����2�l��
	splitChessPattern(colorImg, chessPro, chessCam);
	adaptiveThreshold(chessPro, chessPro, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 181, 0);
	adaptiveThreshold(chessCam, chessCam, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 181, 0);
	//	�`�F�X�p�^�[���̃J�������W�擾 ��{�I�ɕ��ʔ����傫���f���Ă���
	vector<Point2f> chessProCorners, chessCamCorners;
	bool proFound = getChessPoints(chessPro, chessProCorners);
	bool camFound = getChessPoints(chessCam, chessCamCorners);
	cout << "�`�F�X�p�^�[����`�悵�܂��D" << endl;
	cvtColor(chessPro, chessPro, CV_GRAY2BGR);
	cvtColor(chessCam, chessCam, CV_GRAY2BGR);
	drawChessboardCorners(chessPro, numChessPoint, chessProCorners, proFound);
	drawChessboardCorners(chessCam, numChessPoint, chessCamCorners, camFound);
	imshow("Projector", chessPro);
	imshow("Camera", chessCam);
	imshow("�m�F�p", colorImg);
	if (!proFound || !camFound)
	{
		cout << "�`�F�X�p�^�[����������܂���ł����D" << endl << endl;
		waitKey(0);
		destroyAllCalibrationWindows();
		return;
	}
	//	���e�`�F�X�p�^�[���̈ʒu�E�傫���𒲐�
	chessRectPoint = Point(
		(chessCamCorners.at(0).x + chessProCorners.at(0).x - chessRectPoint.x - chessSize / getProCamRatio()) * getProCamRatio(),
		(chessCamCorners.at(0).y + chessProCorners.at(0).y - chessRectPoint.y - chessSize / getProCamRatio()) * getProCamRatio());
	chessSize = (int)(chessSize * getProCamRatio()
		* distance(
		chessCamCorners.at(3 * numChessPoint.width + 3),
		chessCamCorners.at(3 * numChessPoint.width + 4))
		/ distance(
		chessProCorners.at(3 * numChessPoint.width + 3),
		chessProCorners.at(3 * numChessPoint.width + 4)));
	cout
		<< "ROI���_: " << chessRectPoint << endl
		<< "�v���W�F�N�^�̃E�C���h�E�T�C�Y: " << projectorWindowSize << endl
		<< "�`�F�X�{�[�h�̃R�[�i�[��: " << numChessPoint << endl
		<< "�`�F�X�{�[�h�̕ύX��T�C�Y: " << chessSize << endl;
	waitKey(0);
	//	�V�����`�F�X�p�^�[�����쐬���ĕ\��
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
	imshow("�m�F�p", colorImg);
	waitKey(0);
	destroyAllCalibrationWindows();

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
}

double CalibrationEngine::getProCamRatio()
{
	return (double)projectorWindowSize.width / KINECT_CAM_WIDTH;
}
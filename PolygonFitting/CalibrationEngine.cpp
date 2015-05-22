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
	//	1. ���e�p�^�[���T�C�Y�̎�������
	cout << "Starting Calibration..." << endl << endl;
	//	�����`�F�X�p�^�[�����쐬
	createChessPattern(chessYellow, Scalar(0, 255, 255));
	Rect roi_rect(Point(100, 100), chessYellow.size());
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
	splitChessPattern(colorImg, chessPro, chessCam);
	//	2. ���͉摜�̎B�e
	//	3. ���͉摜�̕���
	//	4. �J�����̓����Z��
	//	5. �v���W�F�N�^�̓����Z��
	//	6. �J�����v���W�F�N�^�Ԃ̊O���\���̍����x��
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
#include "KinectV1Adapter.h"

int main(void)
{
	//	Kinect v1�̊ȈՎg�p�N���X
	KinectV1 kSensor(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);

	//auto extractor = cv::DescriptorExtractor::create("ORB");

		//	�������o��I�u�W�F�N�g�̐���
		Ptr<FeatureDetector> detector = cv::FeatureDetector::create("ORB");
		std::vector<cv::KeyPoint> keypoint;			//	�����_�̕ۊǏꏊ
	while (1)
	{
		// �t���[���̍X�V�҂�
		kSensor.waitFrames();

		// Color�J��������t���[�����擾
		cv::Mat colorMat;
		kSensor.getColorFrame(colorMat);
		// Depth�J��������t���[�����擾
		cv::Mat depthMat, playerMask, grayMat, shadeMat;
		kSensor.getDepthFrame(depthMat, playerMask);
		kSensor.cvtDepth2Gray(depthMat, grayMat);
		kSensor.depthShading(depthMat, grayMat, shadeMat);
		//cvtColor(shadeMat, shadeMat, CV_BGR2GRAY);
		// �\��
		cv::imshow("Color", colorMat);
		cv::imshow("Depth", shadeMat);


		////	�����_���o
		//detector->detect(grayMat, keypoint);

		//Mat featureImg = grayMat.clone();
		//cv::drawKeypoints(featureImg, keypoint, featureImg, Scalar(0, 0, 255));

		//cv::imshow("Feature", featureImg);

		// �t���[���̉��
		kSensor.releaseFrames();

		// ���[�v�̏I������(Esc�L�[)
		int key = cv::waitKey(1);
		if (key == VK_ESCAPE)
		{
			break;
		}

	}
	kSensor.shutdown();
	cv::destroyAllWindows();

	return 0;
}
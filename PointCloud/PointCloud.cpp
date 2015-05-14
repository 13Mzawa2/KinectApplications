<<<<<<< HEAD
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
		kSensor.getDepthFrame(depthMat);
		kSensor.cvtDepth2Gray(depthMat, grayMat);
		kSensor.depthShading(depthMat, grayMat, shadeMat);	//	������̖���cvtDepth2Gray()��3ch�摜
		// �\��
		cv::imshow("Color", colorMat);
		cv::imshow("Depth", shadeMat);

		////	�����_���o
		//Mat featureImg = colorMat.clone();
		//cvtColor(featureImg, featureImg, CV_BGR2GRAY);
		//detector->detect(featureImg, keypoint);

		//cv::drawKeypoints(featureImg, keypoint, featureImg, Scalar(0, 0, 255));

		//cv::imshow("Feature", featureImg);

		// �t���[���̉��
		kSensor.releaseFrames();

<<<<<<< ours
		// ���[�v�̏I������(Esc�L�[)
		int key = cv::waitKey(1);
		if (key == VK_ESCAPE)
		{
=======
		if(myWindow.wasStopped())	//	E key
>>>>>>> theirs
			break;
	}

	}
	kSensor.shutdown();
	cv::destroyAllWindows();

	return 0;
=======
#include "KinectV1Adapter.h"

int main(void)
{
	//	Kinect v1�̊ȈՎg�p�N���X
	KinectV1 kSensor(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);

	/// Create a window
	viz::Viz3d myWindow("Coordinate Frame");

	while (1)
	{
		// �t���[���̍X�V�҂�
		kSensor.waitFrames();

		// Color�J��������t���[�����擾
		cv::Mat colorMat;
		kSensor.getColorFrame(colorMat);
		// Depth�J��������t���[�����擾
		cv::Mat depthMat, playerMask, grayMat, shadeMat;
		kSensor.getDepthFrameCoordinated(depthMat);
		kSensor.cvtDepth2Gray(depthMat, grayMat);
		//kSensor.depthShading(depthMat, grayMat, shadeMat);	//	������̖���cvtDepth2Gray()��3ch�摜
		// �\��
		cv::imshow("Color", colorMat);
		cv::imshow("Depth", grayMat);

		Mat cloudMat, colors;
		kSensor.cvtDepth2Cloud(depthMat,cloudMat);
		viz::WCloud cw(cloudMat, colorMat);
		myWindow.showWidget("Kinect", cw, Affine3d().translate(Vec3d(0.0, 0.0, 0.0)));

		myWindow.spinOnce(10, true);

		// �t���[���̉��
		kSensor.releaseFrames();

		if(myWindow.wasStopped())	//	E key
			break;
	}
	//waitKey(1000);
	kSensor.shutdown();
	cv::destroyAllWindows();

	return 0;
>>>>>>> parent of 7a2fade... コメント変更
}
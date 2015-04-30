#include "KinectV1Adapter.h"

int main(void)
{
	//	Kinect v1�̊ȈՎg�p�N���X
	KinectV1 kSensor(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
		NUI_INITIALIZE_FLAG_USES_SKELETON);

	while (1){
		// �t���[���̍X�V�҂�
		kSensor.waitFrames();

		// Color�J��������t���[�����擾
		cv::Mat colorMat;
		kSensor.getColorFrame(colorMat);
		// Depth�J��������t���[�����擾
		cv::Mat depthMat, grayMat, playerMask, shadeMat;
		kSensor.getDepthFrame(depthMat, playerMask);
		kSensor.cvtDepth2Color(depthMat, grayMat);
		kSensor.depthShading(depthMat, grayMat, shadeMat);
		playerMask = playerMask / 2;
		//	���i�f�[�^���擾
		std::vector<cv::Point> joints[KINECT_PLAYER_MAX];
		kSensor.getSkeletonJoints(joints);
		kSensor.drawSkeleton(playerMask, joints);

		// �\��
		cv::imshow("Color", colorMat);
		//cv::imshow("Depth", grayMat);
		cv::imshow("Player", playerMask);
		cv::imshow("Shade", shadeMat);

		// �t���[���̉��
		kSensor.releaseFrames();

		// ���[�v�̏I������(Esc�L�[)
		int key = cv::waitKey(1);
		if (key == VK_ESCAPE){
			break;
		}

	}
	kSensor.shutdown();
	cv::destroyAllWindows();

	return 0;
}
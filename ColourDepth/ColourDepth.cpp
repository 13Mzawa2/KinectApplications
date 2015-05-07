#include "KinectV1Adapter.h"

int main(void)
{
	//	Kinect v1�̊ȈՎg�p�N���X
	KinectV1 kSensor(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH);

	while (1){
		// �t���[���̍X�V�҂�
		kSensor.waitFrames();

		// Color�J��������t���[�����擾
		cv::Mat colorMat;
		kSensor.getColorFrame(colorMat);
		// Depth�J��������t���[�����擾
		cv::Mat depthMat, grayMat;
		kSensor.getDepthFrame(depthMat);
		kSensor.cvtDepth2Color(depthMat, grayMat);
		cv::GaussianBlur(grayMat, grayMat, cv::Size(5, 5), 1.0, 1.0);

		// �\��
		cv::imshow("Color", colorMat);
		cv::imshow("Depth", grayMat);

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
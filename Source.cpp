#include"KinectV1Wrapper.h"

int main(void)
{

	cv::setUseOptimized(true);

	KinectV1 kSensor;
	kSensor.setup();
	kSensor.beginColorStream();
	kSensor.beginDepthStream();
	kSensor.getResolution();
	kSensor.coodinate();

	cv::namedWindow("Color");

	while (1){
		// �t���[���̍X�V�҂�
		kSensor.waitFrames();

		// Color�J��������t���[�����擾
		cv::Mat colorMat;
		kSensor.getColorFrame(colorMat);
		// Depth�J��������t���[�����擾
		cv::Mat depthMat, grayMat, playerMask;
		kSensor.getDepthFrame(depthMat, playerMask);
		kSensor.cvtDepth2Gray(depthMat, grayMat);

		// �\��
		cv::imshow("Color", colorMat);
		cv::imshow("Depth", grayMat);
		cv::imshow("Player", playerMask);

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
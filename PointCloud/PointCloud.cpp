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

		Mat cloudMat;
		kSensor.cvtDepth2Cloud(depthMat,cloudMat);
		viz::WCloud cw(cloudMat, colorMat);
<<<<<<< HEAD
		myWindow.showWidget("Kinect", cw, Affine3d().translate(Vec3d(0.0, 0.0, 0.0)));
=======
		//myWindow.showWidget("Kinect", cw, Affine3d().translate(Vec3d(0.0, 0.0, 0.0)));
>>>>>>> origin/OpenGL_PointCloud_

		myWindow.spinOnce(10, true);

		// �t���[���̉��
		kSensor.releaseFrames();

		if(myWindow.wasStopped())	//	Press E key�@Viz��Q�L�[�������Ē��f���ꂽ��TerminateApp()�𖳎����đ��s����Ƃ悢�@���~���悤�Ƃ���ƃt���[�Y����
			break;
	}
	//waitKey(1000);
	kSensor.shutdown();
	cv::destroyAllWindows();

	return 0;
}
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
		// フレームの更新待ち
		kSensor.waitFrames();

		// Colorカメラからフレームを取得
		cv::Mat colorMat;
		kSensor.getColorFrame(colorMat);
		// Depthカメラからフレームを取得
		cv::Mat depthMat, grayMat, playerMask;
		kSensor.getDepthFrame(depthMat, playerMask);
		kSensor.cvtDepth2Gray(depthMat, grayMat);

		// 表示
		cv::imshow("Color", colorMat);
		cv::imshow("Depth", grayMat);
		cv::imshow("Player", playerMask);

		// フレームの解放
		kSensor.releaseFrames();

		// ループの終了判定(Escキー)
		int key = cv::waitKey(1);
		if (key == VK_ESCAPE){
			break;
		}

	}
	kSensor.shutdown();
	cv::destroyAllWindows();

	return 0;
}
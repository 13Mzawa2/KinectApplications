#include "KinectV1Adapter.h"

int main(void)
{
	//	Kinect v1の簡易使用クラス
	KinectV1 kSensor(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH);

	while (1){
		// フレームの更新待ち
		kSensor.waitFrames();

		// Colorカメラからフレームを取得
		cv::Mat colorMat;
		kSensor.getColorFrame(colorMat);
		// Depthカメラからフレームを取得
		cv::Mat depthMat, grayMat;
		kSensor.getDepthFrame(depthMat);
		kSensor.cvtDepth2Color(depthMat, grayMat);
		cv::GaussianBlur(grayMat, grayMat, cv::Size(5, 5), 1.0, 1.0);

		// 表示
		cv::imshow("Color", colorMat);
		cv::imshow("Depth", grayMat);

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
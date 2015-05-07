#include "KinectV1Adapter.h"

int main(void)
{
	//	Kinect v1の簡易使用クラス
	KinectV1 kSensor(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
		NUI_INITIALIZE_FLAG_USES_SKELETON);

	while (1){
		// フレームの更新待ち
		kSensor.waitFrames();

		// Colorカメラからフレームを取得
		cv::Mat colorMat;
		kSensor.getColorFrame(colorMat);
		// Depthカメラからフレームを取得
		cv::Mat depthMat, grayMat,colorDepth, playerMask, shadeMat;
		kSensor.getDepthFrame(depthMat, playerMask);
		kSensor.cvtDepth2Gray(depthMat, grayMat);				//	実装上の問題でGRAY画像でも3チャンネルで出力される
		kSensor.cvtDepth2Color(depthMat, colorDepth);
		kSensor.depthShading(depthMat, colorDepth, shadeMat);
		kSensor.depthShading(depthMat, grayMat, shadeMat);		//	ここに1チャンネルのgrayMatを入れてはならない
		playerMask = playerMask / 2;
		//	骨格データを取得
		std::vector<cv::Point> joints[KINECT_PLAYER_MAX];
		kSensor.getSkeletonJoints(joints);
		kSensor.drawSkeleton(playerMask, joints);

		// 表示
		cv::imshow("Color", colorMat);
		cv::imshow("DepthGray", grayMat);
		cv::imshow("DepthColor", colorDepth);

		cv::imshow("Player", playerMask);
		cv::imshow("Shade", shadeMat);

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
#include "KinectV1Adapter.h"

int main(void)
{
	//	Kinect v1の簡易使用クラス
	KinectV1 kSensor(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);

	//auto extractor = cv::DescriptorExtractor::create("ORB");

		//	特徴抽出器オブジェクトの生成
		Ptr<FeatureDetector> detector = cv::FeatureDetector::create("ORB");
		std::vector<cv::KeyPoint> keypoint;			//	特徴点の保管場所
	while (1)
	{
		// フレームの更新待ち
		kSensor.waitFrames();

		// Colorカメラからフレームを取得
		cv::Mat colorMat;
		kSensor.getColorFrame(colorMat);
		// Depthカメラからフレームを取得
		cv::Mat depthMat, playerMask, grayMat, shadeMat;
		kSensor.getDepthFrame(depthMat);
		kSensor.cvtDepth2Gray(depthMat, grayMat);
		kSensor.depthShading(depthMat, grayMat, shadeMat);	//	実装上の問題でcvtDepth2Gray()は3ch画像
		// 表示
		cv::imshow("Color", colorMat);
		cv::imshow("Depth", shadeMat);

		////	特徴点抽出
		//Mat featureImg = colorMat.clone();
		//cvtColor(featureImg, featureImg, CV_BGR2GRAY);
		//detector->detect(featureImg, keypoint);

		//cv::drawKeypoints(featureImg, keypoint, featureImg, Scalar(0, 0, 255));

		//cv::imshow("Feature", featureImg);

		// フレームの解放
		kSensor.releaseFrames();

<<<<<<< ours
		// ループの終了判定(Escキー)
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
}
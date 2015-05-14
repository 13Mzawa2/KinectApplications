#include "KinectV1Adapter.h"

int main(void)
{
	//	Kinect v1の簡易使用クラス
	KinectV1 kSensor(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);

<<<<<<< HEAD
	/// Create a window
	viz::Viz3d myWindow("Coordinate Frame");

=======
	//auto extractor = cv::DescriptorExtractor::create("ORB");

		//	特徴抽出器オブジェクトの生成
		Ptr<FeatureDetector> detector = cv::FeatureDetector::create("ORB");
		std::vector<cv::KeyPoint> keypoint;			//	特徴点の保管場所
>>>>>>> origin/modifingKVA
	while (1)
	{
		// フレームの更新待ち
		kSensor.waitFrames();

		// Colorカメラからフレームを取得
		cv::Mat colorMat;
		kSensor.getColorFrame(colorMat);
		// Depthカメラからフレームを取得
		cv::Mat depthMat, playerMask, grayMat, shadeMat;
<<<<<<< HEAD
		kSensor.getDepthFrameCoordinated(depthMat);
		kSensor.cvtDepth2Gray(depthMat, grayMat);
		//kSensor.depthShading(depthMat, grayMat, shadeMat);	//	実装上の問題でcvtDepth2Gray()は3ch画像
		// 表示
		cv::imshow("Color", colorMat);
		cv::imshow("Depth", grayMat);

		Mat cloudMat, colors;
		kSensor.cvtDepth2Cloud(depthMat,cloudMat);
		viz::WCloud cw(cloudMat, colorMat);
		myWindow.showWidget("Kinect", cw, Affine3d().translate(Vec3d(0.0, 0.0, 0.0)));

		myWindow.spinOnce(10, true);
=======
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
>>>>>>> origin/modifingKVA

		// フレームの解放
		kSensor.releaseFrames();

<<<<<<< HEAD
		if(myWindow.wasStopped())	//	Press E key　VizでQキーを押して中断されたらTerminateApp()を無視して続行するとよい　中止しようとするとフリーズする
			break;
	}
	//waitKey(1000);
=======
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
>>>>>>> origin/modifingKVA
	kSensor.shutdown();
	cv::destroyAllWindows();

	return 0;
}
#include "KinectV1Adapter.h"

int main(void)
{
	//	Kinect v1の簡易使用クラス
	KinectV1 kSensor(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);

	/// Create a window
	viz::Viz3d myWindow("Coordinate Frame");

	while (1)
	{
		// フレームの更新待ち
		kSensor.waitFrames();

		// Colorカメラからフレームを取得
		cv::Mat colorMat;
		kSensor.getColorFrame(colorMat);
		// Depthカメラからフレームを取得
		cv::Mat depthMat, playerMask, grayMat, shadeMat;
		kSensor.getDepthFrameCoordinated(depthMat);
		kSensor.cvtDepth2Gray(depthMat, grayMat);
		//kSensor.depthShading(depthMat, grayMat, shadeMat);	//	実装上の問題でcvtDepth2Gray()は3ch画像
		// 表示
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

		// フレームの解放
		kSensor.releaseFrames();

		if(myWindow.wasStopped())	//	Press E key　VizでQキーを押して中断されたらTerminateApp()を無視して続行するとよい　中止しようとするとフリーズする
			break;
	}
	//waitKey(1000);
	kSensor.shutdown();
	cv::destroyAllWindows();

	return 0;
}
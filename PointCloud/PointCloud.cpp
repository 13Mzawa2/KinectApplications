#include "KinectV1Adapter.h"

static
void cvtDepth2Cloud(const Mat& depth, Mat& cloud, const Mat& cameraMatrix)
{
	const float inv_fx = 1.f / cameraMatrix.at<float>(0, 0);
	const float inv_fy = 1.f / cameraMatrix.at<float>(1, 1);
	const float ox = cameraMatrix.at<float>(0, 2);
	const float oy = cameraMatrix.at<float>(1, 2);
	cloud.create(depth.size(), CV_32FC3);
	for (int y = 0; y < cloud.rows; y++)
	{
		Point3f* cloud_ptr = (Point3f*)cloud.ptr(y);
		const float* depth_prt = (const float*)depth.ptr(y);
		for (int x = 0; x < cloud.cols; x++)
		{
			float z = depth_prt[x];
			cloud_ptr[x].x = (x - ox) * z * inv_fx;
			cloud_ptr[x].y = (y - oy) * z * inv_fy;
			cloud_ptr[x].z = z;
		}
	}
}

int main(void)
{
	//	Kinect v1の簡易使用クラス
	KinectV1 kSensor(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);


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
		//kSensor.depthShading(depthMat, grayMat, shadeMat);	//	実装上の問題でcvtDepth2Gray()は3ch画像
		// 表示
		cv::imshow("Color", colorMat);
		cv::imshow("Depth", grayMat);



		// フレームの解放
		kSensor.releaseFrames();

		// ループの終了判定(Escキー)
		int key = cv::waitKey(1);
		if (key == VK_ESCAPE)
		{
			break;
		}

	}
	kSensor.shutdown();
	cv::destroyAllWindows();

	return 0;
}
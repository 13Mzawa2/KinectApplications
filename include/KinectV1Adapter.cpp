#include "KinectV1Adapter.h"

using namespace cv;

KinectV1::KinectV1()
{
	setup(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
		NUI_INITIALIZE_FLAG_USES_SKELETON);
}
KinectV1::KinectV1(DWORD nuiUseFlags)
{
	setup(nuiUseFlags);
}
KinectV1::~KinectV1()
{
}
int KinectV1::setup(DWORD nuiUseFlags)
{
	color[0] = cv::Vec3b(0, 0, 0);
	color[1] = cv::Vec3b(255, 0, 0);
	color[2] = cv::Vec3b(0, 255, 0);
	color[3] = cv::Vec3b(0, 0, 255);
	color[4] = cv::Vec3b(255, 255, 0);
	color[5] = cv::Vec3b(255, 0, 255);
	color[6] = cv::Vec3b(0, 255, 255);

	// Kinectのインスタンス生成、初期化
	hResult = NuiCreateSensorByIndex(0, &pSensor);
	if (FAILED(hResult)){
		std::cerr << "Error : NuiCreateSensorByIndex" << std::endl;
		return -1;
	}
	hResult = pSensor->NuiInitialize(nuiUseFlags);
	if (FAILED(hResult)){
		std::cerr << "Error : NuiInitialize" << std::endl;
		return -1;
	}
	//	どのフレームを使うか判定
	useColorFrame = (nuiUseFlags & NUI_INITIALIZE_FLAG_USES_COLOR) && NUI_INITIALIZE_FLAG_USES_COLOR;
	usePlayerFrame = ((nuiUseFlags & NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX) && NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);
	useDepthFrame = usePlayerFrame
		|| ((nuiUseFlags & NUI_INITIALIZE_FLAG_USES_DEPTH) && NUI_INITIALIZE_FLAG_USES_DEPTH);
	useSkeletonFrame = (nuiUseFlags & NUI_INITIALIZE_FLAG_USES_SKELETON) && NUI_INITIALIZE_FLAG_USES_SKELETON;

	beginColorStream();
	beginDepthStream();
	beginSkeletonStream();
	getCameraSettingInterface();

	coordinate();

	return 0;
}
int KinectV1::beginColorStream()
{
	if (!useColorFrame) return -1;
	// Colorストリームの開始
	hColorEvent = CreateEvent(nullptr, true, false, nullptr);
	hResult = pSensor->NuiImageStreamOpen(
		NUI_IMAGE_TYPE_COLOR,
		imageResolution,
		0,
		2,
		hColorEvent,
		&hColorHandle
		);
	if (FAILED(hResult)){
		std::cerr << "Error : NuiImageStreamOpen( COLOR )" << std::endl;
		return -1;
	}
	getResolution(imageResolution, imageWidth, imageHeight);
	hEvents.push_back(hColorEvent);

	return 0;
}
int KinectV1::beginDepthStream()
{
	if (!useDepthFrame) return -1;
	// Depthストリームの開始
	hDepthEvent = CreateEvent(nullptr, true, false, nullptr);
	hResult = pSensor->NuiImageStreamOpen(
		(usePlayerFrame) ? NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX : NUI_IMAGE_TYPE_DEPTH,
		depthResolution,
		0,
		2,
		hDepthEvent,
		&hDepthHandle);
	if (FAILED(hResult)){
		std::cerr << "Error : NuiImageStreamOpen( DEPTH )" << std::endl;
		return -1;
	}
	getResolution(depthResolution, depthWidth, depthHeight);
	hEvents.push_back(hDepthEvent);

	return 0;
}
int KinectV1::beginSkeletonStream()
{
	if (!useSkeletonFrame) return -1;
	//	Skeletonストリームの開始
	hSkeletonEvent = CreateEvent(nullptr, true, false, nullptr);
	hResult = pSensor->NuiSkeletonTrackingEnable(
		hSkeletonEvent, 0
		/*NUI_SKELETON_TRACKING_FLAG_SUPPRESS_NO_FRAME_DATA |
		NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT |
		NUI_SKELETON_TRACKING_FLAG_ENABLE_IN_NEAR_RANGE*/);
	if (FAILED(hResult)){
		std::cerr << "Error : NuiSkeletonTrackingEnable" << std::endl;
		return -1;
	}
	hEvents.push_back(hSkeletonEvent);
}
// 解像度の取得
void KinectV1::getResolution(NUI_IMAGE_RESOLUTION resolution, int &width, int &height)
{
	unsigned long refWidth = 0;
	unsigned long refHeight = 0;
	NuiImageResolutionToSize(resolution, refWidth, refHeight);
	width = static_cast<int>(refWidth);
	height = static_cast<int>(refHeight);
}
//	デプスマップとRGBカメラとの対応付けの為の準備
int KinectV1::coordinate()
{
	hResult = pSensor->NuiGetCoordinateMapper(&pCoordinateMapper);
	if (FAILED(hResult)){
		std::cerr << "Error : NuiGetCoordinateMapper" << std::endl;
		return -1;
	}
}

//	RGBカメラ設定ストリーム取得
void KinectV1::getCameraSettingInterface()
{
	pSensor->NuiGetColorCameraSettings(&pCameraSettings);
}
//	自動ホワイトバランスの設定・取得
void KinectV1::setAutoWhiteBalance(BOOL isAutoWhiteBalance)
{
	pCameraSettings->SetAutoWhiteBalance(isAutoWhiteBalance);
}
BOOL KinectV1::getAutoWhiteBalance()
{
	BOOL isAutoWhiteBalance = FALSE;
	pCameraSettings->GetAutoWhiteBalance(&isAutoWhiteBalance);
	return isAutoWhiteBalance;
}
//	自動露出の設定・取得
void KinectV1::setAutoExposure(BOOL isAutoExposure)
{
	pCameraSettings->SetAutoExposure(isAutoExposure);
}
BOOL KinectV1::getAutoExposure()
{
	BOOL isAutoExposure = FALSE;
	pCameraSettings->GetAutoExposure(&isAutoExposure);
	return isAutoExposure;
}

void KinectV1::waitFrames()
{
	// フレームの更新待ち
	if (useColorFrame)	ResetEvent(hColorEvent);
	if (useDepthFrame)	ResetEvent(hDepthEvent);
	if (useSkeletonFrame)	ResetEvent(hSkeletonEvent);
	WaitForMultipleObjects(hEvents.size(), hEvents.data(), true, INFINITE);
}
int KinectV1::getColorFrame(cv::Mat& colorMat)
{
	// Colorカメラからフレームを取得
	hResult = pSensor->NuiImageStreamGetNextFrame(hColorHandle, 0, &pColorFrame);
	if (FAILED(hResult)){
		std::cerr << "Error : NuiImageStreamGetNextFrame( COLOR )" << std::endl;
		return -1;
	}

	// Color画像データのロックと取得
	pColorFrameTexture = pColorFrame.pFrameTexture;
	pColorFrameTexture->LockRect(0, &colorLockedRect, nullptr, 0);

	colorMat = cv::Mat(
		imageHeight, imageWidth,
		CV_8UC4,
		reinterpret_cast<unsigned char*>(colorLockedRect.pBits));	//	チャンネル数はb,g,r,予約の4個

	return 0;
}
int KinectV1::getDepthFrame(cv::Mat& depthMat, cv::Mat& playerMask)
{
	// Depthカメラからフレームを取得
	hResult = pSensor->NuiImageStreamGetNextFrame(hDepthHandle, 0, &pDepthFrame);
	if (FAILED(hResult)){
		std::cerr << "Error : NuiImageStreamGetNextFrame( DEPTH )" << std::endl;
		return -1;
	}
	// Depth画像データのロックと取得
	pDepthFrameTexture = nullptr;
	pSensor->NuiImageFrameGetDepthImagePixelFrameTexture(
		hDepthHandle,
		&pDepthFrame,
		&nearMode,
		&pDepthFrameTexture);
	pDepthFrameTexture->LockRect(0, &depthLockedRect, nullptr, 0);

	//	DepthセンサーのrawデータからデプスマップとプレイヤーIDを切り分ける
	depthMat = cv::Mat::zeros(depthHeight, depthWidth, CV_16UC1);
	playerMask = cv::Mat::zeros(depthHeight, depthWidth, CV_8UC3);
	pDepthPixel = reinterpret_cast<NUI_DEPTH_IMAGE_PIXEL*>(depthLockedRect.pBits);

	for (int y = 0; y < depthHeight; y++)
	{
		for (int x = 0; x < depthWidth; x++)
		{
			unsigned int index = y * depthWidth + x;
			depthMat.at<unsigned short>(y, x) = pDepthPixel[index].depth;
			playerMask.at<cv::Vec3b>(y, x) = color[pDepthPixel[index].playerIndex];
		}
	}

	//cv::medianBlur(depthMat, depthMat, 3);
	return 0;
}
int KinectV1::getDepthFrameCoordinated(cv::Mat &depthMat, cv::Mat &playerMask)
{
	// Depthカメラからフレームを取得
	hResult = pSensor->NuiImageStreamGetNextFrame(hDepthHandle, 0, &pDepthFrame);
	if (FAILED(hResult)){
		std::cerr << "Error : NuiImageStreamGetNextFrame( DEPTH )" << std::endl;
		return -1;
	}
	// Depth画像データのロックと取得
	pDepthFrameTexture = nullptr;
	pSensor->NuiImageFrameGetDepthImagePixelFrameTexture(
		hDepthHandle,
		&pDepthFrame,
		&nearMode,
		&pDepthFrameTexture);
	pDepthFrameTexture->LockRect(0, &depthLockedRect, nullptr, 0);

	//	Depthマップをカラー画像にマッピング
	depthMat = cv::Mat::zeros(depthHeight, depthWidth, CV_16UC1);
	playerMask = cv::Mat::zeros(depthHeight, depthWidth, CV_8UC3);
	pDepthPixel = reinterpret_cast<NUI_DEPTH_IMAGE_PIXEL*>(depthLockedRect.pBits);
	std::vector<NUI_COLOR_IMAGE_POINT> pColorPoint = std::vector<NUI_COLOR_IMAGE_POINT>(imageWidth * imageHeight);
	pCoordinateMapper->MapDepthFrameToColorFrame(
		depthResolution, depthWidth * depthHeight, pDepthPixel,
		NUI_IMAGE_TYPE_COLOR, imageResolution, imageWidth * imageHeight, &pColorPoint[0]);

	//	DepthセンサーのrawデータからデプスマップとプレイヤーIDを切り分ける
	for (int y = 0; y < depthHeight; y++)
	{
		for (int x = 0; x < depthWidth; x++)
		{
			unsigned int index = y * depthWidth + x;
			depthMat.at<unsigned short>(pColorPoint[index].y, pColorPoint[index].x) = pDepthPixel[index].depth;
			playerMask.at<cv::Vec3b>(pColorPoint[index].y, pColorPoint[index].x) = color[pDepthPixel[index].playerIndex];
		}
	}
}
int KinectV1::getSkeletonJoints(std::vector<cv::Point> joints[NUI_SKELETON_COUNT])
{
	// Skeletonフレームを取得
	hResult = pSensor->NuiSkeletonGetNextFrame(0, &pSkeletonFrame);
	if (FAILED(hResult)){
		std::cout << "Error : NuiSkeletonGetNextFrame" << std::endl;
		return -1;
	}
	//	Skeletonの各座標を抽出して描画
	NUI_COLOR_IMAGE_POINT colorPoint;			//	スケルトンを構成する座標
	for (int count = 0; count < NUI_SKELETON_COUNT; count++)
	{
		NUI_SKELETON_DATA skeletonData = pSkeletonFrame.SkeletonData[count];
		//	全ての関節が追跡できた場合
		if (skeletonData.eTrackingState == NUI_SKELETON_TRACKED)
		{
			for (int position = 0; position < NUI_SKELETON_POSITION_COUNT; position++)
			{
				//	座標抽出
				pCoordinateMapper->MapSkeletonPointToColorPoint(
					&skeletonData.SkeletonPositions[position],
					NUI_IMAGE_TYPE_COLOR,
					imageResolution,
					&colorPoint);
				joints[count].push_back(Point(colorPoint.x, colorPoint.y));
			}
		}
		//	全体の追跡のみできている場合
		else if (skeletonData.eTrackingState == NUI_SKELETON_POSITION_ONLY)
		{
			pCoordinateMapper->MapSkeletonPointToColorPoint(
				&skeletonData.SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER],
				NUI_IMAGE_TYPE_COLOR,
				imageResolution,
				&colorPoint);
			if ((colorPoint.x >= 0)
				&& (colorPoint.x < imageWidth)
				&& (colorPoint.y >= 0)
				&& (colorPoint.y < imageHeight))
			{
				joints[count].push_back(Point(colorPoint.x, colorPoint.y));
			}
		}
	}
}
int KinectV1::drawSkeleton(cv::Mat& dstMat, std::vector<cv::Point> joints[NUI_SKELETON_COUNT])
{
	if (dstMat.rows == 0 && dstMat.cols == 0) dstMat = cv::Mat::zeros(imageHeight, imageWidth, CV_8UC3);
	//	Skeletonの各座標を描画
	NUI_COLOR_IMAGE_POINT colorPoint;			//	スケルトンを構成する座標
	for (int count = 0; count < NUI_SKELETON_COUNT; count++)
	{
		//	全ての関節が追跡できている場合
		if (joints[count].size() > 1)
		{
			//	関節の描画
			for (int position = 0; position < NUI_SKELETON_POSITION_COUNT; position++)
			{
				cv::circle(dstMat,
					joints[count].at(position), 10,
					static_cast<cv::Scalar>(color[count + 1]), -1, CV_AA);
			}
			//	骨格の描画
			//	脊柱
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_HIP_CENTER),
				joints[count].at(NUI_SKELETON_POSITION_SPINE),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_SPINE),
				joints[count].at(NUI_SKELETON_POSITION_SHOULDER_CENTER),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_SHOULDER_CENTER),
				joints[count].at(NUI_SKELETON_POSITION_HEAD),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			//	左腕
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_SHOULDER_CENTER),
				joints[count].at(NUI_SKELETON_POSITION_SHOULDER_LEFT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_SHOULDER_LEFT),
				joints[count].at(NUI_SKELETON_POSITION_ELBOW_LEFT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_ELBOW_LEFT),
				joints[count].at(NUI_SKELETON_POSITION_WRIST_LEFT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_WRIST_LEFT),
				joints[count].at(NUI_SKELETON_POSITION_HAND_LEFT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			//	右腕
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_SHOULDER_CENTER),
				joints[count].at(NUI_SKELETON_POSITION_SHOULDER_RIGHT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_SHOULDER_RIGHT),
				joints[count].at(NUI_SKELETON_POSITION_ELBOW_RIGHT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_ELBOW_RIGHT),
				joints[count].at(NUI_SKELETON_POSITION_WRIST_RIGHT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_WRIST_RIGHT),
				joints[count].at(NUI_SKELETON_POSITION_HAND_RIGHT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			//	左足
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_HIP_CENTER),
				joints[count].at(NUI_SKELETON_POSITION_HIP_LEFT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_HIP_LEFT),
				joints[count].at(NUI_SKELETON_POSITION_KNEE_LEFT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_KNEE_LEFT),
				joints[count].at(NUI_SKELETON_POSITION_ANKLE_LEFT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_ANKLE_LEFT),
				joints[count].at(NUI_SKELETON_POSITION_FOOT_LEFT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			//	右足
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_HIP_CENTER),
				joints[count].at(NUI_SKELETON_POSITION_HIP_RIGHT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_HIP_RIGHT),
				joints[count].at(NUI_SKELETON_POSITION_KNEE_RIGHT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_KNEE_RIGHT),
				joints[count].at(NUI_SKELETON_POSITION_ANKLE_RIGHT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
			cv::line(dstMat,
				joints[count].at(NUI_SKELETON_POSITION_ANKLE_RIGHT),
				joints[count].at(NUI_SKELETON_POSITION_FOOT_RIGHT),
				static_cast<cv::Scalar>(color[count + 1]), 4);
		}
		//	全体の追跡のみできている場合
		else if (joints[count].size() == 1)
		{
			cv::circle(dstMat,
				joints[count].at(0), 10,
				static_cast<cv::Scalar>(color[count + 1]), -1, CV_AA);
		}
	}
	return 0;
}
void KinectV1::drawSkeleton(cv::Mat& dstMat)
{
	std::vector<Point> joints[6];
	getSkeletonJoints(joints);
	drawSkeleton(dstMat, joints);
}
void KinectV1::cvtDepth2Gray(cv::Mat& depthMat, cv::Mat& grayMat)
{
	Mat tempMat = cv::Mat::zeros(depthMat.size(), CV_8UC3);
	for (int y = 0; y < depthMat.rows; y++){
		for (int x = 0; x < depthMat.cols; x++){
			int iv;
			if ((depthMat.at<unsigned short>(y, x) <= 0) || (depthMat.at<unsigned short>(y, x) > maxDepthRange))
			{
				iv = 255;
			}
			else
			{
				iv = 256 * (depthMat.at<unsigned short>(y, x) - minDepthRange) / maxDepthRange;
				if (iv > 255) iv = 255;
				else if (iv < 0) iv = 0;
			}
			matB(tempMat, x, y) = 255 - iv;
			matG(tempMat, x, y) = 255 - iv;
			matR(tempMat, x, y) = 255 - iv;
		}
	}
	grayMat = tempMat.clone();
}
void KinectV1::cvtDepth2Color(cv::Mat& depthMat, cv::Mat& colorMat)
{
	Mat tempMat = cv::Mat::zeros(depthMat.size(), CV_8UC3);
	cv::cvtColor(tempMat, tempMat, CV_BGR2HSV);
	for (int y = 0; y < tempMat.rows; y++){
		for (int x = 0; x < tempMat.cols; x++){
			int iv;
			if ((depthMat.at<unsigned short>(y, x) <= 0) || (depthMat.at<unsigned short>(y, x) > maxDepthRange))
			{
				iv = 0;
				matG(tempMat, x, y) = 0;
				matR(tempMat, x, y) = 0;
			}
			else
			{
				iv = 180 * (depthMat.at<unsigned short>(y, x) - minDepthRange) / maxDepthRange;
				if (iv > 180) iv = 180;
				else if (iv < 0) iv = 0;
				matG(tempMat, x, y) = 128;
				matR(tempMat, x, y) = 255;
			}
			matB(tempMat, x, y) = iv;
		}
	}
	cv::cvtColor(tempMat, colorMat, CV_HSV2BGR);
}
void KinectV1::depthShading(cv::Mat& depthMat, cv::Mat& srcMat, cv::Mat& dstMat)
{
	Mat tempMat;
	srcMat.convertTo(tempMat, CV_8UC3);
	for (int y = 0; y < depthMat.rows; y++){
		for (int x = 0; x < depthMat.cols; x++){
			//	隣接3pixが範囲外の場合は真っ暗にする
			unsigned short z = depthMat.at<unsigned short>(y, x);
			if ((y >= depthMat.rows) || (x >= depthMat.cols) ||
				(z < 400) || (z > 8000))
			{
				matB(tempMat, x, y) = 0;
				matG(tempMat, x, y) = 0;
				matR(tempMat, x, y) = 0;
				continue;
			}
			//	隣接3pixのデータから現在のピクセルを原点とするベクトルを入手
			cv::Vec3d p[3] =
			{
				cv::Vec3d(0.0, 0.0, (double)depthMat.at<unsigned short>(y, x) * 0.5),
				cv::Vec3d(1.0, 0.0, (double)depthMat.at<unsigned short>(y, x + 1) * 0.5),
				cv::Vec3d(0.0, 1.0, (double)depthMat.at<unsigned short>(y + 1, x) * 0.5)
			};
			cv::Vec3d v[2] = { p[1] - p[0], p[2] - p[0] };
			//	反射光の強さを計算
			cv::Vec3d n = v[0].cross(v[1]);			//	外積(その面の法線ベクトル)
			cv::Vec3d light = cv::Vec3d(0, 0, 1);	//	光源 = 視点
			double diffuse = n.dot(light) / cv::norm(n) / cv::norm(light);	//	反射光の強さ
			//	反射率だけ光を落す
			matB(tempMat, x, y) = (uchar)((double)matB(tempMat, x, y) * diffuse);
			matG(tempMat, x, y) = (uchar)((double)matG(tempMat, x, y) * diffuse);
			matR(tempMat, x, y) = (uchar)((double)matR(tempMat, x, y) * diffuse);
		}
	}
	cv::medianBlur(tempMat, tempMat, 3);
	dstMat = tempMat.clone();
}
void KinectV1::cvtDepth2Cloud(Mat &depthMat, Mat &cloudMat)
{
	//cloudMat = Mat(1, depthMat.cols * depthMat.rows, CV_32FC3);
	cloudMat = Mat(depthMat.rows, depthMat.cols, CV_32FC3);
	Point3f *point = cloudMat.ptr<cv::Point3f>();					//	cloudMatのイテレータ
	for (int y = 0; y < depthMat.rows; y++)
	{
		for (int x = 0; x < depthMat.cols; x++)
		{
			NUI_DEPTH_IMAGE_POINT depthPoints;
			depthPoints.x = x;
			depthPoints.y = y;
			depthPoints.depth = depthMat.at<unsigned short>(y, x);
			Vector4 RealPoints;
			pCoordinateMapper->MapDepthPointToSkeletonPoint(depthResolution, &depthPoints, &RealPoints);
			point[y * depthMat.cols + x].x = RealPoints.x;
			point[y * depthMat.cols + x].y = RealPoints.y;
			point[y * depthMat.cols + x].z = RealPoints.z;
		}
	}
}
void KinectV1::releaseFrames()
{
	if (useColorFrame)
	{
		pColorFrameTexture->UnlockRect(0);
		pSensor->NuiImageStreamReleaseFrame(hColorHandle, &pColorFrame);
	}
	if (useDepthFrame)
	{
		pDepthFrameTexture->UnlockRect(0);
		pSensor->NuiImageStreamReleaseFrame(hDepthHandle, &pDepthFrame);
	}
}
void KinectV1::shutdown()
{
	// Kinectの終了処理
	pSensor->NuiShutdown();
	if (useSkeletonFrame)
	{
		pSensor->NuiSkeletonTrackingDisable();
		CloseHandle(hSkeletonEvent);
	}
	//pCameraSettings->Release();
	if (useColorFrame)	CloseHandle(hColorEvent);
	if (useDepthFrame)	CloseHandle(hDepthEvent);
}
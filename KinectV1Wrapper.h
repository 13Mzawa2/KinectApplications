#ifndef KINECTV1WRAPPER_H_
#define KINECTV1WRAPPER_H_

#include <Windows.h>
#include <NuiApi.h>
#include <NuiSensor.h>
#include "OpenCVAdapter.h"

#pragma comment(lib, "Kinect10.lib")

#define KINECT_CAM_WIDTH		640
#define KINECT_CAM_HEIGHT		480
#define KINECT_DEPTH_WIDTH		640
#define KINECT_DEPTH_HEIGHT		480
#define KINECT_EVENTS			2					//	RGB, Depth, など

#define REAL_DEPTH(pix)			(pix & 0xfff8) >> 3
#define PLAYER_ID(pix)			(pix & 0x0007)

class KinectV1
{
public:
	INuiSensor* pSensor;
	HRESULT hResult = S_OK;							//	ハンドラの実行結果確認用
	HANDLE hColorEvent;								//	RGBカメライベント
	HANDLE hColorHandle;							//	RGBカメラハンドル
	HANDLE hDepthEvent;								//	Dカメライベント
	HANDLE hDepthHandle;							//	Dカメラハンドル
	HANDLE hEvents[KINECT_EVENTS];					//	イベントオブジェクト

	NUI_IMAGE_FRAME pColorFrame;				//	カラー画像データ
	INuiFrameTexture* pColorFrameTexture;
	NUI_LOCKED_RECT colorLockedRect;
	INuiColorCameraSettings* pCameraSettings;		//	RGBカメラ設定
	NUI_IMAGE_FRAME pDepthFrame;				//	デプス画像データ
	INuiFrameTexture* pDepthFrameTexture;
	NUI_LOCKED_RECT depthLockedRect;
	NUI_DEPTH_IMAGE_PIXEL* pDepthPixel;

	INuiCoordinateMapper* pCoordinateMapper;
	std::vector<NUI_COLOR_IMAGE_POINT> pColorPoint;
	BOOL nearMode;

	int minDepthRange;
	int maxDepthRange = 8000.0;
	int width = 0;
	int height = 0;

	// カラーテーブル
	cv::Vec3b color[7];

	KinectV1()
	{
		hColorEvent = INVALID_HANDLE_VALUE;			//	RGBカメライベント
		hColorHandle = INVALID_HANDLE_VALUE;		//	RGBカメラハンドル
		hDepthEvent = INVALID_HANDLE_VALUE;			//	Dカメライベント
		hDepthHandle = INVALID_HANDLE_VALUE;		//	Dカメラハンドル

		color[0] = cv::Vec3b(0, 0, 0);
		color[1] = cv::Vec3b(255, 0, 0);
		color[2] = cv::Vec3b(0, 255, 0);
		color[3] = cv::Vec3b(0, 0, 255);
		color[4] = cv::Vec3b(255, 255, 0);
		color[5] = cv::Vec3b(255, 0, 255);
		color[6] = cv::Vec3b(0, 255, 255);

		nearMode = true;
		if (nearMode) minDepthRange = 400.0;
		else minDepthRange = 800.0;
	}
	~KinectV1()
	{
	}
	int setup()
	{
		// Kinectのインスタンス生成、初期化
		hResult = NuiCreateSensorByIndex(0, &pSensor);
		if (FAILED(hResult)){
			std::cerr << "Error : NuiCreateSensorByIndex" << std::endl;
			return -1;
		}
		hResult = pSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);
		if (FAILED(hResult)){
			std::cerr << "Error : NuiInitialize" << std::endl;
			return -1;
		}
		return 0;
	}
	int beginColorStream()
	{
		// Colorストリームの開始
		hColorEvent = CreateEvent(nullptr, true, false, nullptr);
		hResult = pSensor->NuiImageStreamOpen(
			NUI_IMAGE_TYPE_COLOR,
			NUI_IMAGE_RESOLUTION_640x480,
			0,
			2,
			hColorEvent,
			&hColorHandle
			);
		if (FAILED(hResult)){
			std::cerr << "Error : NuiImageStreamOpen( COLOR )" << std::endl;
			return -1;
		}

		hEvents[0] = hColorEvent;

		return 0;
	}
	int beginDepthStream()
	{
		// Depthストリームの開始
		hDepthEvent = CreateEvent(nullptr, true, false, nullptr);
		hResult = pSensor->NuiImageStreamOpen(
			NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
			NUI_IMAGE_RESOLUTION_640x480,
			0,
			2,
			hDepthEvent,
			&hDepthHandle);
		if (FAILED(hResult)){
			std::cerr << "Error : NuiImageStreamOpen( DEPTH )" << std::endl;
			return -1;
		}
		
		hEvents[1] = hDepthEvent;

		return 0;
	}
	void getResolution()
	{
		// 解像度の取得
		unsigned long refWidth = 0;
		unsigned long refHeight = 0;
		NuiImageResolutionToSize(NUI_IMAGE_RESOLUTION_640x480, refWidth, refHeight);
		width = static_cast<int>(refWidth);
		height = static_cast<int>(refHeight);
	}
	int coodinate()
	{
		hResult = pSensor->NuiGetCoordinateMapper(&pCoordinateMapper);
		if (FAILED(hResult)){
			std::cerr << "Error : NuiGetCoordinateMapper" << std::endl;
			return -1;
		}
		pColorPoint = std::vector<NUI_COLOR_IMAGE_POINT>(width * height);
	}
	void waitFrames()
	{
		// フレームの更新待ち
		ResetEvent(hColorEvent);
		ResetEvent(hDepthEvent);
		WaitForMultipleObjects(KINECT_EVENTS, hEvents, true, INFINITE);
	}
	int getColorFrame(cv::Mat& colorMat)
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
			height, width,
			CV_8UC4,
			reinterpret_cast<unsigned char*>(colorLockedRect.pBits));	//	チャンネル数はb,g,r,予約の4個

		return 0;
	}
	int getDepthFrame(cv::Mat& depthMat, cv::Mat& playerMask)
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

		//	Depth画像をカラー画像に位置合わせする
		depthMat = cv::Mat::zeros(height, width, CV_16UC1);
		playerMask = cv::Mat::zeros(height, width, CV_8UC3);
		pDepthPixel = reinterpret_cast<NUI_DEPTH_IMAGE_PIXEL*>(depthLockedRect.pBits);
		pCoordinateMapper->MapDepthFrameToColorFrame(
			NUI_IMAGE_RESOLUTION_640x480,
			width * height,
			pDepthPixel, 
			NUI_IMAGE_TYPE_COLOR, 
			NUI_IMAGE_RESOLUTION_640x480,
			width * height, 
			&pColorPoint[0]);

		//	DepthセンサーのrawデータからデプスマップとプレイヤーIDを切り分ける
		for (int y = 0; y < height; y++){
			for (int x = 0; x < width; x++){
				unsigned int index = y * width + x;
				depthMat.at<unsigned short>(pColorPoint[index].y, pColorPoint[index].x) = pDepthPixel[index].depth;
				playerMask.at<cv::Vec3b>(pColorPoint[index].y, pColorPoint[index].x) = color[pDepthPixel[index].playerIndex];
			}
		}

		return 0;
	}
	void cvtDepth2Gray(cv::Mat& depthMat, cv::Mat& grayMat)
	{
		grayMat = cv::Mat::zeros(height, width, CV_8UC1);
		for (int y = 0; y < height; y++){
			for (int x = 0; x < width; x++){
				int i;
				if ((depthMat.at<unsigned short>(y, x) <= 0) || (depthMat.at<unsigned short>(y, x) > maxDepthRange))
				{
					i = 255;
				}
				else
				{
					i = 256 * (depthMat.at<unsigned short>(y, x) - minDepthRange) / maxDepthRange;
					if (i > 255) i = 255;
					else if (i < 0) i = 0;
				}
				matGRAY(grayMat, x, y) = i;
			}
		}
		//depthMat.convertTo(grayMat, CV_8U, -255.0f / 10000.0f, 255.0f);
		grayMat = ~grayMat;
	}
	void releaseFrames()
	{
		pColorFrameTexture->UnlockRect(0);
		pDepthFrameTexture->UnlockRect(0);
		pSensor->NuiImageStreamReleaseFrame(hColorHandle, &pColorFrame);
		pSensor->NuiImageStreamReleaseFrame(hDepthHandle, &pDepthFrame);
	}
	void shutdown()
	{
		// Kinectの終了処理
		pSensor->NuiShutdown();
		//pCameraSettings->Release();
		CloseHandle(hColorEvent);
		CloseHandle(hDepthEvent);
	}
};


#endif // KINECTV1WRAPPER_H_
#ifndef KINECTV1ADAPTER_H_
#define KINECTV1ADAPTER_H_

//-------------------------------------------------
//			Includes
//-------------------------------------------------
#include <Windows.h>
#include <NuiApi.h>
#include <NuiSensor.h>
#include "OpenCVAdapter.hpp"

//-------------------------------------------------
//			Libraries
//-------------------------------------------------
#pragma comment(lib, "Kinect10.lib")
//#pragma comment(lib, "FaceTrackLib.lib")
//#pragma comment(lib, "KinectBackgroundRemoval180_32.lib")
//#pragma comment(lib, "KinectFusion180.lib")
//#pragma comment(lib, "KinectIntaraction180_32.lib")


//-------------------------------------------------
//			Define & Mecro
//-------------------------------------------------
#define KINECT_CAM_WIDTH		640
#define KINECT_CAM_HEIGHT		480
#define KINECT_DEPTH_WIDTH		320
#define KINECT_DEPTH_HEIGHT		240
#define KINECT_PLAYER_MAX		NUI_SKELETON_COUNT

#define REAL_DEPTH(pix)			(pix & 0xfff8) >> 3
#define PLAYER_ID(pix)			(pix & 0x0007)

//-------------------------------------------------
//			Class
//-------------------------------------------------
class KinectV1
{
public:
	INuiSensor* pSensor;

	HRESULT hResult = S_OK;							//	ハンドラの実行結果確認用

	std::vector<HANDLE> hEvents;					//	イベントオブジェクト
	HANDLE hColorEvent = INVALID_HANDLE_VALUE;		//	RGBカメライベント
	HANDLE hDepthEvent = INVALID_HANDLE_VALUE;		//	Dカメライベント
	HANDLE hSkeletonEvent = INVALID_HANDLE_VALUE;	//	スケルトンイベント

	HANDLE hColorHandle = INVALID_HANDLE_VALUE;		//	RGBカメラハンドル
	HANDLE hDepthHandle = INVALID_HANDLE_VALUE;		//	Dカメラハンドル

	NUI_IMAGE_FRAME pColorFrame;					//	カラー画像データ
	INuiFrameTexture* pColorFrameTexture;
	NUI_LOCKED_RECT colorLockedRect;
	INuiColorCameraSettings* pCameraSettings;		//	RGBカメラ設定
	static const NUI_IMAGE_RESOLUTION imageResolution = NUI_IMAGE_RESOLUTION_640x480;

	NUI_IMAGE_FRAME pDepthFrame;					//	デプス画像データ
	INuiFrameTexture* pDepthFrameTexture;
	NUI_LOCKED_RECT depthLockedRect;
	NUI_DEPTH_IMAGE_PIXEL* pDepthPixel;
	static const NUI_IMAGE_RESOLUTION depthResolution = NUI_IMAGE_RESOLUTION_640x480;

	NUI_SKELETON_FRAME pSkeletonFrame;				//	スケルトンフレーム

	INuiCoordinateMapper* pCoordinateMapper;
	BOOL nearMode = true;

	//	描画用数値データ
	volatile int minDepthRange = 400;			//	near mode が有効の場合
	volatile int maxDepthRange = 8000;			//	4000[mm]以降は精度が粗くなる
	int imageWidth = 0;
	int imageHeight = 0;
	int depthWidth = 0;
	int depthHeight = 0;

	//	有効化フラグ
	volatile bool useColorFrame = false;
	volatile bool useDepthFrame = false;
	volatile bool usePlayerFrame = false;
	volatile bool useSkeletonFrame = false;

	// カラーテーブル
	cv::Vec3b color[7];

	//	初期化処理
	KinectV1();
	KinectV1(DWORD nuiUseFlags);
	~KinectV1();
	int setup(DWORD nuiUseFlags);
	int beginColorStream();
	int beginDepthStream();
	int beginSkeletonStream();
	void getResolution(NUI_IMAGE_RESOLUTION resolution, int &width, int &height);
	int coordinate();
	//	カメラ設定
	void getCameraSettingInterface();
	void setAutoWhiteBalance(BOOL isAutoWhiteBalance);
	BOOL getAutoWhiteBalance();
	void setAutoExposure(BOOL isAutoExposure);
	BOOL getAutoExposure();
	//	データ取得・表示
	void waitFrames();
	int getColorFrame(cv::Mat& colorMat);
	int getDepthFrame(cv::Mat& depthMat, cv::Mat& playerMask = cv::Mat());
	int getDepthFrameCoordinated(cv::Mat &depthMat, cv::Mat &playerMask = cv::Mat());
	int getSkeletonJoints(std::vector<cv::Point> joints[NUI_SKELETON_COUNT]);
	//	データ加工
	int drawSkeleton(cv::Mat& dstMat, std::vector<cv::Point> joints[NUI_SKELETON_COUNT]);
	void drawSkeleton(cv::Mat& dstMat);
	void cvtDepth2Gray(cv::Mat& depthMat, cv::Mat& grayMat);
	void cvtDepth2Color(cv::Mat& depthMat, cv::Mat& colorMat);
	void depthShading(cv::Mat& depthMat, cv::Mat& srcMat, cv::Mat& dstMat);
	void cvtDepth2Cloud(cv::Mat &depthMat, cv::Mat &cloudMat);
	//	終了処理
	void releaseFrames();
	void shutdown();


};


#endif // KINECTV1ADAPTER_H_
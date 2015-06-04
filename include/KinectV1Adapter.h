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

	HRESULT hResult = S_OK;							//	�n���h���̎��s���ʊm�F�p

	std::vector<HANDLE> hEvents;					//	�C�x���g�I�u�W�F�N�g
	HANDLE hColorEvent = INVALID_HANDLE_VALUE;		//	RGB�J�����C�x���g
	HANDLE hDepthEvent = INVALID_HANDLE_VALUE;		//	D�J�����C�x���g
	HANDLE hSkeletonEvent = INVALID_HANDLE_VALUE;	//	�X�P���g���C�x���g

	HANDLE hColorHandle = INVALID_HANDLE_VALUE;		//	RGB�J�����n���h��
	HANDLE hDepthHandle = INVALID_HANDLE_VALUE;		//	D�J�����n���h��

	NUI_IMAGE_FRAME pColorFrame;					//	�J���[�摜�f�[�^
	INuiFrameTexture* pColorFrameTexture;
	NUI_LOCKED_RECT colorLockedRect;
	INuiColorCameraSettings* pCameraSettings;		//	RGB�J�����ݒ�
	static const NUI_IMAGE_RESOLUTION imageResolution = NUI_IMAGE_RESOLUTION_640x480;

	NUI_IMAGE_FRAME pDepthFrame;					//	�f�v�X�摜�f�[�^
	INuiFrameTexture* pDepthFrameTexture;
	NUI_LOCKED_RECT depthLockedRect;
	NUI_DEPTH_IMAGE_PIXEL* pDepthPixel;
	static const NUI_IMAGE_RESOLUTION depthResolution = NUI_IMAGE_RESOLUTION_640x480;

	NUI_SKELETON_FRAME pSkeletonFrame;				//	�X�P���g���t���[��

	INuiCoordinateMapper* pCoordinateMapper;
	BOOL nearMode = true;

	//	�`��p���l�f�[�^
	volatile int minDepthRange = 400;			//	near mode ���L���̏ꍇ
	volatile int maxDepthRange = 8000;			//	4000[mm]�ȍ~�͐��x���e���Ȃ�
	int imageWidth = 0;
	int imageHeight = 0;
	int depthWidth = 0;
	int depthHeight = 0;

	//	�L�����t���O
	volatile bool useColorFrame = false;
	volatile bool useDepthFrame = false;
	volatile bool usePlayerFrame = false;
	volatile bool useSkeletonFrame = false;

	// �J���[�e�[�u��
	cv::Vec3b color[7];

	//	����������
	KinectV1();
	KinectV1(DWORD nuiUseFlags);
	~KinectV1();
	int setup(DWORD nuiUseFlags);
	int beginColorStream();
	int beginDepthStream();
	int beginSkeletonStream();
	void getResolution(NUI_IMAGE_RESOLUTION resolution, int &width, int &height);
	int coordinate();
	//	�J�����ݒ�
	void getCameraSettingInterface();
	void setAutoWhiteBalance(BOOL isAutoWhiteBalance);
	BOOL getAutoWhiteBalance();
	void setAutoExposure(BOOL isAutoExposure);
	BOOL getAutoExposure();
	//	�f�[�^�擾�E�\��
	void waitFrames();
	int getColorFrame(cv::Mat& colorMat);
	int getDepthFrame(cv::Mat& depthMat, cv::Mat& playerMask = cv::Mat());
	int getDepthFrameCoordinated(cv::Mat &depthMat, cv::Mat &playerMask = cv::Mat());
	int getSkeletonJoints(std::vector<cv::Point> joints[NUI_SKELETON_COUNT]);
	//	�f�[�^���H
	int drawSkeleton(cv::Mat& dstMat, std::vector<cv::Point> joints[NUI_SKELETON_COUNT]);
	void drawSkeleton(cv::Mat& dstMat);
	void cvtDepth2Gray(cv::Mat& depthMat, cv::Mat& grayMat);
	void cvtDepth2Color(cv::Mat& depthMat, cv::Mat& colorMat);
	void depthShading(cv::Mat& depthMat, cv::Mat& srcMat, cv::Mat& dstMat);
	void cvtDepth2Cloud(Mat &depthMat, Mat &cloudMat);
	//	�I������
	void releaseFrames();
	void shutdown();


};


#endif // KINECTV1ADAPTER_H_
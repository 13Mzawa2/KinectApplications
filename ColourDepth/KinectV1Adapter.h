#ifndef KINECTV1ADAPTER_H_
#define KINECTV1ADAPTER_H_

//-------------------------------------------------
//			Includes
//-------------------------------------------------
#include <Windows.h>
#include <NuiApi.h>
#include <NuiSensor.h>
#include "OpenCVAdapter.h"

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

	NUI_IMAGE_FRAME pDepthFrame;					//	�f�v�X�摜�f�[�^
	INuiFrameTexture* pDepthFrameTexture;
	NUI_LOCKED_RECT depthLockedRect;
	NUI_DEPTH_IMAGE_PIXEL* pDepthPixel;

	NUI_SKELETON_FRAME pSkeletonFrame;				//	�X�P���g���t���[��

	INuiCoordinateMapper* pCoordinateMapper;
	std::vector<NUI_COLOR_IMAGE_POINT> pColorPoint;
	BOOL nearMode = true;

	//	�`��p���l�f�[�^
	volatile int minDepthRange = 400;			//	near mode ���L���̏ꍇ
	volatile int maxDepthRange = 8000;			//	4000[mm]�ȍ~�͐��x���e���Ȃ�
	volatile int width = 0;
	volatile int height = 0;

	//	�L�����t���O
	volatile bool useColorFrame = false;
	volatile bool useDepthFrame = false;
	volatile bool usePlayerFrame = false;
	volatile bool useSkeletonFrame = false;


	// �J���[�e�[�u��
	cv::Vec3b color[7];

	KinectV1()
	{
		setup(
			NUI_INITIALIZE_FLAG_USES_COLOR |
			NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
			NUI_INITIALIZE_FLAG_USES_SKELETON);
	}
	KinectV1(DWORD nuiUseFlags)
	{
		setup(nuiUseFlags);
	}
	~KinectV1()
	{
	}
	int setup(DWORD nuiUseFlags)
	{
		color[0] = cv::Vec3b(0, 0, 0);
		color[1] = cv::Vec3b(255, 0, 0);
		color[2] = cv::Vec3b(0, 255, 0);
		color[3] = cv::Vec3b(0, 0, 255);
		color[4] = cv::Vec3b(255, 255, 0);
		color[5] = cv::Vec3b(255, 0, 255);
		color[6] = cv::Vec3b(0, 255, 255);

		// Kinect�̃C���X�^���X�����A������
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
		//	�ǂ̃t���[�����g��������
		useColorFrame = (nuiUseFlags & NUI_INITIALIZE_FLAG_USES_COLOR) && NUI_INITIALIZE_FLAG_USES_COLOR;
		usePlayerFrame = ((nuiUseFlags & NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX) && NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);
		useDepthFrame = usePlayerFrame
			|| ((nuiUseFlags & NUI_INITIALIZE_FLAG_USES_DEPTH) && NUI_INITIALIZE_FLAG_USES_DEPTH);
		useSkeletonFrame = (nuiUseFlags & NUI_INITIALIZE_FLAG_USES_SKELETON) && NUI_INITIALIZE_FLAG_USES_SKELETON;

		beginColorStream();
		beginDepthStream();
		beginSkeletonStream();

		getResolution();
		coordinate();

		return 0;
	}
	int beginColorStream()
	{
		if (!useColorFrame) return -1;
		// Color�X�g���[���̊J�n
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

		hEvents.push_back(hColorEvent);

		return 0;
	}
	int beginDepthStream()
	{
		if (!useDepthFrame) return -1;
		// Depth�X�g���[���̊J�n
		hDepthEvent = CreateEvent(nullptr, true, false, nullptr);
		hResult = pSensor->NuiImageStreamOpen(
			(usePlayerFrame)? NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX : NUI_IMAGE_TYPE_DEPTH,
			NUI_IMAGE_RESOLUTION_640x480,
			0,
			2,
			hDepthEvent,
			&hDepthHandle);
		if (FAILED(hResult)){
			std::cerr << "Error : NuiImageStreamOpen( DEPTH )" << std::endl;
			return -1;
		}

		hEvents.push_back(hDepthEvent);

		return 0;
	}
	int beginSkeletonStream()
	{
		if (!useSkeletonFrame) return -1;
		//	Skeleton�X�g���[���̊J�n
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
	// �𑜓x�̎擾
	void getResolution()
	{
		unsigned long refWidth = 0;
		unsigned long refHeight = 0;
		NuiImageResolutionToSize(NUI_IMAGE_RESOLUTION_640x480, refWidth, refHeight);
		width = static_cast<int>(refWidth);
		height = static_cast<int>(refHeight);
	}
	//	�f�v�X�}�b�v��RGB�J�����Ƃ̑Ή��t���ׂ̈̏���
	int coordinate()
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
		// �t���[���̍X�V�҂�
		if (useColorFrame)	ResetEvent(hColorEvent);
		if (useDepthFrame)	ResetEvent(hDepthEvent);
		if (useSkeletonFrame)	ResetEvent(hSkeletonEvent);
		WaitForMultipleObjects(hEvents.size(), hEvents.data(), true, INFINITE);
	}
	int getColorFrame(cv::Mat& colorMat)
	{
		// Color�J��������t���[�����擾
		hResult = pSensor->NuiImageStreamGetNextFrame(hColorHandle, 0, &pColorFrame);
		if (FAILED(hResult)){
			std::cerr << "Error : NuiImageStreamGetNextFrame( COLOR )" << std::endl;
			return -1;
		}

		// Color�摜�f�[�^�̃��b�N�Ǝ擾
		pColorFrameTexture = pColorFrame.pFrameTexture;
		pColorFrameTexture->LockRect(0, &colorLockedRect, nullptr, 0);

		colorMat = cv::Mat(
			height, width,
			CV_8UC4,
			reinterpret_cast<unsigned char*>(colorLockedRect.pBits));	//	�`�����l������b,g,r,�\���4��

		return 0;
	}
	int getDepthFrame(cv::Mat& depthMat, cv::Mat& playerMask = cv::Mat())
	{
		// Depth�J��������t���[�����擾
		hResult = pSensor->NuiImageStreamGetNextFrame(hDepthHandle, 0, &pDepthFrame);
		if (FAILED(hResult)){
			std::cerr << "Error : NuiImageStreamGetNextFrame( DEPTH )" << std::endl;
			return -1;
		}
		// Depth�摜�f�[�^�̃��b�N�Ǝ擾
		pDepthFrameTexture = nullptr;
		pSensor->NuiImageFrameGetDepthImagePixelFrameTexture(
			hDepthHandle,
			&pDepthFrame,
			&nearMode,
			&pDepthFrameTexture);
		pDepthFrameTexture->LockRect(0, &depthLockedRect, nullptr, 0);

		//	Depth�摜���J���[�摜�Ɉʒu���킹����
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

		//	Depth�Z���T�[��raw�f�[�^����f�v�X�}�b�v�ƃv���C���[ID��؂蕪����
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				unsigned int index = y * width + x;
				depthMat.at<unsigned short>(pColorPoint[index].y, pColorPoint[index].x) = pDepthPixel[index].depth;
				playerMask.at<cv::Vec3b>(pColorPoint[index].y, pColorPoint[index].x) = color[pDepthPixel[index].playerIndex];
			}
		}

		//cv::medianBlur(depthMat, depthMat, 3);
		return 0;
	}
	int getSkeletonJoints(std::vector<cv::Point> joints[NUI_SKELETON_COUNT])
	{
		// Skeleton�t���[�����擾
		hResult = pSensor->NuiSkeletonGetNextFrame(0, &pSkeletonFrame);
		if (FAILED(hResult)){
			std::cout << "Error : NuiSkeletonGetNextFrame" << std::endl;
			return -1;
		}
		//	Skeleton�̊e���W�𒊏o���ĕ`��
		NUI_COLOR_IMAGE_POINT colorPoint;			//	�X�P���g�����\��������W
		for (int count = 0; count < NUI_SKELETON_COUNT; count++)
		{
			NUI_SKELETON_DATA skeletonData = pSkeletonFrame.SkeletonData[count];
			//	�S�Ă̊֐߂��ǐՂł����ꍇ
			if (skeletonData.eTrackingState == NUI_SKELETON_TRACKED)
			{
				for (int position = 0; position < NUI_SKELETON_POSITION_COUNT; position++)
				{
					//	���W���o
					pCoordinateMapper->MapSkeletonPointToColorPoint(
						&skeletonData.SkeletonPositions[position],
						NUI_IMAGE_TYPE_COLOR,
						NUI_IMAGE_RESOLUTION_640x480,
						&colorPoint);
					joints[count].push_back(Point(colorPoint.x, colorPoint.y));
				}
			}
			//	�S�̂̒ǐՂ݂̂ł��Ă���ꍇ
			else if (skeletonData.eTrackingState == NUI_SKELETON_POSITION_ONLY)
			{
				pCoordinateMapper->MapSkeletonPointToColorPoint(
					&skeletonData.SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER],
					NUI_IMAGE_TYPE_COLOR,
					NUI_IMAGE_RESOLUTION_640x480,
					&colorPoint);
				if ((colorPoint.x >= 0)
					&& (colorPoint.x < width)
					&& (colorPoint.y >= 0)
					&& (colorPoint.y < height))
				{
					joints[count].push_back(Point(colorPoint.x, colorPoint.y));
				}
			}
		}
	}
	int drawSkeleton(cv::Mat& dstMat, std::vector<cv::Point> joints[NUI_SKELETON_COUNT])
	{
		if (dstMat.rows == 0 && dstMat.cols == 0) dstMat = cv::Mat::zeros(height, width, CV_8UC3);
		//	Skeleton�̊e���W��`��
		NUI_COLOR_IMAGE_POINT colorPoint;			//	�X�P���g�����\��������W
		for (int count = 0; count < NUI_SKELETON_COUNT; count++)
		{
			//	�S�Ă̊֐߂��ǐՂł��Ă���ꍇ
			if (joints[count].size() > 1)
			{
				//	�֐߂̕`��
				for (int position = 0; position < NUI_SKELETON_POSITION_COUNT; position++)
				{
					cv::circle(dstMat, 
						joints[count].at(position), 10,
						static_cast<cv::Scalar>(color[count + 1]), -1, CV_AA);
				}
				//	���i�̕`��
				//	�Ғ�
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
				//	���r
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
				//	�E�r
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
				//	����
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
				//	�E��
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
			//	�S�̂̒ǐՂ݂̂ł��Ă���ꍇ
			else if (joints[count].size() == 1)
			{
				cv::circle(dstMat, 
					joints[count].at(0), 10,
					static_cast<cv::Scalar>(color[count + 1]), -1, CV_AA);
			}
		}
		return 0;
	}
	void drawSkeleton(cv::Mat& dstMat)
	{
		std::vector<Point> joints[6];
		getSkeletonJoints(joints);
		drawSkeleton(dstMat, joints);
	}
	void cvtDepth2Gray(cv::Mat& depthMat, cv::Mat& grayMat)
	{
		grayMat = cv::Mat::zeros(height, width, CV_8UC1);
		for (int y = 0; y < height; y++){
			for (int x = 0; x < width; x++){
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
				grayMat.at<unsigned short>(y, x) = iv;
			}
		}
		grayMat = ~grayMat;
	}
	void cvtDepth2Color(cv::Mat& depthMat, cv::Mat& colorMat)
	{
		colorMat = cv::Mat::zeros(height, width, CV_8UC3);
		cv::cvtColor(colorMat, colorMat, CV_BGR2HSV);
		for (int y = 0; y < height; y++){
			for (int x = 0; x < width; x++){
				int iv;
				if ((depthMat.at<unsigned short>(y, x) <= 0) || (depthMat.at<unsigned short>(y, x) > maxDepthRange))
				{
					iv = 0;
					matG(colorMat, x, y) = 0;
					matR(colorMat, x, y) = 0;
				}
				else
				{
					iv = 180 * (depthMat.at<unsigned short>(y, x) - minDepthRange) / maxDepthRange;
					if (iv > 180) iv = 180;
					else if (iv < 0) iv = 0;
					matG(colorMat, x, y) = 128;
					matR(colorMat, x, y) = 255;
				}
				matB(colorMat, x, y) = iv;
			}
		}
		cv::cvtColor(colorMat, colorMat, CV_HSV2BGR);
	}
	void depthShading(cv::Mat& depthMat, cv::Mat& srcMat, cv::Mat& dstMat)
	{
		srcMat.convertTo(dstMat, CV_8UC3);
		for (int y = 0; y < height; y++){
			for (int x = 0; x < width; x++){
				//	�א�3pix���͈͊O�̏ꍇ�͐^���Âɂ���
				unsigned short z = depthMat.at<unsigned short>(y, x);
				if ((y >= height) || (x >= width) ||
					(z < 400) || (z > 8000))
				{
					matB(dstMat, x, y) = 0;
					matG(dstMat, x, y) = 0;
					matR(dstMat, x, y) = 0;
					continue;
				}
				//	�א�3pix�̃f�[�^���猻�݂̃s�N�Z�������_�Ƃ���x�N�g�������
				cv::Vec3d p[3] = 
				{
					cv::Vec3d(0.0, 0.0, (double)depthMat.at<unsigned short>(y, x) * 0.5),
					cv::Vec3d(1.0, 0.0, (double)depthMat.at<unsigned short>(y, x+1) * 0.5),
					cv::Vec3d(0.0, 1.0, (double)depthMat.at<unsigned short>(y+1, x) * 0.5)
				};
				cv::Vec3d v[2] = {p[1] - p[0], p[2] - p[0]};
				//	���ˌ��̋������v�Z
				cv::Vec3d n = v[0].cross(v[1]);			//	�O��(���̖ʂ̖@���x�N�g��)
				cv::Vec3d light = cv::Vec3d(0, 0, 1);	//	���� = ���_
				double diffuse = n.dot(light) / cv::norm(n) / cv::norm(light);	//	���ˌ��̋���
				//	���˗��������𗎂�
				matB(dstMat, x, y) = (uchar)(matB(dstMat, x, y) * diffuse);
				matG(dstMat, x, y) = (uchar)(matG(dstMat, x, y) * diffuse);
				matR(dstMat, x, y) = (uchar)(matR(dstMat, x, y) * diffuse);
			}
		}
		cv::medianBlur(dstMat, dstMat, 3);
	}
	void releaseFrames()
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
	void shutdown()
	{
		// Kinect�̏I������
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
};


#endif // KINECTV1ADAPTER_H_
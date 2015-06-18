#pragma once

#include <OpenCVAdapter.hpp>
#include <KinectV1Adapter.h>
#include "HomographicImage.h"

class TextureMappingEngine
{
public:
	bool fullscreen = false;
	int selectedTextuerIndex = -1;
	int selectedTextureVertexIndex = -1;		//	-1:���I�����
	cv::Point2f previousMousePosition;
	HomographicImage hImg[3];

	TextureMappingEngine();
	~TextureMappingEngine();
	void staticTextureMapping(KinectV1 kinect);
};

void modifyVertexCallback(int aEvent, int x, int y, int flags, void *param);		//	����E�B���h�E��̃}�E�X�R�[���o�b�N�֐�
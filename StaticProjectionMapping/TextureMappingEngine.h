#pragma once

#include <OpenCVAdapter.hpp>
#include <KinectV1Adapter.h>
#include "HomographicImage.h"
//#include <iostream>
#include <fstream>

class TextureMappingEngine
{
public:
	bool fullscreen = false;
	int selectedTextuerIndex = 0;
	int selectedTextureVertexIndex = -1;		//	-1:未選択状態
	cv::Point2f previousMousePosition;
	HomographicImage hImg[3];

	TextureMappingEngine();
	~TextureMappingEngine();
	void staticTextureMapping(KinectV1 kinect);
	void saveTextureVertexData(string filename);
	void loadTextureVertexData(string filename);
};

void modifyVertexCallback(int aEvent, int x, int y, int flags, void *param);		//	制御ウィンドウ上のマウスコールバック関数
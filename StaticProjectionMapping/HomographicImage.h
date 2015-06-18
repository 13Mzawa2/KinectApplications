#pragma once

#include <OpenCVAdapter.hpp>
#include "CalibrationEngine.h"

class HomographicImage
{
public:
	//	変数
	cv::Mat srcTexture;					//	ソース画像
	cv::Mat warpedImg;					//	変形後画像
	cv::Mat mask;						//	マスク領域
	cv::vector<cv::Point2f> camPonits;	//	カメラ座標系でのテクスチャ頂点（キャリブレーション後に使用）
	cv::vector<cv::Point2f> proPoints;	//	プロジェクタ座標系でのテクスチャ頂点
	cv::vector<cv::Point2f> texPoints;	//	テクスチャの四隅
	cv::Mat H_pro;						//	投影平面 = H * プロジェクタスクリーン平面
	//	関数宣言
	HomographicImage();
	~HomographicImage();
	HomographicImage(cv::Mat texture);
	
	void loadTexture(cv::Mat texture);
	void initProPoints();
	void calcProHomography();
	void deformation();
	void drawLayer(cv::Mat &projectionImg);
	void drawVertex(cv::Mat &img);
};


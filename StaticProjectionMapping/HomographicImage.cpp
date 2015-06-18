#include "HomographicImage.h"

using namespace cv;

HomographicImage::HomographicImage()
{
	initProPoints();
	warpedImg = Mat(Size(PROJECTOR_WINDOW_WIDTH, PROJECTOR_WINDOW_HEIGHT), CV_8UC3);
}

HomographicImage::~HomographicImage()
{
}

HomographicImage::HomographicImage(cv::Mat texture)
{
	initProPoints();
	loadTexture(texture);
	warpedImg = Mat(Size(PROJECTOR_WINDOW_WIDTH, PROJECTOR_WINDOW_HEIGHT), CV_8UC3);
}

void HomographicImage::loadTexture(cv::Mat texture)
{
	srcTexture = texture.clone();
	texPoints.clear();
	texPoints.push_back(Point2f(0, 0));										//	左上
	texPoints.push_back(Point2f(0, srcTexture.cols - 1));						//	右上
	texPoints.push_back(Point2f(srcTexture.rows - 1, 0));						//	左下
	texPoints.push_back(Point2f(srcTexture.rows - 1, srcTexture.cols - 1));	//	右下
}

void HomographicImage::initProPoints()
{
	proPoints.clear();
	proPoints.push_back(Point2f(200, 200));
	proPoints.push_back(Point2f(200, 400));
	proPoints.push_back(Point2f(400, 200));
	proPoints.push_back(Point2f(400, 400));

}

void HomographicImage::calcProHomography()
{
	H_pro = findHomography(texPoints, proPoints, 0);
}

void HomographicImage::deformation()
{
	//	テクスチャの変形
	warpPerspective(srcTexture, warpedImg, H_pro, warpedImg.size());
	//	マスク画像生成
	mask = Mat(warpedImg.size(), CV_8UC1, Scalar(0, 0, 0));
	cout << proPoints << endl;
	fillConvexPoly(mask, proPoints, Scalar(255, 255, 255));
}

void HomographicImage::drawLayer(cv::Mat &projectionImg)
{
	calcProHomography();
	deformation();
	warpedImg.copyTo(projectionImg, mask);
}
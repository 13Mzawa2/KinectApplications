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
	loadTexture(texture);
	initProPoints();
	warpedImg = Mat(Size(PROJECTOR_WINDOW_WIDTH, PROJECTOR_WINDOW_HEIGHT), CV_8UC3);
}

void HomographicImage::loadTexture(cv::Mat texture)
{
	srcTexture = texture.clone();
	texPoints.clear();
	texPoints.push_back(Point2f(0, 0));										//	左上
	texPoints.push_back(Point2f(srcTexture.cols - 1, 0));						//	右上
	texPoints.push_back(Point2f(0, srcTexture.rows - 1));						//	左下
	texPoints.push_back(Point2f(srcTexture.cols - 1, srcTexture.rows - 1));	//	右下
}

void HomographicImage::initProPoints()
{
	proPoints.clear();
	Point origin;
	Size sz;
	if (srcTexture.empty())
	{
		origin = Point(200, 200);
		sz = Size(200, 200);
	}
	else
	{
		origin = Point(200, 200);
		sz = Size(200, 200*PROJECTOR_WINDOW_HEIGHT/PROJECTOR_WINDOW_WIDTH);
	}
	proPoints.push_back(Point2f(origin.x, origin.y));
	proPoints.push_back(Point2f(origin.x + sz.width, origin.y));
	proPoints.push_back(Point2f(origin.x, origin.y + sz.height));
	proPoints.push_back(Point2f(origin.x + sz.width, origin.y + sz.height));
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
	Point pp[4] = { proPoints.at(0), proPoints.at(1), proPoints.at(3), proPoints.at(2) };
	fillConvexPoly(mask, pp, 4, Scalar(255, 255, 255));
	//imshow("確認用", mask);
	//waitKey(0);
	//cout << proPoints << endl;
}

void HomographicImage::drawLayer(cv::Mat &projectionImg)
{
	calcProHomography();
	deformation();
	warpedImg.copyTo(projectionImg, mask);
}

void HomographicImage::drawVertex(cv::Mat &img)
{
	for (int idx = 0; idx < 4; idx++)
	{
		circle(img, proPoints[idx], 5, Scalar(0, 0, 255), 2);
	}
}
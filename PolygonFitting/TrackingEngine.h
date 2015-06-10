#pragma once

#include "PCLAdapter.h"
#include <OpenCVAdapter.hpp>

using namespace pcl;
using namespace pcl::io;

#define TRACKING_FPFH_RADIUS 0.05
#define TRACKING_HARRIS_RADIUS 10.0

class TrackingEngine
{
public:
	pcl::PolygonMesh		mesh;			//	TrackingEngine���Ŋm�ۂ���郁�b�V���f�[�^
	pcl::PointCloud<PointXYZ>::Ptr	cloud_mesh;		//	���b�V���f�[�^������o�����_�Q

	pcl::PointCloud<FPFHSignature33>::Ptr fpfh_mesh;			//	FPFH����
	pcl::PointCloud<PointXYZI>::Ptr harris_keypoints_mesh;		//	Harris�����_
	pcl::PointCloud<PointXYZ>::Ptr harris_keypoints3D_mesh;		//	Harris�����_�̕\���p

	pcl::PointCloud<PointXYZRGB>::Ptr kinectpoints;			//	kinect���瓾��ꂽ�_�Q�f�[�^

	TrackingEngine();
	~TrackingEngine();
	int importSTLFile(std::string filename);
	void showLoadedMesh(std::string windowname);
	void getLoadedMeshFPFHFeature();
	int numCloudPoints();
	void estimateNormal(pcl::PointCloud<PointXYZ>::Ptr &cloud, pcl::PointCloud<PointNormal>::Ptr &normal, int kSearch);
	void getHarrisKeypointsFromLoadedMesh();
	void loadPointCloudData(cv::Mat cloudMat, cv::Mat colorMat);
};


#pragma once

#include "PCLAdapter.h"

using namespace pcl;
using namespace pcl::io;

#define TRACKING_FPFH_RADIUS 0.05
#define TRACKING_HARRIS_RADIUS 10.0

class TrackingEngine
{
public:
	PolygonMesh		mesh;			//	TrackingEngine���Ŋm�ۂ���郁�b�V���f�[�^
	pcl::PointCloud<FPFHSignature33>::Ptr fpfh_mesh;			//	FPFH����
	pcl::PointCloud<PointXYZI>::Ptr harris_keypoints_mesh;		//	Harris�����_
	pcl::PointCloud<PointXYZ>::Ptr harris_keypoints3D_mesh;		//	Harris�����_�̕\���p

	TrackingEngine();
	~TrackingEngine();
	int importSTLFile(std::string filename);
	void showLoadedMesh(std::string windowname);
	void getLoadedMeshFPFHFeature();
	int numCloudPoints();
	void estimateNormal(pcl::PointCloud<PointXYZ>::Ptr &cloud, pcl::PointCloud<PointNormal>::Ptr &normal, int kSearch);
	void getHarrisKeypointsFromLoadedMesh();
};


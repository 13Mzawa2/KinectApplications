#pragma once

#include "PCLAdapter.h"

using namespace pcl;
using namespace pcl::io;

#define TRACKING_FPFH_RADIUS 0.05
#define TRACKING_HARRIS_RADIUS 10.0

class TrackingEngine
{
public:
	PolygonMesh		mesh;			//	TrackingEngine内で確保されるメッシュデータ
	pcl::PointCloud<FPFHSignature33>::Ptr fpfh_mesh;			//	FPFH特徴
	pcl::PointCloud<PointXYZI>::Ptr harris_keypoints_mesh;		//	Harris特徴点
	pcl::PointCloud<PointXYZ>::Ptr harris_keypoints3D_mesh;		//	Harris特徴点の表示用

	TrackingEngine();
	~TrackingEngine();
	int importSTLFile(std::string filename);
	void showLoadedMesh(std::string windowname);
	void getLoadedMeshFPFHFeature();
	int numCloudPoints();
	void estimateNormal(pcl::PointCloud<PointXYZ>::Ptr &cloud, pcl::PointCloud<PointNormal>::Ptr &normal, int kSearch);
	void getHarrisKeypointsFromLoadedMesh();
};


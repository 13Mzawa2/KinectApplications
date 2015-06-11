#pragma once

#include "PCLAdapter.h"
#include <OpenCVAdapter.hpp>

using namespace pcl;
using namespace pcl::io;

#define TRACKING_FPFH_RADIUS	0.05
#define TRACKING_HARRIS_RADIUS	10.0
#define TRACKING_METER2MILLI	1000.0

class TrackingEngine
{
public:
	pcl::PolygonMesh		mesh;			//	TrackingEngine内で確保されるメッシュデータ
	
	pcl::PointCloud<PointXYZ>::Ptr	cloud_mesh;		//	メッシュデータから取り出した点群
	pcl::PointCloud<FPFHSignature33>::Ptr fpfh_mesh;			//	FPFH特徴
	pcl::PointCloud<PointXYZI>::Ptr harris_keypoints_mesh;		//	Harris特徴点
	pcl::PointCloud<PointXYZ>::Ptr harris_keypoints3D_mesh;		//	Harris特徴点の表示用

	pcl::PointCloud<PointXYZ>::Ptr cloud_kinect;					//	kinectから得られた点群データ
	pcl::PointCloud<PointXYZI>::Ptr harris_keypoints_kinect;		//	Harris特徴点
	pcl::PointCloud<PointXYZ>::Ptr harris_keypoints3D_kinect;		//	Harris特徴点の表示用

	TrackingEngine();
	~TrackingEngine();
	int importSTLFile(std::string filename);
	void showLoadedMesh(std::string windowname);
	void getLoadedMeshFPFHFeature();
	int numCloudPoints();
	void estimateNormal(pcl::PointCloud<PointXYZ>::Ptr &cloud, pcl::PointCloud<PointNormal>::Ptr &normal, int kSearch);
	void getHarrisKeypointsFromLoadedMesh();
	void getHarrisKeypointsFromKinect();
	void loadPointCloudData(cv::Mat &cloudMat);
	void removeFlatSurface(pcl::PointCloud<PointXYZ>::Ptr &src_cloud, pcl::PointCloud<PointXYZ>::Ptr &dst_cloud);
};


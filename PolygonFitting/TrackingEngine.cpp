#include "TrackingEngine.h"


TrackingEngine::TrackingEngine()
{
}


TrackingEngine::~TrackingEngine()
{
}

int TrackingEngine::importSTLFile(std::string filename)
{
	return io::loadPolygonFileSTL(filename, mesh);
}

void TrackingEngine::showLoadedMesh(std::string windowname)
{
	visualization::CloudViewer viewer(windowname);
	boost::shared_ptr<PointCloud<PointXYZ>> pcd_ptr(new PointCloud<PointXYZ>);
	fromPCLPointCloud2(mesh.cloud, *pcd_ptr);
	viewer.showCloud(pcd_ptr);
	while (!viewer.wasStopped()) {
	}
}

void TrackingEngine::getFPFHFeature()
{
	PointCloud<PointXYZ> cloud;
	PointCloud<PointNormal> normals;
	fromPCLPointCloud2(mesh.cloud, cloud);
	
}
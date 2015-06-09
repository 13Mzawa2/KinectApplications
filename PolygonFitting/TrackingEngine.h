#pragma once

#include "PCLAdapter.h"

using namespace pcl;
using namespace pcl::io;

class TrackingEngine
{
public:
	PolygonMesh		mesh;
	TrackingEngine();
	~TrackingEngine();
	int importSTLFile(std::string filename);
	void showLoadedMesh(std::string windowname);
	void getFPFHFeature();
};


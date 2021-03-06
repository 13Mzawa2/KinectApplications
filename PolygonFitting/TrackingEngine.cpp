#include "TrackingEngine.h"


TrackingEngine::TrackingEngine()
{
	fpfh_mesh = PointCloud<FPFHSignature33>::Ptr(new PointCloud<FPFHSignature33>());
	harris_keypoints_mesh = PointCloud<PointXYZI>::Ptr(new PointCloud<PointXYZI>());
	harris_keypoints3D_mesh = PointCloud<PointXYZ>::Ptr(new PointCloud<PointXYZ>());
}


TrackingEngine::~TrackingEngine()
{
}

int TrackingEngine::importSTLFile(std::string filename)
{
	int result = io::loadPolygonFileSTL(filename, mesh);
	if (result != -1)
	{
		cloud_mesh = PointCloud<PointXYZ>::Ptr(new PointCloud<PointXYZ>());
		fromPCLPointCloud2(mesh.cloud, *cloud_mesh);
	}
	return result;
}

//	読み込んだ3Dデータの点群と特徴点を表示
void TrackingEngine::showLoadedMesh(std::string windowname)
{
	//	viewer
	visualization::PCLVisualizer viewer(windowname);
	
	//	色設定
	visualization::PointCloudColorHandlerCustom<PointXYZ> pcColor(cloud_mesh, 255, 255, 255);
	visualization::PointCloudColorHandlerCustom<PointXYZ> kpColor(harris_keypoints3D_mesh, 255, 0, 0);

	//	PointCloudの追加
	viewer.addPointCloud(cloud_mesh, pcColor, "PointCloud.png");
	viewer.addPointCloud(harris_keypoints3D_mesh, kpColor, "Keypoints.png");
	viewer.setPointCloudRenderingProperties(visualization::PCL_VISUALIZER_POINT_SIZE, 7, "Keypoints.png");

	viewer.spinOnce(1000);			//	glutと競合しているのか消えてくれないっぽい
	
}

//	読み込んだ3DデータのFPFH特徴点を抽出
void TrackingEngine::getLoadedMeshFPFHFeature()
{
	//	点群データ，法線データの用意
	PointCloud<Normal>::Ptr normals(new PointCloud<Normal>());
	fromPCLPointCloud2(mesh.cloud, *normals);
	//	FPFH特徴推定器の用意，点群データのロード
	FPFHEstimation<PointXYZ, Normal, FPFHSignature33> fpfh;
	fpfh.setInputCloud(cloud_mesh);
	fpfh.setInputNormals(normals);
	//	kdtreeメソッドを探索アルゴリズムに設定
	search::KdTree<PointXYZ>::Ptr tree(new search::KdTree<PointXYZ>());
	fpfh.setSearchMethod(tree);
	//	特徴記述の半径を設定
	fpfh.setRadiusSearch(TRACKING_FPFH_RADIUS);
	//	特徴量計算
	fpfh.compute(*fpfh_mesh);
}

//	3Dデータの頂点数
int TrackingEngine::numCloudPoints()
{
	return mesh.cloud.width * mesh.cloud.height;
}

//	点群からの法線推定
//	ただしビューポイント側を向いていない法線は反転
void TrackingEngine::estimateNormal(PointCloud<PointXYZ>::Ptr &cloud, PointCloud<PointNormal>::Ptr &normal, int kSearch = 20)
{
	NormalEstimation<PointXYZ, PointNormal> ne;		//	法線推定器
	ne.setInputCloud(cloud);
	//	探索メソッドの設定
	search::KdTree<PointXYZ>::Ptr tree(new search::KdTree<PointXYZ>());
	tree->setInputCloud(cloud);
	ne.setSearchMethod(tree);

	//	出力
	ne.setKSearch(kSearch);
	ne.compute(*normal);
}

//	読み込んだ3DデータからHarris特徴点を抽出
void TrackingEngine::getHarrisKeypointsFromLoadedMesh()
{
	//	Harris特徴検出器の用意
	HarrisKeypoint3D<PointXYZ, PointXYZI> detector;
	detector.setNonMaxSupression(true);
	detector.setRadius(TRACKING_HARRIS_RADIUS);
	detector.setInputCloud(cloud_mesh);
	detector.compute(*harris_keypoints_mesh);
	cout << "keypoints detected: " << harris_keypoints_mesh->size() << endl;
	//	Visualize用Harris特徴点の用意
	PointXYZ temp;
	for (PointCloud<PointXYZI>::iterator it = harris_keypoints_mesh->begin(); it != harris_keypoints_mesh->end(); it++)
	{
		temp = PointXYZ((*it).x, (*it).y, (*it).z);
		harris_keypoints3D_mesh->push_back(temp);
	}
}

//	Kinectから得た点群データからHarris特徴点を抽出
void TrackingEngine::getHarrisKeypointsFromKinect()
{
	//	平面検出と除去
	PointCloud<PointXYZ>::Ptr removed_cloud(new PointCloud<PointXYZ>());
	removeFlatSurface(cloud_kinect, removed_cloud);
	//	初期化
	harris_keypoints_kinect = PointCloud<PointXYZI>::Ptr(new PointCloud<PointXYZI>());
	harris_keypoints3D_kinect = PointCloud<PointXYZ>::Ptr(new PointCloud<PointXYZ>());
	//	Harris特徴検出器の用意
	HarrisKeypoint3D<PointXYZ, PointXYZI> detector;
	detector.setNonMaxSupression(true);
	detector.setRadius(TRACKING_HARRIS_RADIUS);
	detector.setInputCloud(removed_cloud);
	detector.compute(*harris_keypoints_kinect);
	cout << "keypoints detected: " << harris_keypoints_kinect->size() << endl;
	//	Visualize用Harris特徴点の用意
	PointXYZ temp;
	for (PointCloud<PointXYZI>::iterator it = harris_keypoints_kinect->begin(); it != harris_keypoints_kinect->end(); it++)
	{
		temp = PointXYZ((*it).x, (*it).y, (*it).z);
		harris_keypoints3D_kinect->push_back(temp);
	}
}

//	OpenCVのcv::Matで保持している点群データをPointCloud型に変換
void TrackingEngine::loadPointCloudData(cv::Mat &cloudMat)
{
	cloud_kinect = PointCloud<PointXYZ>::Ptr(new PointCloud<PointXYZ>());
	for (int y = 0; y < cloudMat.rows; y++)
	{
		for (int x = 0; x < cloudMat.cols; x++)
		{
			PointXYZ temp;
			cv::Vec3f cloudPoint = cloudMat.at<cv::Vec3f>(y, x);
			if (cloudPoint[2] == 0) continue;
			temp.x = cloudPoint[0] * TRACKING_METER2MILLI;
			temp.y = cloudPoint[1] * TRACKING_METER2MILLI;
			temp.z = cloudPoint[2] * TRACKING_METER2MILLI;
			//temp.r = matR(colorMat, x, y);
			//temp.g = matG(colorMat, x, y);
			//temp.b = matB(colorMat, x, y);
			cloud_kinect->points.push_back(temp);
		}
	}
}

void TrackingEngine::removeFlatSurface(pcl::PointCloud<PointXYZ>::Ptr &cloud, PointCloud<PointXYZ>::Ptr &dstCloud)
{
	pcl::ModelCoefficients::Ptr coefficients(new pcl::ModelCoefficients);
	pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
	// Create the segmentation object
	pcl::SACSegmentation<pcl::PointXYZ> seg;
	// Optional
	seg.setOptimizeCoefficients(true);
	// Mandatory
	seg.setModelType(pcl::SACMODEL_PLANE);
	seg.setMethodType(pcl::SAC_RANSAC);
	seg.setDistanceThreshold(10.0);

	seg.setInputCloud(cloud->makeShared());
	seg.segment(*inliers, *coefficients);

	if (inliers->indices.size() == 0)
	{
		//cout << "点群データから平面が検出できませんでした" << endl;
		return ;
	}

	//	平面フィルタリング実行

	pcl::ExtractIndices<pcl::PointXYZ> extract;
	extract.setInputCloud(cloud->makeShared());
	extract.setIndices(inliers);
	extract.setNegative(true);			//	平面除去オプション
	extract.filter(*dstCloud);
}
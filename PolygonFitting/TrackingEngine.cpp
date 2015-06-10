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

//	読み込んだデータからHarris特徴点を抽出
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

//	OpenCVのcv::Matで保持している点群データをPointCloud型に変換
void TrackingEngine::loadPointCloudData(cv::Mat cloudMat, cv::Mat colorMat)
{
	kinectpoints = PointCloud<PointXYZRGB>::Ptr(new PointCloud<PointXYZRGB>());
	for (int y = 0; y < cloudMat.rows; y++)
	{
		for (int x = 0; x < cloudMat.cols; x++)
		{
			PointXYZRGB temp;
			cv::Vec3f cloudPoint = cloudMat.at<cv::Vec3f>(y, x);
			temp.x = cloudPoint[0];
			temp.y = cloudPoint[1];
			temp.z = cloudPoint[2];
			temp.r = matR(colorMat, x, y);
			temp.g = matG(colorMat, x, y);
			temp.b = matB(colorMat, x, y);
			kinectpoints->points.push_back(temp);
		}
	}
}
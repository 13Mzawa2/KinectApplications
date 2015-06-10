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
	return io::loadPolygonFileSTL(filename, mesh);
}

//	読み込んだ3Dデータの点群と特徴点を表示
void TrackingEngine::showLoadedMesh(std::string windowname)
{
	//	viewer
	visualization::PCLVisualizer viewer(windowname);
	boost::shared_ptr<PointCloud<PointXYZ>> pcd_ptr(new PointCloud<PointXYZ>);
	fromPCLPointCloud2(mesh.cloud, *pcd_ptr);
	
	//	色設定
	visualization::PointCloudColorHandlerCustom<PointXYZ> pcColor(pcd_ptr, 255, 255, 255);
	visualization::PointCloudColorHandlerCustom<PointXYZ> kpColor(harris_keypoints3D_mesh, 255, 0, 0);

	//	PointCloudの追加
	viewer.addPointCloud(pcd_ptr, pcColor, "PointCloud.png");
	viewer.addPointCloud(harris_keypoints3D_mesh, kpColor, "Keypoints.png");
	viewer.setPointCloudRenderingProperties(visualization::PCL_VISUALIZER_POINT_SIZE, 7, "Keypoints.png");

	viewer.spinOnce(1000);			//	glutと競合しているのか消えてくれないっぽい
	
}

//	読み込んだ3DデータのFPFH特徴点を抽出
void TrackingEngine::getLoadedMeshFPFHFeature()
{
	//	点群データ，法線データの用意
	PointCloud<PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>());
	PointCloud<Normal>::Ptr normals(new PointCloud<Normal>());
	fromPCLPointCloud2(mesh.cloud, *cloud);
	fromPCLPointCloud2(mesh.cloud, *normals);
	//	FPFH特徴推定器の用意，点群データのロード
	FPFHEstimation<PointXYZ, Normal, FPFHSignature33> fpfh;
	fpfh.setInputCloud(cloud);
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
	//	点群データ，法線データの用意
	PointCloud<PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>());
	fromPCLPointCloud2(mesh.cloud, *cloud);
	//	Harris特徴検出器の用意
	HarrisKeypoint3D<PointXYZ, PointXYZI> detector;
	detector.setNonMaxSupression(true);
	detector.setRadius(TRACKING_HARRIS_RADIUS);
	detector.setInputCloud(cloud);
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


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

//	�ǂݍ���3D�f�[�^�̓_�Q�Ɠ����_��\��
void TrackingEngine::showLoadedMesh(std::string windowname)
{
	//	viewer
	visualization::PCLVisualizer viewer(windowname);
	
	//	�F�ݒ�
	visualization::PointCloudColorHandlerCustom<PointXYZ> pcColor(cloud_mesh, 255, 255, 255);
	visualization::PointCloudColorHandlerCustom<PointXYZ> kpColor(harris_keypoints3D_mesh, 255, 0, 0);

	//	PointCloud�̒ǉ�
	viewer.addPointCloud(cloud_mesh, pcColor, "PointCloud.png");
	viewer.addPointCloud(harris_keypoints3D_mesh, kpColor, "Keypoints.png");
	viewer.setPointCloudRenderingProperties(visualization::PCL_VISUALIZER_POINT_SIZE, 7, "Keypoints.png");

	viewer.spinOnce(1000);			//	glut�Ƌ������Ă���̂������Ă���Ȃ����ۂ�
	
}

//	�ǂݍ���3D�f�[�^��FPFH�����_�𒊏o
void TrackingEngine::getLoadedMeshFPFHFeature()
{
	//	�_�Q�f�[�^�C�@���f�[�^�̗p��
	PointCloud<Normal>::Ptr normals(new PointCloud<Normal>());
	fromPCLPointCloud2(mesh.cloud, *normals);
	//	FPFH���������̗p�ӁC�_�Q�f�[�^�̃��[�h
	FPFHEstimation<PointXYZ, Normal, FPFHSignature33> fpfh;
	fpfh.setInputCloud(cloud_mesh);
	fpfh.setInputNormals(normals);
	//	kdtree���\�b�h��T���A���S���Y���ɐݒ�
	search::KdTree<PointXYZ>::Ptr tree(new search::KdTree<PointXYZ>());
	fpfh.setSearchMethod(tree);
	//	�����L�q�̔��a��ݒ�
	fpfh.setRadiusSearch(TRACKING_FPFH_RADIUS);
	//	�����ʌv�Z
	fpfh.compute(*fpfh_mesh);
}

//	3D�f�[�^�̒��_��
int TrackingEngine::numCloudPoints()
{
	return mesh.cloud.width * mesh.cloud.height;
}

//	�_�Q����̖@������
//	�������r���[�|�C���g���������Ă��Ȃ��@���͔��]
void TrackingEngine::estimateNormal(PointCloud<PointXYZ>::Ptr &cloud, PointCloud<PointNormal>::Ptr &normal, int kSearch = 20)
{
	NormalEstimation<PointXYZ, PointNormal> ne;		//	�@�������
	ne.setInputCloud(cloud);
	//	�T�����\�b�h�̐ݒ�
	search::KdTree<PointXYZ>::Ptr tree(new search::KdTree<PointXYZ>());
	tree->setInputCloud(cloud);
	ne.setSearchMethod(tree);

	//	�o��
	ne.setKSearch(kSearch);
	ne.compute(*normal);
}

//	�ǂݍ��񂾃f�[�^����Harris�����_�𒊏o
void TrackingEngine::getHarrisKeypointsFromLoadedMesh()
{
	//	Harris�������o��̗p��
	HarrisKeypoint3D<PointXYZ, PointXYZI> detector;
	detector.setNonMaxSupression(true);
	detector.setRadius(TRACKING_HARRIS_RADIUS);
	detector.setInputCloud(cloud_mesh);
	detector.compute(*harris_keypoints_mesh);
	cout << "keypoints detected: " << harris_keypoints_mesh->size() << endl;
	//	Visualize�pHarris�����_�̗p��
	PointXYZ temp;
	for (PointCloud<PointXYZI>::iterator it = harris_keypoints_mesh->begin(); it != harris_keypoints_mesh->end(); it++)
	{
		temp = PointXYZ((*it).x, (*it).y, (*it).z);
		harris_keypoints3D_mesh->push_back(temp);
	}
}

//	OpenCV��cv::Mat�ŕێ����Ă���_�Q�f�[�^��PointCloud�^�ɕϊ�
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
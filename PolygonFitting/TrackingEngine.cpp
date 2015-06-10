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

//	�ǂݍ���3D�f�[�^�̓_�Q�Ɠ����_��\��
void TrackingEngine::showLoadedMesh(std::string windowname)
{
	//	viewer
	visualization::PCLVisualizer viewer(windowname);
	boost::shared_ptr<PointCloud<PointXYZ>> pcd_ptr(new PointCloud<PointXYZ>);
	fromPCLPointCloud2(mesh.cloud, *pcd_ptr);
	
	//	�F�ݒ�
	visualization::PointCloudColorHandlerCustom<PointXYZ> pcColor(pcd_ptr, 255, 255, 255);
	visualization::PointCloudColorHandlerCustom<PointXYZ> kpColor(harris_keypoints3D_mesh, 255, 0, 0);

	//	PointCloud�̒ǉ�
	viewer.addPointCloud(pcd_ptr, pcColor, "PointCloud.png");
	viewer.addPointCloud(harris_keypoints3D_mesh, kpColor, "Keypoints.png");
	viewer.setPointCloudRenderingProperties(visualization::PCL_VISUALIZER_POINT_SIZE, 7, "Keypoints.png");

	viewer.spinOnce(1000);			//	glut�Ƌ������Ă���̂������Ă���Ȃ����ۂ�
	
}

//	�ǂݍ���3D�f�[�^��FPFH�����_�𒊏o
void TrackingEngine::getLoadedMeshFPFHFeature()
{
	//	�_�Q�f�[�^�C�@���f�[�^�̗p��
	PointCloud<PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>());
	PointCloud<Normal>::Ptr normals(new PointCloud<Normal>());
	fromPCLPointCloud2(mesh.cloud, *cloud);
	fromPCLPointCloud2(mesh.cloud, *normals);
	//	FPFH���������̗p�ӁC�_�Q�f�[�^�̃��[�h
	FPFHEstimation<PointXYZ, Normal, FPFHSignature33> fpfh;
	fpfh.setInputCloud(cloud);
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
	//	�_�Q�f�[�^�C�@���f�[�^�̗p��
	PointCloud<PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>());
	fromPCLPointCloud2(mesh.cloud, *cloud);
	//	Harris�������o��̗p��
	HarrisKeypoint3D<PointXYZ, PointXYZI> detector;
	detector.setNonMaxSupression(true);
	detector.setRadius(TRACKING_HARRIS_RADIUS);
	detector.setInputCloud(cloud);
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


#include <opencv2/viz/vizcore.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "OpenCVAdapter.hpp"
#include <iostream>

using namespace cv;
using namespace std;

/**
* @function main
*/
int main()
{
	/// Create a window
	viz::Viz3d myWindow("Coordinate Frame");
	/// Add coordinate axes
	//	XYZ���W�����`�����
	myWindow.showWidget("Coordinate Widget", viz::WCoordinateSystem());

	/// Add line to represent (1,1,1) axis
	viz::WLine axis(Point3f(-1.0f, -1.0f, -1.0f), Point3f(1.0f, 1.0f, 1.0f));
	axis.setRenderingProperty(viz::LINE_WIDTH, 4.0);
	myWindow.showWidget("Line Widget", axis);

	/// Construct a cube widget
	//	�Β��p�̓_���w�肵�Cxyz���W�ɕ��s�Ȓ����̂��쐬
	viz::WCube cube_widget(Point3f(0.5, 0.5, 0.0), Point3f(0.0, 0.0, -0.5), true, viz::Color::blue());
	cube_widget.setRenderingProperty(viz::LINE_WIDTH, 4.0);

	/// Display widget (update if already displayed)
	myWindow.showWidget("Cube Widget", cube_widget);

	//	cloud points ��ǂݍ��ށiCV_32FC3�j
	Mat cloud = viz::readCloud("bunny.ply");
	Mat colors(cloud.size(), CV_8UC3);
	theRNG().fill(colors, RNG::UNIFORM, 50, 255);	//	�F�������_���ɖ��߂�

	//	�f�[�^�_��1/16�ɂ���
	float qnan = numeric_limits<float>::quiet_NaN();
	Mat masked_cloud = cloud.clone();
	for (int i = 0; i < cloud.total(); ++i)
	{
		if (i % 16 != 0)
			masked_cloud.at<Vec3f>(i) = Vec3f(qnan, qnan, qnan);
	}
	viz::WCloud cw(cloud, viz::Color::red());
	cw.setRenderingProperty(viz::LINE_WIDTH, 4.0);
	myWindow.showWidget("bunny", cw, Affine3d().translate(Vec3d(-1.0, 0.0, 0.0)));

	/// Rodrigues vector
	Mat rot_vec = Mat::zeros(1, 3, CV_32F);
	float translation_phase = 0.0, translation = 0.0;
	while (!myWindow.wasStopped())
	{
		//	Rodrigues�̉�]�s��̍쐬
		//	��]�� rot_vec �܂��Ɂ@theta = ||rot_vec|| ������]����s��
		/* Rotation using rodrigues */
		/// Rotate around (1,1,1)
		rot_vec.at<float>(0, 0) += CV_PI * 0.01f;
		rot_vec.at<float>(0, 1) += CV_PI * 0.01f;
		rot_vec.at<float>(0, 2) += CV_PI * 0.01f;
		Mat rot_mat;
		Rodrigues(rot_vec, rot_mat);

		/// Shift on (1,1,1)
		translation_phase += CV_PI * 0.01f;
		translation = sin(translation_phase);

		/// Construct pose
		Affine3f pose(rot_mat, Vec3f(translation, translation, translation));

		myWindow.setWidgetPose("Cube Widget", pose);

		myWindow.spinOnce(1, true);
	}

	return 0;
}
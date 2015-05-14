#include "OpenCVAdapter.hpp"

using namespace viz;

int main(void)
{
	cv::VideoCapture capture(CV_CAP_OPENNI);
	//Viz3d viz("dynamic");
	//... We add contents...

	//we expose a camera rule hardly sideways
	//viz.setViewerPose(Affine3d().translate(Vec3f(1.0, 0.0, 0.0)));

	while (waitKey() != 27)
	{
		//... We update contents...
		//if it is necessary, we change poses at added vidzhetov
		//if it is necessary, we replace clouds new gained with Kinect
		//if it is necessary, we change a camera rule

		capture.grab();
		Mat color, depth;
		capture.retrieve(color, CV_CAP_OPENNI_BGR_IMAGE);

		viz::imshow("colorImg", color);
		//capture.retrieve(depth, CV_CAP_OPENNI_DEPTH_MAP);
		//Mat cloud = computeCloud(depth);
		//Mat display = normalizeDepth(depth);

		//viz.showWidget("cloud", WCloud(cloud, color));
		//viz.showWidget("image", WImageOverlay(color, Rect(0, 0, 240, 160)));

		//otrisovyvaem the user feeding into in flow 30 msec also is processed
		//viz.spinOnce(30/*ms*/, true/*force_redraw*/);
	}
}
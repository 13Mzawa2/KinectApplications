#ifndef PCLADAPTER_H_
#define PCLADAPTER_H_

//	インクルード
#include <pcl\io\pcd_io.h>
#include <pcl\point_types.h>

// ビルドモード
#ifdef _DEBUG
#define PCL_EXT_STR "_debug.lib"
#define PCL_FLANN_EXT_STR "-gd.lib"
#define PCL_QHULL_EXT_STR "_d.lib"
#define PCL_VTK_EXT_STR "-6.2-gd.lib"
#else
#define PCL_EXT_STR "_release.lib"
#define PCL_FLANN_EXT_STR ".lib"
#define PCL_QHULL_EXT_STR ".lib"
#define PCL_VTK_EXT_STR "-6.2.lib"
#endif

// ライブラリのリンク（不要な物はコメントアウト）
#pragma comment(lib, "pcl_common"			PCL_EXT_STR)
#pragma comment(lib, "pcl_features"			PCL_EXT_STR)
#pragma comment(lib, "pcl_filters"			PCL_EXT_STR)
#pragma comment(lib, "pcl_io"				PCL_EXT_STR)
#pragma comment(lib, "pcl_io_ply"			PCL_EXT_STR)
#pragma comment(lib, "pcl_kdtree"			PCL_EXT_STR)
#pragma comment(lib, "pcl_keypoints"		PCL_EXT_STR)
#pragma comment(lib, "pcl_octree"			PCL_EXT_STR)
#pragma comment(lib, "pcl_outofcore"		PCL_EXT_STR)
#pragma comment(lib, "pcl_people"			PCL_EXT_STR)
#pragma comment(lib, "pcl_recognition"		PCL_EXT_STR)
#pragma comment(lib, "pcl_resistration"		PCL_EXT_STR)
#pragma comment(lib, "pcl_sample_consensus"	PCL_EXT_STR)
#pragma comment(lib, "pcl_search"			PCL_EXT_STR)
#pragma comment(lib, "pcl_surface"			PCL_EXT_STR)
#pragma comment(lib, "pcl_tracking"			PCL_EXT_STR)
#pragma comment(lib, "pcl_visualization"	PCL_EXT_STR)

#pragma comment(lib, "flann"		PCL_FLANN_EXT_STR)
#pragma comment(lib, "flann_s"		PCL_FLANN_EXT_STR)
#pragma comment(lib, "flann_cpp_s"	PCL_FLANN_EXT_STR)

#pragma comment(lib, "qhull"	PCL_QHULL_EXT_STR)
#pragma comment(lib, "qhull_p"	PCL_QHULL_EXT_STR)
#pragma comment(lib, "qhullcpp"	PCL_QHULL_EXT_STR)
#pragma comment(lib, "qhullstatic"	PCL_QHULL_EXT_STR)
#pragma comment(lib, "qhullstatic_p"	PCL_QHULL_EXT_STR)


#endif // PCLADAPTER_H_
//------------------------------------------------------------
//		Point Cloud Library Adapter (Library Linker)
//------------------------------------------------------------

/*
	pclを利用するためのリンカスクリプト
	VTKのリンカ設定は当面使わないので除外
	includeは必要なものを書き足していく

	環境変数設定 (PCL v1.7.2 32bit vc120)

	$(PCL_INC) =
		C:\Program Files (x86)\PCL 1.7.2\include\pcl-1.7;
		C:\Program Files (x86)\PCL 1.7.2\3rdParty\Boost\include\boost-1_57;
		C:\Program Files (x86)\PCL 1.7.2\3rdParty\Eigen\eigen3;
		C:\Program Files (x86)\PCL 1.7.2\3rdParty\FLANN\include;
		C:\Program Files (x86)\PCL 1.7.2\3rdParty\Qhull\include\libqhullcpp;
		C:\Program Files (x86)\PCL 1.7.2\3rdParty\VTK\include\vtk-6.2;

	$(PCL_LIB) = 
		C:\Program Files (x86)\PCL 1.7.2\lib;
		C:\Program Files (x86)\PCL 1.7.2\3rdParty\Boost\lib;
		C:\Program Files (x86)\PCL 1.7.2\3rdParty\FLANN\lib;
		C:\Program Files (x86)\PCL 1.7.2\3rdParty\Qhull\lib;
		C:\Program Files (x86)\PCL 1.7.2\3rdParty\VTK\lib\vtk-6.2;

*/


#ifndef PCLADAPTER_H_
#define PCLADAPTER_H_

//	インクルード
#include <pcl\io\pcd_io.h>
#include <pcl\point_types.h>

// ビルドモード
#ifdef _DEBUG
#define PCL_EXT_STR "_debug.lib"
#define PCL_BOOST_EXT_STR "-vc120-mt-gd-1_57.lib"
#define PCL_FLANN_EXT_STR "-gd.lib"
#define PCL_QHULL_EXT_STR "_d.lib"
#define PCL_VTK_EXT_STR "-6.2-gd.lib"
#else
#define PCL_EXT_STR "_release.lib"
#define PCL_BOOST_EXT_STR "-vc120-mt-1_57.lib"
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
#pragma comment(lib, "pcl_registration"		PCL_EXT_STR)
#pragma comment(lib, "pcl_sample_consensus"	PCL_EXT_STR)
#pragma comment(lib, "pcl_search"			PCL_EXT_STR)
#pragma comment(lib, "pcl_surface"			PCL_EXT_STR)
#pragma comment(lib, "pcl_tracking"			PCL_EXT_STR)
#pragma comment(lib, "pcl_visualization"	PCL_EXT_STR)

#pragma comment(lib, "libboost_atomic"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_chrono"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_container"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_context"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_coroutine"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_date_time"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_exception"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_filesystem"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_graph"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_iostreams"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_locale"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_log_setup"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_log"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_math_c99f"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_math_c99l"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_math_c99"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_math_tr1f"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_math_tr1l"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_math_tr1"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_mpi"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_prg_exec_monitor"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_program_options"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_random"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_regex"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_serialization"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_signals"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_system"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_test_exec_monitor"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_thread"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_unit_test_framework"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_wave"		PCL_BOOST_EXT_STR)
#pragma comment(lib, "libboost_wserialization"		PCL_BOOST_EXT_STR)

#pragma comment(lib, "flann"		PCL_FLANN_EXT_STR)
#pragma comment(lib, "flann_s"		PCL_FLANN_EXT_STR)
#pragma comment(lib, "flann_cpp_s"	PCL_FLANN_EXT_STR)

#pragma comment(lib, "qhull"	PCL_QHULL_EXT_STR)
#pragma comment(lib, "qhull_p"	PCL_QHULL_EXT_STR)
#pragma comment(lib, "qhullcpp"	PCL_QHULL_EXT_STR)
#pragma comment(lib, "qhullstatic"	PCL_QHULL_EXT_STR)
#pragma comment(lib, "qhullstatic_p"	PCL_QHULL_EXT_STR)


#endif // PCLADAPTER_H_
#ifndef CALIBRATIONENGINE_H_
#define CALIBRATIONENGINE_H_

#include <KinectV1Adapter.h>

#define PROJECTOR_WINDOW_WIDTH	1024
#define PROJECTOR_WINDOW_HEIGHT	768
#define CAMERA_WINDOW_WIDTH		640
#define CAMERA_WINDOW_HEIGHT	480
#define PROJECTOR_FOVY_X	21.24		//	プロジェクタの仕様から求めたX視野角
#define CAMERA_FOVY_X		45.0

class CalibrationEngine
{
public:
	CalibrationEngine();
	~CalibrationEngine();

};

#endif //	CALIBRATIONENGINE_H_
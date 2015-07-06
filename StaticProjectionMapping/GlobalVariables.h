//	MainWindow, ProjectionWindow�Ԃŋ��n���������ϐ��Q

#ifndef GLOBALVAL_H_
#define GLOBALVAL_H_

#include "ARTKLinker.h"
#include "OBJLoader.h"

#define MARKER_SIZE 48.0f	//	mm

extern Marker marker;		//	���́Fmain.cpp
extern ARParam kinectParam;	//	main.cpp
extern ARGL_CONTEXT_SETTINGS_REF gArglSettings;		//	main.cpp

extern OBJMESH mesh;

#endif //	GLOBALVAL_H_
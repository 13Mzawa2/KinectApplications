//	MainWindow, ProjectionWindow間で橋渡ししたい変数群

#ifndef GLOBALVAL_H_
#define GLOBALVAL_H_

#include "ARTKLinker.h"
#include "OBJLoader.h"

#define MARKER_SIZE 48.0f	//	mm

extern Marker marker;		//	実体：main.cpp
extern ARParam kinectParam;	//	main.cpp
extern ARGL_CONTEXT_SETTINGS_REF gArglSettings;		//	main.cpp

extern OBJMESH mesh;

#endif //	GLOBALVAL_H_
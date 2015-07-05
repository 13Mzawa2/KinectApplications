#ifndef MAIN_H_
#define MAIN_H_

#include <Windows.h>
#include "MainWindow.h"
#include "ProjectionWindow.h"
#include "OpenGLDrawing.h"
#include "ARTKLinker.h"
#include "GlobalVariables.h"

//	�}���`�E�B���h�E�֌W
int mainWindowID, projectionWindowID;
const char *windowName[] = { "Main Window", "Projection Window" };

//	�������p�֐�
void addMainCallbackFunc();
void addProjectionCallbackFunc();
void artkInit();
//	�A�C�h�����O
void idleEvent();

#endif // MAIN_H_
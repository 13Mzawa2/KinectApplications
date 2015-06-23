#ifndef MAIN_H_
#define MAIN_H_

#include <Windows.h>
#include "MainWindow.h"
#include "ProjectionWindow.h"
#include "OpenGLDrawing.h"

//	マルチウィンドウ関係
int mainWindowID, projectionWindowID;
const char *windowName[] = { "Main Window", "Projection Window" };

//	初期化用関数
void addMainCallbackFunc();
void addProjectionCallbackFunc();
//	アイドリング
void idleEvent();

#endif // MAIN_H_
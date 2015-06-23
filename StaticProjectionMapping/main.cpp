#include "main.h"

using namespace cv;


void idleEvent()
{
	glutSetWindow(mainWindowID);
	glutPostRedisplay();
	glutSetWindow(projectionWindowID);
	glutPostRedisplay();
}

void addMainCallbackFunc()
{
	glutDisplayFunc(mainLoop);
	glutReshapeFunc(mainReshapeEvent);			//	ウィンドウの再描画
	glutKeyboardFunc(mainKeyEvent);
	glutMouseFunc(mainMouseEvent);
	glutMotionFunc(mainMotionEvent);
}

void addProjectionCallbackFunc()
{
	glutDisplayFunc(projectionLoop);
	glutReshapeFunc(projectionReshapeEvent);			//	ウィンドウの再描画
	glutKeyboardFunc(projectionKeyEvent);
	glutMouseFunc(projectionMouseEvent);
	glutMotionFunc(projectionMotionEvent);
}

int main(int argc, char** argv)
{
	//	Kinect v1の簡易使用クラス
	kSensor = KinectV1(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);


	//	GLUTの初期化
	glutInit(&argc, argv);

	////	Main Window（主にPointCloud表示）
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	mainWindowID = glutCreateWindow(windowName[0]);
	//	コールバック関数登録
	addMainCallbackFunc();
	//	初期設定
	glClearColor(0., 0., 0., 1.0);		//	black
	glEnable(GL_DEPTH_TEST);


	////	プロジェクタ投影用ウィンドウ
	glutInitWindowPosition(1600, 0);
	glutInitWindowSize(PROJECTOR_WINDOW_WIDTH, PROJECTOR_WINDOW_HEIGHT);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	projectionWindowID = glutCreateWindow(windowName[1]);
	//	コールバック関数登録
	addProjectionCallbackFunc();
	//	初期設定
	glClearColor(0.5, 0.5, 0.5, 1.0);		//	gray
	glEnable(GL_DEPTH_TEST);
	////	メインでないウィンドウを全画面表示にする
	//GLのデバイスコンテキストハンドル取得
	HDC glDc = wglGetCurrentDC();
	//ウィンドウハンドル取得
	HWND hWnd = WindowFromDC(glDc);
	//ウィンドウの属性と位置変更
	SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
	SetWindowPos(hWnd, HWND_TOP,
		PROJ_WIN_POS_X, PROJ_WIN_POS_Y,
		PROJ_WIN_WIDTH, PROJ_WIN_HEIGHT,
		SWP_SHOWWINDOW);

	glutIdleFunc(idleEvent);		//	ここで登録される関数は特定ウィンドウに依存しない

	glewInit();

	//namedWindow("ProjectorWindow", CV_WINDOW_NORMAL);
	glutMainLoop();

	return 0;
}

//	static texture mappin のコールバック関数
void modifyVertexCallback(int aEvent, int x, int y, int flags, void *param)
{
	Point2f currentMousePosition(x, y);

	switch (aEvent)
	{
		//	マウスの移動量に合わせて指定した頂点を移動
	case EVENT_MOUSEMOVE:
		if (tme.selectedTextureVertexIndex >= 0)	//	selected
		{
			tme.hImg[tme.selectedTextuerIndex].proPoints[tme.selectedTextureVertexIndex]
				+= currentMousePosition - tme.previousMousePosition;
		}
		break;
		//	頂点を選択する
	case EVENT_LBUTTONDOWN:
		for (int idx = 0; idx < 4; idx++)
		{
			float d = norm(tme.hImg[tme.selectedTextuerIndex].proPoints[idx] - currentMousePosition);
			if (d < 10.0)
			{
				tme.selectedTextureVertexIndex = idx;
			}
		}
		break;
		//	選択を解除
	case EVENT_LBUTTONUP:
		tme.selectedTextureVertexIndex = -1;
		break;
	}
	tme.previousMousePosition = currentMousePosition;
}
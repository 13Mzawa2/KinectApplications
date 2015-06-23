#include "MainWindow.h"
using namespace cv;

//	自作処理エンジン
KinectV1 kSensor;
CalibrationEngine ce;
TextureMappingEngine tme;

//	必要な画像群
cv::Mat cameraImg;
cv::Mat depthImg;
cv::Mat depthGrayImg;
cv::Mat cloudImg;

//	距離に関するパラメータ　単位：m
const double glZNear = 0.001;		//	カメラからの最小距離
const double glZFar = 10.0;		//	カメラからの最大距離
double glFovy = 45;			//	カメラの視野角(degree)

//	MainWindow操作のためのパラメータ
int FormWidth = 640;			//	フォームの幅
int FormHeight = 480;			//	フォームの高さ
int mButton;					//	ドラッグしているボタンID
float twist, elevation, azimuth;						//	roll, pitch, yaw
float cameraDistance = 0, cameraX = 0, cameraY = 0;		//	カメラ原点からのZ, X, Y 距離
int xBegin, yBegin;				//	ドラッグ開始位置
bool cameraLoop = true;
bool projectorView = false;		//	最初はカメラビュー

//	fps計測
static int GLframe = 0; //フレーム数
static int GLtimenow = 0;//経過時間
static int GLtimebase = 0;//計測開始時間

void mainLoop()
{
	if (cameraLoop)
	{
		getFrames();
	}
	//	OpenGLで描画
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(
		0, 0, 0,				//	視点位置
		0, 0, 10.0,			//	注視点
		0, 1.0, 0);				//	画面の上を表すベクトル

	//	マウスによる視点の変更
	polarview();
	//	座標軸の描画
	drawGlobalXYZ(1.0, 3.0);
	//	PointCloudの描画
	renderPointCloud(cloudImg, cameraImg);

	//	FPS計測
	GLframe++; //フレーム数を＋１
	GLtimenow = glutGet(GLUT_ELAPSED_TIME);//経過時間を取得

	static char str[] = "";
	GLfloat color[] = { 1.0, 1.0, 1.0 };
	if (GLtimenow - GLtimebase > 1000)      //１秒以上たったらfpsを出力
	{
		sprintf_s(str, 50, "fps:%f\r", GLframe*1000.0 / (GLtimenow - GLtimebase));
		GLtimebase = GLtimenow;//基準時間を設定                
		GLframe = 0;//フレーム数をリセット
	}
	render_string(0, 0, str, color);

	glutSwapBuffers();
}
void mainReshapeEvent(int w, int h)
{
	FormWidth = w;
	FormHeight = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//射影変換行列の指定
	gluPerspective(glFovy, (GLfloat)w / (GLfloat)h, glZNear, glZFar);
	glMatrixMode(GL_MODELVIEW);

}
void mainKeyEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'c':			//	キャリブレーション
		break;
	case 't':			//	3つの四角形のフィッティング
		textureMapping();
		break;
	case 'r':			//	MainWindowの視点リセット
		twist = 0; elevation = 0; azimuth = 0;
		cameraDistance = 0, cameraX = 0, cameraY = 0;
		break;
	case 'p':
		changeViewPoint();
		break;
	case VK_ESCAPE:
		kSensor.shutdown();
		cv::destroyAllWindows();
		exit(0);
		break;
	default:
		break;
	}

}
void mainMouseEvent(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		switch (button)
		{
		case GLUT_RIGHT_BUTTON:
		case GLUT_MIDDLE_BUTTON:
		case GLUT_LEFT_BUTTON:
			mButton = button;
			break;
		}
		xBegin = x;
		yBegin = y;
	}


}
void mainMotionEvent(int x, int y)
{
	int xDisp, yDisp;
	xDisp = x - xBegin;
	yDisp = y - yBegin;
	switch (mButton)
	{
	case GLUT_LEFT_BUTTON:
		azimuth += (float)xDisp / 2.0;
		elevation -= (float)yDisp / 2.0;
		break;
	case GLUT_MIDDLE_BUTTON:
		cameraX -= (float)xDisp / 100.0;
		cameraY -= (float)yDisp / 100.0;
		break;
	case GLUT_RIGHT_BUTTON:
		twist -= (float)xDisp / 2.0;
		cameraDistance += yDisp / 100.0;
		break;
	}
	xBegin = x;
	yBegin = y;

}
//----------------------------------------
//		UIのための関数
//----------------------------------------

//	視点変更
void polarview()
{
	glRotatef(-twist, 0.0, 0.0, 1.0);			//	roll
	glRotatef(-elevation, 1.0, 0.0, 0.0);		//	pitch
	glRotatef(-azimuth, 0.0, 1.0, 0.0);			//	yaw
	glTranslatef(cameraX, cameraY, cameraDistance);
}
//	3枚の図形を貼り付ける
void textureMapping()
{
	cameraLoop = false;
	tme.staticTextureMapping(kSensor);
	cameraLoop = true;
}
//	視点の切り替え
void changeViewPoint()
{
	projectorView = !projectorView;
	stringstream ss;
	if (projectorView)
	{
		ss << "プロジェクタ";
		glFovy = 21.24;
		glutReshapeWindow(CAMERA_WINDOW_HEIGHT * PROJECTOR_WINDOW_WIDTH/PROJECTOR_WINDOW_HEIGHT, CAMERA_WINDOW_HEIGHT);
	}
	else
	{
		ss << "カメラ";
		glFovy = 45.0;
		glutReshapeWindow(CAMERA_WINDOW_WIDTH, CAMERA_WINDOW_HEIGHT);
	}
	cout << ss.str() << "座標に切り替えました" << endl;
	cout << "現在の座標" << endl;
	cout << " - Translate(x, y, z): " << cameraX << "," << cameraY << "," << cameraDistance << endl;
	cout << " - Rotate(roll, pitch, yaw): " << -twist << "," << -elevation << "," << -azimuth << endl;
}
//----------------------------------------
//		処理まとめ用関数
//----------------------------------------
void getFrames()
{
	//	Kinectからの読込
	kSensor.waitFrames();
	kSensor.getColorFrame(cameraImg);
	kSensor.getDepthFrameCoordinated(depthImg);
	//blur(depthImg, depthImg, Size(11,11));			//	デプスマップのノイズ対策（橋本ら）
	//kSensor.cvtDepth2Gray(depthImg, depthGrayImg);
	//imshow("cam", cameraImg);
	//imshow("depth", depthGrayImg);
	//flip(depthGrayImg, depthGrayImg, 1);
	kSensor.cvtDepth2Cloud(depthImg, cloudImg);
	kSensor.releaseFrames();

}
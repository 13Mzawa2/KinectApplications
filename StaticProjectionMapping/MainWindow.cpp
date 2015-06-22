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
const int glFovy = 45;			//	カメラの視野角(degree)

//	MainWindow操作のためのパラメータ
int FormWidth = 640;			//	フォームの幅
int FormHeight = 480;			//	フォームの高さ
int mButton;					//	ドラッグしているボタンID
float twist, elevation, azimuth;						//	roll, pitch, yaw
float cameraDistance = 0, cameraX = 0, cameraY = 0;		//	カメラ原点からのZ, X, Y 距離
int xBegin, yBegin;				//	ドラッグ開始位置
bool cameraLoop = true;

void mainLoop()
{
	if (cameraLoop)
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
	//	OpenGLで描画
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(
		0, 0, 0,				//	視点位置
		0, 0, 10.0,				//	注視点
		0, 1.0, 0);				//	画面の上を表すベクトル

	//	視点の変更
	polarview();
	//	座標軸の描画
	drawGlobalXYZ(1.0, 3.0);

	glPointSize(1);
	glBegin(GL_POINTS);
	for (int y = 0; y < cloudImg.rows; y++)
	{
		for (int x = 0; x < cloudImg.cols; x++)
		{
			Vec3f cloudPoint = cloudImg.at<Vec3f>(y, x);
			if (cloudPoint[2] == 0) continue;
			glColor3d(matR(cameraImg, x, y) / 255.0, matG(cameraImg, x, y) / 255.0, matB(cameraImg, x, y) / 255.0);
			glVertex3d(cloudPoint[0], cloudPoint[1], cloudPoint[2]);
		}
	}
	glEnd();


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
		cameraX -= (float)xDisp / 40.0;
		cameraY += (float)yDisp / 40.0;
		break;
	case GLUT_RIGHT_BUTTON:
		cameraDistance += xDisp / 40.0;
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
	glTranslatef(cameraX, cameraY, cameraDistance);
	glRotatef(-twist, 0.0, 0.0, 1.0);			//	roll
	glRotatef(-elevation, 1.0, 0.0, 0.0);		//	pitch
	glRotatef(-azimuth, 0.0, 1.0, 0.0);			//	yaw
}
//	3枚の図形を貼り付ける
void textureMapping()
{
	cameraLoop = false;
	tme.staticTextureMapping(kSensor);
	cameraLoop = true;
}
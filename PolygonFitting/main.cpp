//	Vizライブラリから利用するのに限界を感じたのでglutを利用してみるテスト
#include "main.h"

using namespace cv;

//視点変更
void polarview()
{
	if (calibrated)
	{
		glMultMatrixd((GLdouble*)cEngine.rotate);
	}
	glTranslatef(cameraX, cameraY, cameraDistance);
	glRotatef(-twist, 0.0, 0.0, 1.0);			//	roll
	glRotatef(-azimuth, 0.0, 1.0, 0.0);			//	yaw
	glRotatef(-elevation, 1.0, 0.0, 0.0);		//	pitch
}
void mainLoop()
{
	//	OpenGLで描画
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(
		0, 0, 0,				//	視点位置
		0, 0, 10.0,				//	注視点
		0, 1.0, 0);				//	画面の上を表すベクトル

	//視点の変更
	polarview();

	glLineWidth(3.0);
	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(1, 0, 0);
	glColor3d(0, 1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 1, 0);
	glColor3d(0, 0, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 1);
	glEnd();

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
	//glFlush();
	glutSwapBuffers();
}
void reshape(int w, int h)
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
void glutKeyEvent(unsigned char key, int x, int y)
{
	Mat chess;
	switch (key)
	{
	//	キャリブレーション
	case 'c':
		cEngine.calibrateProKinect(kSensor);
		calibrated = true;
		break;
	//	チェスパターンを出力
	case 'p':
		cEngine.createChessPattern(chess, Scalar(255, 255, 0), Scalar(255, 255,255));
		imwrite("chess7x10.png", chess);
		break;
	//	カメラの自動ホワイトバランス・露出設定
	case 'a':
		setcam();
		break;
	//	STLファイルのインポート
	case 'i':
		import3DFile("drop_x001.stl");
		break;
	//	トラッキング開始フラグ
	case 't':
		isTracking = !isTracking;
		break;
	//	終了処理
	case VK_ESCAPE:	//	Esc
		exit(0);
		break;
	}

}
void glutMouseEvent(int button, int state, int x, int y)
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
void glutMotionEvent(int x, int y)
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
void glutIdleEvent()
{
	//	Kinectからの読込
	kSensor.waitFrames();
	kSensor.getColorFrame(cameraImg);
	kSensor.getDepthFrameCoordinated(depthImg);
	//blur(depthImg, depthImg, Size(11,11));			//	デプスマップのノイズ対策（橋本ら）
	kSensor.cvtDepth2Gray(depthImg, depthGrayImg);
	//imshow("cam", cameraImg);
	//imshow("depth", depthGrayImg);
	flip(depthGrayImg, depthGrayImg, 1);
	kSensor.cvtDepth2Cloud(depthImg, cloudImg);
	kSensor.releaseFrames();
	glutPostRedisplay();		//	再描画
}

int main(int argc, char** argv)
{
	//	Kinect v1の簡易使用クラス
	kSensor = KinectV1(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);

	//	GLUTの初期化，ウィンドウの用意
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Main Window");

	//	コールバック関数登録
	glutDisplayFunc(mainLoop);
	glutReshapeFunc(reshape);				//	ウィンドウの再描画
	glutKeyboardFunc(glutKeyEvent);
	glutMouseFunc(glutMouseEvent);
	glutMotionFunc(glutMotionEvent);
	glutIdleFunc(glutIdleEvent);			//	待ち時間中の動作

	glClearColor(0., 0., 0., 1.0);		//	Neutral Gray
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();

	kSensor.shutdown();
	cv::destroyAllWindows();

	return 0;

}

//------------------------------------------------
//	UIのための関数群
//------------------------------------------------
void setcam()
{
	char yn = 0;
	cout << "カメラの設定をします．" << endl
		<< "現在の設定..." << endl;
	if (kSensor.getAutoWhiteBalance())
		cout << "自動ホワイトバランス：入" << endl;
	else
		cout << "自動ホワイトバランス：切" << endl;
	cout << "現在の設定を変更しますか？(y/n)：";
	cin >> yn;
	kSensor.setAutoWhiteBalance((yn == 'y'));

	if (kSensor.getAutoExposure())
		cout << "自動露出：入" << endl;
	else
		cout << "自動露出：切" << endl;
	cout << "現在の設定を変更しますか？(y/n)：";
	cin >> yn;
	kSensor.setAutoWhiteBalance((yn == 'y'));
	cout << "設定を変更しました．" << endl;

}

bool import3DFile(string filename)
{
	if (tEngine.importSTLFile(filename) == -1)
	{
		cout << filename << "が正常にインポートできませんでした．" << endl;
		return false;
	}
	else
	{
		cout << filename << "をインポートしました．" << endl
			<< "頂点数：" << tEngine.numCloudPoints() << endl;
		tEngine.getHarrisKeypointsFromLoadedMesh();
		tEngine.showLoadedMesh("Imported 3D File");
		return true;
	}
}

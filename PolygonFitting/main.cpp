//	Viz���C�u�������痘�p����̂Ɍ��E���������̂�glut�𗘗p���Ă݂�e�X�g
#include "main.h"

using namespace cv;

//���_�ύX
void polarview()
{
	if (calibrated)
	{
		cameraX = cEngine.transVector.x;
		cameraY = cEngine.transVector.y;
		cameraDistance = cEngine.transVector.z;
		twist = -cEngine.eulerAngles.x;
		azimuth = -cEngine.eulerAngles.z;
		elevation = -cEngine.eulerAngles.y;
	}
	glTranslatef(cameraX, cameraY, cameraDistance);
	glRotatef(-twist, 1.0, 0.0, 0.0);			//	pitch
	glRotatef(-elevation, 0.0, 0.0, 1.0);		//	roll
	glRotatef(-azimuth, 0.0, 1.0, 0.0);			//	yaw
}
void mainLoop()
{
	//	OpenGL�ŕ`��
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(
		0, 0, 0,				//	���_�ʒu
		0, 0, 10.0,				//	�����_
		0, 1.0, 0);				//	��ʂ̏��\���x�N�g��

	//���_�̕ύX
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
	//�ˉe�ϊ��s��̎w��
	gluPerspective(glFovy, (GLfloat)w / (GLfloat)h, glZNear, glZFar);
	glMatrixMode(GL_MODELVIEW);

}
void glutKeyEvent(unsigned char key, int x, int y)
{
	Mat chess;
	switch (key)
	{
	//	�L�����u���[�V����
	case 'c':
		cEngine.calibrateProKinect(kSensor);
		calibrated = true;
		break;
	//	�`�F�X�p�^�[�����o��
	case 'p':
		cEngine.createChessPattern(chess, Scalar(255, 255, 0), Scalar(255, 255,255));
		imwrite("chess7x10.png", chess);
		break;
	//	�J�����̎����z���C�g�o�����X�E�I�o�ݒ�
	case 'a':
		setcam();
		break;
	//	STL�t�@�C���̃C���|�[�g
	case 'i':
		import3DFile("drop_x001.stl");
		break;
	//	�g���b�L���O�J�n�t���O
	case 't':
		isTracking = !isTracking;
		break;
	//	�I������
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
	//	Kinect����̓Ǎ�
	kSensor.waitFrames();
	kSensor.getColorFrame(cameraImg);
	kSensor.getDepthFrameCoordinated(depthImg);
	//blur(depthImg, depthImg, Size(11,11));			//	�f�v�X�}�b�v�̃m�C�Y�΍�i���{��j
	kSensor.cvtDepth2Gray(depthImg, depthGrayImg);
	//imshow("cam", cameraImg);
	//imshow("depth", depthGrayImg);
	flip(depthGrayImg, depthGrayImg, 1);
	kSensor.cvtDepth2Cloud(depthImg, cloudImg);
	kSensor.releaseFrames();
	glutPostRedisplay();		//	�ĕ`��
}

int main(int argc, char** argv)
{
	//	Kinect v1�̊ȈՎg�p�N���X
	kSensor = KinectV1(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);

	//	GLUT�̏������C�E�B���h�E�̗p��
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Main Window");

	//	�R�[���o�b�N�֐��o�^
	glutDisplayFunc(mainLoop);
	glutReshapeFunc(reshape);				//	�E�B���h�E�̍ĕ`��
	glutKeyboardFunc(glutKeyEvent);
	glutMouseFunc(glutMouseEvent);
	glutMotionFunc(glutMotionEvent);
	glutIdleFunc(glutIdleEvent);			//	�҂����Ԓ��̓���

	glClearColor(0., 0., 0., 1.0);		//	Neutral Gray
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();

	kSensor.shutdown();
	cv::destroyAllWindows();

	return 0;

}

//------------------------------------------------
//	UI�̂��߂̊֐��Q
//------------------------------------------------
void setcam()
{
	char yn = 0;
	cout << "�J�����̐ݒ�����܂��D" << endl
		<< "���݂̐ݒ�..." << endl;
	if (kSensor.getAutoWhiteBalance())
		cout << "�����z���C�g�o�����X�F��" << endl;
	else
		cout << "�����z���C�g�o�����X�F��" << endl;
	cout << "���݂̐ݒ��ύX���܂����H(y/n)�F";
	cin >> yn;
	kSensor.setAutoWhiteBalance((yn == 'y'));

	if (kSensor.getAutoExposure())
		cout << "�����I�o�F��" << endl;
	else
		cout << "�����I�o�F��" << endl;
	cout << "���݂̐ݒ��ύX���܂����H(y/n)�F";
	cin >> yn;
	kSensor.setAutoWhiteBalance((yn == 'y'));
	cout << "�ݒ��ύX���܂����D" << endl;

}

bool import3DFile(string filename)
{
	if (tEngine.importSTLFile(filename) == -1)
	{
		cout << filename << "������ɃC���|�[�g�ł��܂���ł����D" << endl;
		return false;
	}
	else
	{
		cout << filename << "���C���|�[�g���܂����D" << endl
			<< "���_���F" << tEngine.numCloudPoints() << endl;
		tEngine.getHarrisKeypointsFromLoadedMesh();
		tEngine.showLoadedMesh("Imported 3D File");
		return true;
	}
}

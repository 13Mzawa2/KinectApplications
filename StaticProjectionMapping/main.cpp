#include "main.h"

using namespace cv;


//���_�ύX
void polarview()
{
	glTranslatef(cameraX, cameraY, cameraDistance);
	glRotatef(-twist, 0.0, 0.0, 1.0);			//	roll
	glRotatef(-elevation, 1.0, 0.0, 0.0);		//	pitch
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
void reshapeEvent(int w, int h)
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
	switch (key)
	{
	case 'c':			//	�L�����u���[�V����
		break;
	case 'f':			//	3�̎l�p�`�̃t�B�b�e�B���O
		break;
	case 'r':			//	MainWindow�̎��_���Z�b�g
		break;
	case VK_ESCAPE:
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
	glutReshapeFunc(reshapeEvent);			//	�E�B���h�E�̍ĕ`��
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
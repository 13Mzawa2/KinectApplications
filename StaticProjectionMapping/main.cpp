#include "main.h"

using namespace cv;


void mainLoop()
{
	if (cameraLoop)
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

	}
	//	OpenGL�ŕ`��
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(
		0, 0, 0,				//	���_�ʒu
		0, 0, 10.0,				//	�����_
		0, 1.0, 0);				//	��ʂ̏��\���x�N�g��

	//	���_�̕ύX
	polarview();
	//	���W���̕`��
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
	case 't':			//	3�̎l�p�`�̃t�B�b�e�B���O
		textureMapping();
		break;
	case 'r':			//	MainWindow�̎��_���Z�b�g
		twist = 0; elevation = 0; azimuth = 0;
		cameraDistance = 0, cameraX = 0, cameraY = 0;
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
	glutPostRedisplay();		//	�ĕ`��
}


int main(int argc, char** argv)
{
	//	Kinect v1�̊ȈՎg�p�N���X
	kSensor = KinectV1(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);

	//	�v���W�F�N�^���e�p�E�B���h�E
	namedWindow("ProjectorWindow", CV_WINDOW_NORMAL);

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
//----------------------------------------
//		UI�̂��߂̊֐�
//----------------------------------------

//	���_�ύX
void polarview()
{
	glTranslatef(cameraX, cameraY, cameraDistance);
	glRotatef(-twist, 0.0, 0.0, 1.0);			//	roll
	glRotatef(-elevation, 1.0, 0.0, 0.0);		//	pitch
	glRotatef(-azimuth, 0.0, 1.0, 0.0);			//	yaw
}
//	3���̐}�`��\��t����
void textureMapping()
{
	cameraLoop = false;
	tme.staticTextureMapping(kSensor);
	cameraLoop = true;
}
//	static texture mappin �̃R�[���o�b�N�֐�
void modifyVertexCallback(int aEvent, int x, int y, int flags, void *param)
{
	Point2f currentMousePosition(x, y);

	switch (aEvent)
	{
		//	�}�E�X�̈ړ��ʂɍ��킹�Ďw�肵�����_���ړ�
	case EVENT_MOUSEMOVE:
		if (tme.selectedTextureVertexIndex >= 0)	//	selected
		{
			tme.hImg[tme.selectedTextuerIndex].proPoints[tme.selectedTextureVertexIndex]
				+= currentMousePosition - tme.previousMousePosition;
		}
		break;
		//	���_��I������
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
		//	�I��������
	case EVENT_LBUTTONUP:
		tme.selectedTextureVertexIndex = -1;
		break;
	}
	tme.previousMousePosition = currentMousePosition;
}
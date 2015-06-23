#include "MainWindow.h"
using namespace cv;

//	���쏈���G���W��
KinectV1 kSensor;
CalibrationEngine ce;
TextureMappingEngine tme;

//	�K�v�ȉ摜�Q
cv::Mat cameraImg;
cv::Mat depthImg;
cv::Mat depthGrayImg;
cv::Mat cloudImg;

//	�����Ɋւ���p�����[�^�@�P�ʁFm
const double glZNear = 0.001;		//	�J��������̍ŏ�����
const double glZFar = 10.0;		//	�J��������̍ő勗��
const int glFovy = 45;			//	�J�����̎���p(degree)

//	MainWindow����̂��߂̃p�����[�^
int FormWidth = 640;			//	�t�H�[���̕�
int FormHeight = 480;			//	�t�H�[���̍���
int mButton;					//	�h���b�O���Ă���{�^��ID
float twist, elevation, azimuth;						//	roll, pitch, yaw
float cameraDistance = 0, cameraX = 0, cameraY = 0;		//	�J�������_�����Z, X, Y ����
int xBegin, yBegin;				//	�h���b�O�J�n�ʒu
bool cameraLoop = true;

//	fps�v��
static int GLframe = 0; //�t���[����
static int GLtimenow = 0;//�o�ߎ���
static int GLtimebase = 0;//�v���J�n����

void mainLoop()
{
	if (cameraLoop)
	{
		//	Kinect����̓Ǎ�
		kSensor.waitFrames();
		kSensor.getColorFrame(cameraImg);
		kSensor.getDepthFrameCoordinated(depthImg);
		//blur(depthImg, depthImg, Size(11,11));			//	�f�v�X�}�b�v�̃m�C�Y�΍�i���{��j
		//kSensor.cvtDepth2Gray(depthImg, depthGrayImg);
		//imshow("cam", cameraImg);
		//imshow("depth", depthGrayImg);
		//flip(depthGrayImg, depthGrayImg, 1);
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

	//	FPS�v��
	GLframe++; //�t���[�������{�P
	GLtimenow = glutGet(GLUT_ELAPSED_TIME);//�o�ߎ��Ԃ��擾

	static char str[] = "";
	GLfloat color[] = { 1.0, 1.0, 1.0 };
	if (GLtimenow - GLtimebase > 1000)      //�P�b�ȏソ������fps���o��
	{
		sprintf_s(str, 50, "fps:%f\r", GLframe*1000.0 / (GLtimenow - GLtimebase));
		GLtimebase = GLtimenow;//����Ԃ�ݒ�                
		GLframe = 0;//�t���[���������Z�b�g
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
	//�ˉe�ϊ��s��̎w��
	gluPerspective(glFovy, (GLfloat)w / (GLfloat)h, glZNear, glZFar);
	glMatrixMode(GL_MODELVIEW);

}
void mainKeyEvent(unsigned char key, int x, int y)
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
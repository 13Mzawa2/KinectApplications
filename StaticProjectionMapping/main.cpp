#include "main.h"

using namespace cv;

OBJMESH mesh;		//	�y�b�g�{�g�����b�V��
Marker marker = { "Data/markerB.pat", -1, 0, 0, MARKER_SIZE, { 0.0, 0.0 } };
ARGL_CONTEXT_SETTINGS_REF gArglSettings;
ARParam kinectParam;		//	kinect�̃J�����p�����[�^
static GLdouble kinectCameraMat[3][4] = {
	{ 526.37013657, 0.00000000, 313.68782938, 0.0000000 },
	{ 0.00000000, 526.37013657, 259.01834898, 0.0000000 },
	{ 0.00000000, 0.00000000, 1.00000000, 0.0000000 }
};
static GLdouble kinectDistCoeffs[4] = { 
	0.18126525, -0.39866885, 0.00000000, 0.00000000
};

void artkInit()
{
	if ((gArglSettings = arglSetupForCurrentContext()) == NULL) {
		fprintf(stderr, "main(): arglSetupForCurrentContext() returned error.\n");
		exit(-1);
	}
	//	�}�[�J�[�̓ǂݍ���
	if ((marker.patt_id = arLoadPatt(marker.patt_name)) < 0)
	{
		return;
	}
	//	Kinect�̃J�����p�����[�^�ݒ�(���ړ���)
	ARParam wparam;
	//wparam.xsize = 640;
	//wparam.ysize = 480;
	//for (int i = 0; i < 3; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		wparam.mat[i][j] = kinectCameraMat[i][j];
	//	}
	//}
	//for (int i = 0; i < 4; i++)
	//{
	//	wparam.dist_factor[i] = kinectDistCoeffs[i];
	//}
	if (arParamLoad("data/webcam_param.dat", 1, &wparam) < 0)
	{
		cout << "�J�����p�����[�^�̓Ǎ��Ɏ��s���܂���" << endl;
		exit(-1);
	}
	arParamChangeSize(&wparam, 640, 480, &kinectParam);
	arInitCparam(&kinectParam);
}

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
	glutReshapeFunc(mainReshapeEvent);			//	�E�B���h�E�̍ĕ`��
	glutKeyboardFunc(mainKeyEvent);
	glutMouseFunc(mainMouseEvent);
	glutMotionFunc(mainMotionEvent);
}

void addProjectionCallbackFunc()
{
	glutDisplayFunc(projectionLoop);
	glutReshapeFunc(projectionReshapeEvent);			//	�E�B���h�E�̍ĕ`��
	glutKeyboardFunc(projectionKeyEvent);
	glutMouseFunc(projectionMouseEvent);
	glutMotionFunc(projectionMotionEvent);
	glutMouseWheelFunc(projectionMouseWheelEvent);			//	�}�E�X�z�C�[��
}

int main(int argc, char** argv)
{
	//	Kinect v1�̊ȈՎg�p�N���X
	kSensor = KinectV1(
		NUI_INITIALIZE_FLAG_USES_COLOR |
		NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);


	//	GLUT�̏�����
	glutInit(&argc, argv);

	////	Main Window�i���PointCloud�\���j
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	mainWindowID = glutCreateWindow(windowName[0]);
	//	�R�[���o�b�N�֐��o�^
	addMainCallbackFunc();
	//	�����ݒ�
	glClearColor(0., 0., 0., 1.0);		//	black
	glEnable(GL_DEPTH_TEST);
	artkInit();

	////	�v���W�F�N�^���e�p�E�B���h�E
	glutInitWindowPosition(PROJ_WIN_POS_X, PROJ_WIN_POS_Y);
	glutInitWindowSize(PROJECTOR_WINDOW_WIDTH, PROJECTOR_WINDOW_HEIGHT);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	projectionWindowID = glutCreateWindow(windowName[1]);
	//	�R�[���o�b�N�֐��o�^
	addProjectionCallbackFunc();
	//	�����ݒ�
	glClearColor(0., 0., 0., 1.0);		//	gray
	glEnable(GL_DEPTH_TEST);
	////	���C���łȂ��E�B���h�E��S��ʕ\���ɂ���
	//GL�̃f�o�C�X�R���e�L�X�g�n���h���擾
	//glutSetWindow(projectionWindowID);
	HDC glDc = wglGetCurrentDC();
	//�E�B���h�E�n���h���擾
	HWND hWnd = WindowFromDC(glDc);
	//�E�B���h�E�̑����ƈʒu�ύX
	SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
	SetWindowPos(hWnd, HWND_TOP,
		PROJ_WIN_POS_X, PROJ_WIN_POS_Y,
		PROJ_WIN_WIDTH, PROJ_WIN_HEIGHT,
		SWP_SHOWWINDOW);

	glutIdleFunc(idleEvent);		//	�����œo�^�����֐��͓���E�B���h�E�Ɉˑ����Ȃ�

	glewInit();

	//	obj�t�@�C���̃��[�h
	if (!mesh.LoadFile("data/drop_x005.obj"))
		return 0;

	glutMainLoop();

	return 0;
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
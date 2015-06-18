#include "TextureMappingEngine.h"

using namespace cv;

TextureMappingEngine::TextureMappingEngine()
{
}


TextureMappingEngine::~TextureMappingEngine()
{
}

void TextureMappingEngine::staticTextureMapping(KinectV1 kinect)
{
	Mat temp;
	temp = imread("kinoko.jpg");
	hImg[0].loadTexture(temp);
	hImg[1].loadTexture(imread("takenoko.jpg"));
	hImg[2].loadTexture(imread("takenoko.jpg"));
	for (int idx = 0; idx < 3; idx++)
		hImg[idx].initProPoints();
	namedWindow("ControlWindow");
	setMouseCallback("ControlWindow", modifyVertexCallback, 0);

	Mat projectFrame = cv::Mat(Size(PROJECTOR_WINDOW_WIDTH, PROJECTOR_WINDOW_HEIGHT), CV_8UC3);
	Mat controlFrame = cv::Mat(Size(PROJECTOR_WINDOW_WIDTH, PROJECTOR_WINDOW_HEIGHT), CV_8UC3);

	while (1)
	{
		////	kinect����̉摜�m�F
		//Mat colorImg;
		//kinect.waitFrames();
		//kinect.getColorFrame(colorImg);
		//flip(colorImg, colorImg, 1);		//	���E���]
		//imshow("�m�F�p", colorImg);
		//	Frame�w�i��h��Ԃ�
		projectFrame = Scalar(0,0,0);			//	���i���e���Ȃ��j
		controlFrame = Scalar(100, 100, 100);	//	�Â߂̃O���[
		// Frame�ɑ΂��Ďl�p�`��`��
		for (int idx = 0; idx < 3; idx++)
		{
			hImg[idx].drawLayer(projectFrame);
			hImg[idx].drawLayer(controlFrame);
			hImg[idx].drawVertex(controlFrame);
		}
		imshow("ProjectorWindow", projectFrame);
		imshow("ControlWindow", controlFrame);

		//	�L�[�C�x���g����R�[�h
		switch (waitKey(10))
		{
		case VK_ESCAPE:
			destroyWindow("ControlWindow");
			return ;
		case ' ':		//	�t���X�N���[����
			fullscreen = !fullscreen;
			if (fullscreen)
			{
				setWindowProperty("ProjectorWindow", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
				cout << "�t���X�N���[���ɂ��܂���" << endl;
			}
			else
			{
				setWindowProperty("ProjectorWindow", CV_WND_PROP_FULLSCREEN, CV_WINDOW_NORMAL);
			}
			break;
			//	�e�N�X�`���I��
		case '1':
			selectedTextuerIndex = 0;
			cout << "�e�N�X�`��0��I��" << endl;
			break;
		case '2':
			selectedTextuerIndex = 1;
			cout << "�e�N�X�`��1��I��" << endl;
			break;
		case '3':
			selectedTextuerIndex = 2;
			cout << "�e�N�X�`��2��I��" << endl;
			break;

		}

	}
}

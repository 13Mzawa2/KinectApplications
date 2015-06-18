#pragma once

#include <OpenCVAdapter.hpp>
#include "CalibrationEngine.h"

class HomographicImage
{
public:
	//	�ϐ�
	cv::Mat srcTexture;					//	�\�[�X�摜
	cv::Mat warpedImg;					//	�ό`��摜
	cv::Mat mask;						//	�}�X�N�̈�
	cv::vector<cv::Point2f> camPonits;	//	�J�������W�n�ł̃e�N�X�`�����_�i�L�����u���[�V������Ɏg�p�j
	cv::vector<cv::Point2f> proPoints;	//	�v���W�F�N�^���W�n�ł̃e�N�X�`�����_
	cv::vector<cv::Point2f> texPoints;	//	�e�N�X�`���̎l��
	cv::Mat H_pro;						//	���e���� = H * �v���W�F�N�^�X�N���[������
	//	�֐��錾
	HomographicImage();
	~HomographicImage();
	HomographicImage(cv::Mat texture);
	
	void loadTexture(cv::Mat texture);
	void initProPoints();
	void calcProHomography();
	void deformation();
	void drawLayer(cv::Mat &projectionImg);
};


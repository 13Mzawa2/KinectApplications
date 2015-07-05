#ifndef OPENGLDRAWING_H_
#define OPENGLDRAWING_H_

#include <GLEW\glew.h>
#include <GL\freeglut.h>
#include "Quaternion.h"
#include <OpenCVAdapter.hpp>

#define GL_PI CV_PI

#pragma comment(lib, "glew32.lib")
//	�^����ꂽ�C���[�W���o�b�N�O���E���h�ɕ`��
inline void renderBackgroundImage(cv::Mat image)
{
	cv::Mat dstImg;
	cv::cvtColor(image, dstImg, CV_BGR2RGB);	//	OpenGL��RGB����
	cv::flip(dstImg, dstImg, 0);		//	OpenGL�͍������_
	//	���e�s��̏�����
	glMatrixMode(GL_PROJECTION);
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
	{
		glLoadIdentity();
		glRasterPos2i(-1, -1);		//	�񎟌��摜���_�������ɐݒ�
		glDrawPixels(dstImg.cols, dstImg.rows, GL_RGB, GL_UNSIGNED_BYTE, dstImg.data);
	}
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
}

//	�^����ꂽ�_�Q�f�[�^��`��
inline void renderPointCloud(cv::Mat pointcloud, cv::Mat colorData)
{
	glPointSize(1);
	glBegin(GL_POINTS);
	for (int y = 0; y < pointcloud.rows; y++)
	{
		for (int x = 0; x < pointcloud.cols; x++)
		{
			cv::Vec3f cloudPoint = pointcloud.at<cv::Vec3f>(y, x);
			if (cloudPoint[2] == 0) continue;
			glColor3d(matR(colorData, x, y) / 255.0, matG(colorData, x, y) / 255.0, matB(colorData, x, y) / 255.0);
			glVertex3d(cloudPoint[0], cloudPoint[1], cloudPoint[2]);
		}
	}
	glEnd();
}

#define DRAW_FLAG_X	0x1
#define DRAW_FLAG_Y	0x2
#define DRAW_FLAG_Z	0x4

//	���W�n�̕`��
inline void drawGlobalXYZ(GLfloat length, GLfloat width, int flag = DRAW_FLAG_X | DRAW_FLAG_Y | DRAW_FLAG_Z)
{
	glLineWidth(width);
	glBegin(GL_LINES);
	if (flag & DRAW_FLAG_X)
	{
		glColor3d(1, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(length, 0, 0);
	}
	if (flag & DRAW_FLAG_Y)
	{
		glColor3d(0, 1, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, length, 0);
	}
	if (flag & DRAW_FLAG_Z)
	{
		glColor3d(0, 0, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, length);
	}
	glEnd();
}

//	������̕`��@�E�C���h�E���W�n
inline void render_string(float x, float y, const char* string, GLfloat *colorv)
{
	glPushMatrix();

	float z = -1.0f;
	glColor3fv(colorv);
	glWindowPos3f(x, y, z);
	char* p = (char*)string;
	while (*p != '\0') glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);
	
	glPopMatrix();
}

inline static void myBox(double x, double y, double z)
{
	//	�e���W�l
	GLdouble vertex[][3] = {
		{ -x/2, -y/2, -z/2 },
		{ x/2, -y/2, -z/2 },
		{ x/2, y/2, -z/2 },
		{ -x/2, y/2, -z/2 },
		{ -x/2, -y/2, z/2 },
		{ x/2, -y/2, z/2 },
		{ x/2, y/2, z/2 },
		{ -x/2, y/2, z/2 }
	};
	//	�����̖̂�
	const static int face[][4] = {
		{ 0, 1, 2, 3 },
		{ 1, 5, 6, 2 },
		{ 5, 4, 7, 6 },
		{ 4, 0, 3, 7 },
		{ 4, 5, 1, 0 },
		{ 3, 2, 6, 7 }
	};
	//	�ʂ̖@��
	const static GLdouble normal[][3] = {
		{ 0.0, 0.0, -1.0 },
		{ 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0 },
		{ -1.0, 0.0, 0.0 },
		{ 0.0, -1.0, 0.0 },
		{ 0.0, 1.0, 0.0 }
	};
	//	�`��
	int i, j;
	glBegin(GL_QUADS);
	for (j = 0; j < 6; ++j) {
		glNormal3dv(normal[j]);
		for (i = 4; --i >= 0;) {
			glVertex3dv(vertex[face[j][i]]);
		}
	}
	glEnd();
	//	�֊s��
	glColor3d(0.0, 0.0, 0.0);
	glLineWidth(3.0);
	glBegin(GL_LINES);
	for (j = 0; j < 6; ++j) {
		for (i = 0; i < 3; i ++) {
			glVertex3dv(vertex[face[j][i]]);
			glVertex3dv(vertex[face[j][i+1]]);
		}
	}
	glEnd();
	
}



#endif //	OPENGLDRAWING_H_
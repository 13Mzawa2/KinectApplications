#ifndef OPENGLDRAWING_H_
#define OPENGLDRAWING_H_

#include <GLEW\glew.h>
#include <GL\freeglut.h>

#pragma comment(lib, "glew32.lib")


inline void drawGlobalXYZ(GLfloat length, GLfloat width)
{
	glLineWidth(width);
	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(length, 0, 0);
	glColor3d(0, 1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, length, 0);
	glColor3d(0, 0, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, length);
	glEnd();
}

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


#endif //	OPENGLDRAWING_H_
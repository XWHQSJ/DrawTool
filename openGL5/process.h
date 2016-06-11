#pragma once
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mouse.h"

#define SIZE 512                                       //�����

GLint HITS;

//���ƺ���
void drawObjects(GLenum mode)
{
}

//ʰȡ�������
void processSelect(GLint hits, GLuint buffer[])
{
	unsigned int i, j;
	GLuint names;                                     //ͼ������
	GLuint *ptr;                                      //����ָ��
	GLuint minZ;
	GLuint *ptrNames;                                 //������
	GLuint numberOfNames;                             //������Ŀ

	printf("hits = %d\n", hits);
	ptr = (GLuint *)buffer;
	minZ = 0xffffffff;
	for (i = 0; i < hits; i++)
	{
		names = *ptr;
		ptr++;
		if (*ptr < minZ) {
			numberOfNames = names;
			minZ = *ptr;
			ptrNames = ptr + 2;
		}

		ptr += names + 2;
	}

	printf("The closest hit names are ");
	ptr = ptrNames;

	for (j = 0; j < numberOfNames; j++, ptr++)
	{
		printf("%d ", *ptr);
	}

	printf("\n");
}

//ʰȡ����
void SelectObject(int btn, int state, GLint x, GLint y)
{
	GLuint selectBuff[SIZE] = { 0 };                                                    //����һ������ѡ����������  
	GLint hits;                                                                         //��ű�ѡ�е�����ĸ���
	GLint viewport[4];                                                                  //��ſ��ӻ�����Ĳ���

	if (glutGetModifiers() == GLUT_ACTIVE_CTRL && btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		glGetIntegerv(GL_VIEWPORT, viewport);                                           //���viewport  
		glSelectBuffer(64, selectBuff);                                                 //����OpenGL��ʼ��  selectbuffer  
		glRenderMode(GL_SELECT);                                                        //����ѡ��ģʽ  

		glInitNames();                                                                  //��ʼ������ջ  
		glPushName(0);                                                                  //������ջ�з���һ����ʼ�����֣�����Ϊ��0��  

		glMatrixMode(GL_PROJECTION);                                                    //����ͶӰ�׶�׼��ʰȡ  
		glPushMatrix();                                                                 //������ǰ��ͶӰ����  
		glLoadIdentity();                                                               //���뵥λ����  

		float m[16];

		glGetFloatv(GL_PROJECTION_MATRIX, m);
		gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 5.0, 5.0, viewport);    //��������ѡ���ͶӰ����ջ

		glGetFloatv(GL_PROJECTION_MATRIX, m);
		glOrtho(-10, 10, -10, 10, -10, 10);                                             //ʰȡ�������ͶӰ���������Ϳ�����ѡ���Ŵ�Ϊ������һ����  
		glGetFloatv(GL_PROJECTION_MATRIX, m);

		drawObjects(GL_SELECT);				                                            // �ú�������Ⱦ���壬���Ҹ������趨����  

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();                                                                  // ����������ͶӰ�任  
		glGetFloatv(GL_PROJECTION_MATRIX, m);

		hits = glRenderMode(GL_RENDER);                                                 // ��ѡ��ģʽ��������ģʽ,�ú�������ѡ�񵽶���ĸ���  

		if (hits != 0)
		{
			processSelect(hits, selectBuff);                                            //  ѡ��������  
			printf("\n");
		}

		glutPostRedisplay();
	}

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		glRenderMode(GL_RENDER);
		drawObjects(GL_RENDER);
		glutPostRedisplay();
	}
}
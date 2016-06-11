#pragma once
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mouse.h"

#define SIZE 512                                       //点击数

GLint HITS;

//绘制函数
void drawObjects(GLenum mode)
{
}

//拾取输出函数
void processSelect(GLint hits, GLuint buffer[])
{
	unsigned int i, j;
	GLuint names;                                     //图形名称
	GLuint *ptr;                                      //计数指针
	GLuint minZ;
	GLuint *ptrNames;                                 //计数名
	GLuint numberOfNames;                             //名称数目

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

//拾取函数
void SelectObject(int btn, int state, GLint x, GLint y)
{
	GLuint selectBuff[SIZE] = { 0 };                                                    //创建一个保存选择结果的数组  
	GLint hits;                                                                         //存放被选中的物体的个数
	GLint viewport[4];                                                                  //存放可视化区域的参数

	if (glutGetModifiers() == GLUT_ACTIVE_CTRL && btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		glGetIntegerv(GL_VIEWPORT, viewport);                                           //获得viewport  
		glSelectBuffer(64, selectBuff);                                                 //告诉OpenGL初始化  selectbuffer  
		glRenderMode(GL_SELECT);                                                        //进入选择模式  

		glInitNames();                                                                  //初始化名字栈  
		glPushName(0);                                                                  //在名字栈中放入一个初始化名字，这里为‘0’  

		glMatrixMode(GL_PROJECTION);                                                    //进入投影阶段准备拾取  
		glPushMatrix();                                                                 //保存以前的投影矩阵  
		glLoadIdentity();                                                               //载入单位矩阵  

		float m[16];

		glGetFloatv(GL_PROJECTION_MATRIX, m);
		gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 5.0, 5.0, viewport);    //创建用于选择的投影矩阵栈

		glGetFloatv(GL_PROJECTION_MATRIX, m);
		glOrtho(-10, 10, -10, 10, -10, 10);                                             //拾取矩阵乘以投影矩阵，这样就可以让选择框放大为和视体一样大  
		glGetFloatv(GL_PROJECTION_MATRIX, m);

		drawObjects(GL_SELECT);				                                            // 该函数中渲染物体，并且给物体设定名字  

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();                                                                  // 返回正常的投影变换  
		glGetFloatv(GL_PROJECTION_MATRIX, m);

		hits = glRenderMode(GL_RENDER);                                                 // 从选择模式返回正常模式,该函数返回选择到对象的个数  

		if (hits != 0)
		{
			processSelect(hits, selectBuff);                                            //  选择结果处理  
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
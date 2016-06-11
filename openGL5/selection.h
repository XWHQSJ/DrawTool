#pragma once
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

GLfloat red = 0.0, green = 0.0, blue = 1.0;

//ͼ��ѡ��
void graphics(GLint selectedOption)
{
	switch (selectedOption)
	{
	case 1:
		Line = true, LCircle = false, PCircle = false, LRectanger = false, PRectanger = false, LTriangle = false, PTriangle = false;
		break;
	case 2:
		Line = false, LCircle = true, PCircle = false, LRectanger = false, PRectanger = false, LTriangle = false, PTriangle = false;
		break;
	case 3:
		Line = false, LCircle = false, PCircle = true, LRectanger = false, PRectanger = false, LTriangle = false, PTriangle = false;
		break;
	case 4:
		Line = false, LCircle = false, PCircle = false, LRectanger = true, PRectanger = false, LTriangle = false, PTriangle = false;
		break;
	case 5:
		Line = false, LCircle = false, PCircle = false, LRectanger = false, PRectanger = true, LTriangle = false, PTriangle = false;
		break;
	case 6:
		Line = false, LCircle = false, PCircle = false, LRectanger = false, PRectanger = false, LTriangle = true, PTriangle = false;
		break;
	case 7:
		Line = false, LCircle = false, PCircle = false, LRectanger = false, PRectanger = false, LTriangle = false, PTriangle = true;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

//��ɫѡ��
void colorSelection(GLint selectedOption)
{
	switch (selectedOption)
	{
	case 1:
		red = 1.0, green = 0.0, blue = 0.0;
		break;
	case 2:
		red = 0.0, green = 1.0, blue = 0.0;
		break;
	case 3:
		red = 0.0, green = 0.0, blue = 1.0;
		break;
	case 4:
		red = 1.0, green = 1.0, blue = 0.0;
		break;
	case 5:
		red = 1.0, green = 0.0, blue = 1.0;
		break;
	case 6:
		red = 0.0, green = 1.0, blue = 1.0;
		break;
	case 7:
		red = 1.0, green = 1.0, blue = 1.0;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

//ͼ��ģʽѡ��
void polygonModeSelection(GLint selectedOption)
{
	switch (selectedOption)
	{
	case 1:
		//�޸Ķ��������Ϊ���ģʽ
		glPolygonMode(GL_FRONT, GL_FILL);
		break;
	case 2:
		//�޸Ķ��������Ϊ��ģʽ
		glPolygonMode(GL_FRONT, GL_LINE);
		break;
	case 3:
		//�޸Ķ��������Ϊ�����ģʽ
		glPolygonMode(GL_FRONT, GL_POINT);
		break;
	case 4:
		//�޸Ķ���η���Ϊ���ģʽ
		glPolygonMode(GL_BACK, GL_FILL);
		break;
	case 5:
		//�޸Ķ���η���Ϊ��ģʽ
		glPolygonMode(GL_BACK, GL_LINE);
		break;
	case 6:
		//�޸Ķ���η���Ϊ��ģʽ
		glPolygonMode(GL_BACK, GL_POINT);
		break;
	case 7:
		//���ö���ε���ӰģʽΪƽ������ģʽ
		glShadeModel(GL_FLAT);
		break;
	case 8:
		//���ö���ε���ӰģʽΪ�⻬����ģʽ
		glShadeModel(GL_SMOOTH);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

//����ģʽѡ��
void linesModeSelection(GLint selectedOption)
{
	switch (selectedOption)
	{
	case 1:
		glLineStipple(1, 0x00FF);
		glEnable(GL_LINE_STIPPLE);
		break;
	case 2:
		glLineStipple(2, 0x00FF);
		glEnable(GL_LINE_STIPPLE);
		break;
	case 3:
		glLineStipple(1, 0x0C0F);
		glEnable(GL_LINE_STIPPLE);
		break;
	case 4:
		glLineStipple(2, 0x0C0F);
		glEnable(GL_LINE_STIPPLE);
		break;
	case 5:
		glLineStipple(1, 0xAAAA);
		glEnable(GL_LINE_STIPPLE);
		break;
	case 6:
		glLineStipple(2, 0xAAAA);
		glEnable(GL_LINE_STIPPLE);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
#pragma once
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

GLfloat red = 0.0, green = 0.0, blue = 1.0;

//图形选择
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

//颜色选择
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

//图形模式选择
void polygonModeSelection(GLint selectedOption)
{
	switch (selectedOption)
	{
	case 1:
		//修改多边形正面为填充模式
		glPolygonMode(GL_FRONT, GL_FILL);
		break;
	case 2:
		//修改多边形正面为线模式
		glPolygonMode(GL_FRONT, GL_LINE);
		break;
	case 3:
		//修改多边形正面为点填充模式
		glPolygonMode(GL_FRONT, GL_POINT);
		break;
	case 4:
		//修改多边形反面为填充模式
		glPolygonMode(GL_BACK, GL_FILL);
		break;
	case 5:
		//修改多边形反面为线模式
		glPolygonMode(GL_BACK, GL_LINE);
		break;
	case 6:
		//修改多边形反面为点模式
		glPolygonMode(GL_BACK, GL_POINT);
		break;
	case 7:
		//设置多边形的阴影模式为平面明暗模式
		glShadeModel(GL_FLAT);
		break;
	case 8:
		//设置多边形的阴影模式为光滑明暗模式
		glShadeModel(GL_SMOOTH);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

//线形模式选择
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
#pragma once
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define N 3

GLsizei winWidth = 500, winHeight = 500;
GLboolean Line, LCircle, PCircle, LRectanger, PRectanger, LTriangle, PTriangle; //定义的图形
GLint xstart, ystart, xend, yend;                                               //鼠标的坐标
GLfloat r;                                                                      //圆的半径
GLint NUM = 500;                                                                  
const static GLfloat pi = 3.1415926;                                              
GLint i;                                                                        //线的宽度值
int ver = 0;                                                                    //三角形的顶点
GLfloat triVertex[N][N];                                                        //存储顶点的数组

//鼠标移动检测函数
void mouseMove(int xmove, int ymove)
{
	//画线
	if (Line)
	{
		glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_XOR);
		glLineWidth(i);

		glBegin(GL_LINES);
		glVertex2i(xstart, ystart);
		glVertex2i(xend, yend);
		glEnd();

		xend = xmove;
		yend = winHeight - ymove;

		glBegin(GL_LINES);
		glVertex2i(xstart, ystart);
		glVertex2i(xend, yend);
		glEnd();

		glFlush();
	}

	//画线矩
	else if (LRectanger)
	{
		glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_XOR);
		glLineWidth(i);

		glBegin(GL_LINE_LOOP);
		glVertex2i(xstart, ystart);
		glVertex2i(xend, ystart);
		glVertex2i(xend, yend);
		glVertex2i(xstart, yend);
		glEnd();

		xend = xmove;
		yend = winHeight - ymove;

		glBegin(GL_LINE_LOOP);
		glVertex2i(xstart, ystart);
		glVertex2i(xstart, yend);
		glVertex2i(xend, yend);
		glVertex2i(xend, ystart);
		glEnd();

		glFlush();
	}

	//画图矩
	else if (PRectanger)
	{
		glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_XOR);

		glBegin(GL_POLYGON);
		glVertex2i(xstart, ystart);
		glVertex2i(xend, ystart);
		glVertex2i(xend, yend);
		glVertex2i(xstart, yend);
		glEnd();

		xend = xmove;
		yend = winHeight - ymove;

		glBegin(GL_POLYGON);
		glVertex2i(xstart, ystart);
		glVertex2i(xstart, yend);
		glVertex2i(xend, yend);
		glVertex2i(xend, ystart);
		glEnd();

		glFlush();
	}

	//画线圆
	else if (LCircle)
	{
		glEnable(GL_COLOR_LOGIC_OP);                                                           //启动逻辑异或
		glLogicOp(GL_XOR);
		glLineWidth(i);

		r = sqrt((xstart - xend)*(xstart - xend) + (ystart - yend)*(ystart - yend)*1.0) / 2;   //计算圆的半径

		glBegin(GL_LINE_LOOP);						 						                   // 绘制圆(new)
		for (int i = 0; i<NUM; i++)
		{
			glVertex2f((xstart + xend) / 2 + r*float(sin(2 * pi*i / NUM)), (ystart + yend) / 2 + r*float(cos(2 * pi*i / NUM)));
		}
		glEnd();                                                                               // 圆绘制结束

		xend = xmove;
		yend = winHeight - ymove;
		r = sqrt((xstart - xend)*(xstart - xend) + (ystart - yend)*(ystart - yend)*1.0) / 2;   //计算圆的半径

		glBegin(GL_LINE_LOOP);						 						                   // 绘制圆(new)
		for (int i = 0; i<NUM; i++)
		{
			glVertex2f((xstart + xend) / 2 + r*float(sin(2 * pi*i / NUM)), (ystart + yend) / 2 + r*float(cos(2 * pi*i / NUM)));
		}
		glEnd();
		// 圆绘制结束
		glFlush();
	}

	//画图圆
	else if (PCircle)
	{
		glEnable(GL_COLOR_LOGIC_OP);                                                           //启动逻辑异或
		glLogicOp(GL_XOR);

		r = sqrt((xstart - xend)*(xstart - xend) + (ystart - yend)*(ystart - yend)*1.0) / 2;   //计算圆的半径

		glBegin(GL_POLYGON);						 						                   // 绘制圆(new)
		for (int i = 0; i<NUM; i++)
		{
			glVertex2f((xstart + xend) / 2 + r*float(sin(2 * pi*i / NUM)), (ystart + yend) / 2 + r*float(cos(2 * pi*i / NUM)));
		}
		glEnd();                                                                               // 圆绘制结束

		xend = xmove;
		yend = winHeight - ymove;
		r = sqrt((xstart - xend)*(xstart - xend) + (ystart - yend)*(ystart - yend)*1.0) / 2;   //计算圆的半径

		glBegin(GL_POLYGON);						 						                   // 绘制圆(new)
		for (int i = 0; i<NUM; i++)
		{
			glVertex2f((xstart + xend) / 2 + r*float(sin(2 * pi*i / NUM)), (ystart + yend) / 2 + r*float(cos(2 * pi*i / NUM)));
		}
		glEnd();
		// 圆绘制结束
		glFlush();
	}

	//线角
	else if (LTriangle)
	{

	}

	//图角
	else if (PTriangle)
	{

	}
}

//鼠标操作函数
void mouseud(int btn, int state, int x, int y)
{
	if (Line)
	{
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			xstart = x;
			ystart = winHeight - y;
			xend = xstart;
			yend = ystart;
		}
		else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP)
		{
			xend = x;
			yend = winHeight - y;

			glDisable(GL_COLOR_LOGIC_OP);
			glLineWidth(i);
			glBegin(GL_LINES);
			glVertex2i(xstart, ystart);
			glVertex2i(xend, yend);
			glEnd();

			glFlush();
		}
	}

	else if (LRectanger)
	{
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			xstart = x;
			ystart = winHeight - y;
			xend = xstart;
			yend = ystart;
		}
		else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP)
		{
			xend = x;
			yend = winHeight - y;

			glDisable(GL_COLOR_LOGIC_OP);
			glLineWidth(i);
			glBegin(GL_LINE_LOOP);
			glVertex2i(xstart, ystart);
			glVertex2i(xstart, yend);
			glVertex2i(xend, yend);
			glVertex2i(xend, ystart);
			glEnd();

			glFlush();
		}
	}

	else if (PRectanger)
	{
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			xstart = x;
			ystart = winHeight - y;
			xend = xstart;
			yend = ystart;
		}
		else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP)
		{
			xend = x;
			yend = winHeight - y;

			glDisable(GL_COLOR_LOGIC_OP);
			glBegin(GL_POLYGON);
			glVertex2i(xstart, ystart);
			glVertex2i(xstart, yend);
			glVertex2i(xend, yend);
			glVertex2i(xend, ystart);
			glEnd();

			glFlush();
		}
	}

	else if (LCircle)
	{
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			xstart = x;
			ystart = winHeight - y;
			xend = xstart;
			yend = ystart;
		}
		else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP)
		{
			xend = x;
			yend = winHeight - y;

			glDisable(GL_COLOR_LOGIC_OP);                                                          //之前的全是异或操作

			r = sqrt((xstart - xend)*(xstart - xend) + (ystart - yend)*(ystart - yend)*1.0) / 2;   //计算圆的半径

			glLineWidth(i);
			glBegin(GL_LINE_LOOP);						 						                   // 绘制圆(new)
			for (int i = 0; i<NUM; i++)
			{
				glVertex2f((xstart + xend) / 2 + r*float(sin(2 * pi*i / NUM)), (ystart + yend) / 2 + r*float(cos(2 * pi*i / NUM)));
			}
			glEnd();

			glFlush();
		}
	}

	else if (PCircle)
	{
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			xstart = x;
			ystart = winHeight - y;
			xend = xstart;
			yend = ystart;
		}
		else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP)
		{
			xend = x;
			yend = winHeight - y;

			glDisable(GL_COLOR_LOGIC_OP);                                                          //之前的全是异或操作

			r = sqrt((xstart - xend)*(xstart - xend) + (ystart - yend)*(ystart - yend)*1.0) / 2;   //计算圆的半径

			glBegin(GL_POLYGON);						 						                   // 绘制圆(new)
			for (int i = 0; i<NUM; i++)
			{
				glVertex2f((xstart + xend) / 2 + r*float(sin(2 * pi*i / NUM)), (ystart + yend) / 2 + r*float(cos(2 * pi*i / NUM)));
			}
			glEnd();

			glFlush();
		}
	}

	else if (LTriangle)
	{
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			triVertex[ver][0] = x;
			triVertex[ver][1] = winHeight - y;

			glBegin(GL_POINTS);
			glVertex2f(triVertex[ver][0], triVertex[ver][1]);
			glEnd();

			glFlush();

			ver++;
			if (ver == 3)
			{
				glLineWidth(i);
				glBegin(GL_LINE_LOOP);
				glVertex2f(triVertex[0][0], triVertex[0][1]);
				glVertex2f(triVertex[1][0], triVertex[1][1]);
				glVertex2f(triVertex[2][0], triVertex[2][1]);
				glEnd();

				glFlush();

				ver = 0;
			}

		}
	}

	else if (PTriangle)
	{
		if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			triVertex[ver][0] = x;
			triVertex[ver][1] = winHeight - y;

			glBegin(GL_POINTS);
			glVertex2f(triVertex[ver][0], triVertex[ver][1]);
			glEnd();

			glFlush();

			ver++;
			if (ver == 3)
			{
				glBegin(GL_TRIANGLES);
				glVertex2f(triVertex[0][0], triVertex[0][1]);
				glVertex2f(triVertex[1][0], triVertex[1][1]);
				glVertex2f(triVertex[2][0], triVertex[2][1]);
				glEnd();

				glFlush();

				ver = 0;
			}

		}
	}

}
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mouse.h"
#include "process.h"
#include "selection.h"

GLenum fillMode = GL_SMOOTH;                                 //设置默认的图形模式

//初始化
void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);                                      //设置窗口颜色为黑色
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 300.0, 0.0, 300.0);
}


void display(void)
{
	glColor3f(red, green, blue);                             //设置默认图形颜色
	drawObjects(GL_RENDER);       
	glFlush();
}

//主菜单函数
void fillOption(GLint s)
{
	switch (s)
	{
	case 6:                                                  //清除函数
		glClear(GL_COLOR_BUFFER_BIT);
		glFlush();
		break;
	case 7:                                                  //退出函数
		exit(0);
	default:
		break;
	}
}

void reshape(GLint newWidth, GLint newHeight)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, newWidth, newHeight);                   // 设置视口大小为增个窗口大小 
	gluOrtho2D(0.0, (GLfloat)newWidth, 0.0, (GLfloat)newHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char **argv)
{
	printf("Please input the width of the line : \n");       //设置线形图形的线宽 
	scanf("%d", &i);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("by 徐皖辉 2014060103018");

	init();
	glutDisplayFunc(display);

	//添加子菜单
	//图形类型菜单
	GLint SubMenu1 = glutCreateMenu(graphics);
	glutAddMenuEntry("Line", 1);                                  //添加画线子菜单
	glutAddMenuEntry("LCircle", 2);                               //添加画线圆子菜单
	glutAddMenuEntry("PCircle", 3);                               //添加画图圆子菜单
	glutAddMenuEntry("LRectanger", 4);                            //添加画线矩子菜单
	glutAddMenuEntry("PRectanger", 5);                            //添加画图矩子菜单
	glutAddMenuEntry("LTriangle", 6);                             //添加画线角子菜单
	glutAddMenuEntry("PTriangle", 7);                             //添加画图角子菜单

	//颜色选择菜单
	GLint SubMenu2 = glutCreateMenu(colorSelection);              //创建颜色选择子菜单
	glutAddMenuEntry("Red", 1);                                   //红色
	glutAddMenuEntry("Green", 2);                                 //绿色
	glutAddMenuEntry("Blue", 3);                                  //蓝色
	glutAddMenuEntry("Yellow", 4);                                //黄色
	glutAddMenuEntry("Purple", 5);                                //紫色
	glutAddMenuEntry("Cyan", 6);                                  //青色
	glutAddMenuEntry("White", 7);                                 //白色

	//多边形模式选择菜单
	GLint SubMenu3 = glutCreateMenu(polygonModeSelection);        //创建模式选择子菜单
	glutAddMenuEntry("FrontFill", 1);                             //正面填充模式
	glutAddMenuEntry("FrontLines", 2);                            //正面线模式
	glutAddMenuEntry("FrontPoints", 3);                           //正面点模式
	glutAddMenuEntry("BackFill", 4);                              //反面填充模式
	glutAddMenuEntry("BackLines", 5);                             //反面线模式
	glutAddMenuEntry("BackPoints", 6);                            //反面点模式

	GLint SubMenu4 = glutCreateMenu(polygonModeSelection);        
	glutAddMenuEntry("Flat", 7);                                  
	glutAddMenuEntry("Smooth", 8);

	//线形模式选择菜单
	GLint SubMenu5 = glutCreateMenu(linesModeSelection);          //创建模式选择子菜单
	glutAddMenuEntry("1_0x00FF", 1);
	glutAddMenuEntry("2_0x00FF", 2);
	glutAddMenuEntry("1_0x0C0", 3);
	glutAddMenuEntry("2_0x0C0", 4);
	glutAddMenuEntry("1_0xAAAA", 5);
	glutAddMenuEntry("2_0xAAAA", 6);

	//清除屏幕内容菜单
	GLint SubMenu6 = glutCreateMenu(fillOption);

	//退出屏幕菜单
	GLint SubMenu7 = glutCreateMenu(fillOption);

	//主菜单
	glutCreateMenu(fillOption);                                   //创建主菜单.该函数为所创建菜单返回一个独一无二的标识符。
	glutAddSubMenu("Graphics", SubMenu1);                         //图形选择菜单
	glutAddSubMenu("ColorSelection", SubMenu2);                   //颜色选择菜单
	glutAddSubMenu("PolygonMode", SubMenu3);                      //图形模式选择菜单
	glutAddSubMenu("ColorMode", SubMenu4);                        //颜色模式选择菜单
	glutAddSubMenu("LinesMode", SubMenu5);                        //线形模式选择菜单
	glutAddMenuEntry("Clear", SubMenu6);                          //清屏菜单
	glutAddMenuEntry("Exit", SubMenu7);                           //退出菜单
	glutAttachMenu(GLUT_RIGHT_BUTTON);                            //把菜单附加给一个鼠标右键

	glutMotionFunc(mouseMove);
	glutMouseFunc(mouseud);
	glutReshapeFunc(reshape);

	glutMainLoop();

	return 0;
}

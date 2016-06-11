#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mouse.h"
#include "process.h"
#include "selection.h"

GLenum fillMode = GL_SMOOTH;                                 //����Ĭ�ϵ�ͼ��ģʽ

//��ʼ��
void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);                                      //���ô�����ɫΪ��ɫ
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 300.0, 0.0, 300.0);
}


void display(void)
{
	glColor3f(red, green, blue);                             //����Ĭ��ͼ����ɫ
	drawObjects(GL_RENDER);       
	glFlush();
}

//���˵�����
void fillOption(GLint s)
{
	switch (s)
	{
	case 6:                                                  //�������
		glClear(GL_COLOR_BUFFER_BIT);
		glFlush();
		break;
	case 7:                                                  //�˳�����
		exit(0);
	default:
		break;
	}
}

void reshape(GLint newWidth, GLint newHeight)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, newWidth, newHeight);                   // �����ӿڴ�СΪ�������ڴ�С 
	gluOrtho2D(0.0, (GLfloat)newWidth, 0.0, (GLfloat)newHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char **argv)
{
	printf("Please input the width of the line : \n");       //��������ͼ�ε��߿� 
	scanf("%d", &i);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("by ����� 2014060103018");

	init();
	glutDisplayFunc(display);

	//����Ӳ˵�
	//ͼ�����Ͳ˵�
	GLint SubMenu1 = glutCreateMenu(graphics);
	glutAddMenuEntry("Line", 1);                                  //��ӻ����Ӳ˵�
	glutAddMenuEntry("LCircle", 2);                               //��ӻ���Բ�Ӳ˵�
	glutAddMenuEntry("PCircle", 3);                               //��ӻ�ͼԲ�Ӳ˵�
	glutAddMenuEntry("LRectanger", 4);                            //��ӻ��߾��Ӳ˵�
	glutAddMenuEntry("PRectanger", 5);                            //��ӻ�ͼ���Ӳ˵�
	glutAddMenuEntry("LTriangle", 6);                             //��ӻ��߽��Ӳ˵�
	glutAddMenuEntry("PTriangle", 7);                             //��ӻ�ͼ���Ӳ˵�

	//��ɫѡ��˵�
	GLint SubMenu2 = glutCreateMenu(colorSelection);              //������ɫѡ���Ӳ˵�
	glutAddMenuEntry("Red", 1);                                   //��ɫ
	glutAddMenuEntry("Green", 2);                                 //��ɫ
	glutAddMenuEntry("Blue", 3);                                  //��ɫ
	glutAddMenuEntry("Yellow", 4);                                //��ɫ
	glutAddMenuEntry("Purple", 5);                                //��ɫ
	glutAddMenuEntry("Cyan", 6);                                  //��ɫ
	glutAddMenuEntry("White", 7);                                 //��ɫ

	//�����ģʽѡ��˵�
	GLint SubMenu3 = glutCreateMenu(polygonModeSelection);        //����ģʽѡ���Ӳ˵�
	glutAddMenuEntry("FrontFill", 1);                             //�������ģʽ
	glutAddMenuEntry("FrontLines", 2);                            //������ģʽ
	glutAddMenuEntry("FrontPoints", 3);                           //�����ģʽ
	glutAddMenuEntry("BackFill", 4);                              //�������ģʽ
	glutAddMenuEntry("BackLines", 5);                             //������ģʽ
	glutAddMenuEntry("BackPoints", 6);                            //�����ģʽ

	GLint SubMenu4 = glutCreateMenu(polygonModeSelection);        
	glutAddMenuEntry("Flat", 7);                                  
	glutAddMenuEntry("Smooth", 8);

	//����ģʽѡ��˵�
	GLint SubMenu5 = glutCreateMenu(linesModeSelection);          //����ģʽѡ���Ӳ˵�
	glutAddMenuEntry("1_0x00FF", 1);
	glutAddMenuEntry("2_0x00FF", 2);
	glutAddMenuEntry("1_0x0C0", 3);
	glutAddMenuEntry("2_0x0C0", 4);
	glutAddMenuEntry("1_0xAAAA", 5);
	glutAddMenuEntry("2_0xAAAA", 6);

	//�����Ļ���ݲ˵�
	GLint SubMenu6 = glutCreateMenu(fillOption);

	//�˳���Ļ�˵�
	GLint SubMenu7 = glutCreateMenu(fillOption);

	//���˵�
	glutCreateMenu(fillOption);                                   //�������˵�.�ú���Ϊ�������˵�����һ����һ�޶��ı�ʶ����
	glutAddSubMenu("Graphics", SubMenu1);                         //ͼ��ѡ��˵�
	glutAddSubMenu("ColorSelection", SubMenu2);                   //��ɫѡ��˵�
	glutAddSubMenu("PolygonMode", SubMenu3);                      //ͼ��ģʽѡ��˵�
	glutAddSubMenu("ColorMode", SubMenu4);                        //��ɫģʽѡ��˵�
	glutAddSubMenu("LinesMode", SubMenu5);                        //����ģʽѡ��˵�
	glutAddMenuEntry("Clear", SubMenu6);                          //�����˵�
	glutAddMenuEntry("Exit", SubMenu7);                           //�˳��˵�
	glutAttachMenu(GLUT_RIGHT_BUTTON);                            //�Ѳ˵����Ӹ�һ������Ҽ�

	glutMotionFunc(mouseMove);
	glutMouseFunc(mouseud);
	glutReshapeFunc(reshape);

	glutMainLoop();

	return 0;
}

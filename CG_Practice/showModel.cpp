//showModel���� ���� 3D���� ���������ִ� �ڵ�

#define _CRT_SECURE_NO_WARNINGS
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

using namespace std;
#define WIRE 0
#define SHADE 1

typedef struct { //3���� ��ǥ ����
	float x;
	float y;
	float z;
} Point;

typedef struct { //�ε��� ����
	unsigned int ip[3];
} Face;

GLfloat xLocation = 0, yLocation = 0, zLocation = 0;
int pointnum; //Point Number
int facenum; //Face Number
Point* mpoint = NULL;
Face* mface = NULL;
GLfloat angle = 0; /* in degrees */
int moving; //UI�� �ֱ� ���� ������
int trans;
int mousestart;
int light_moving;
float s_factor = 1.0; // �������� ��
int scale = 0;
int status = 0; //WIRE or SHADE or SMOOTH
int Cull = 0; //CULLING toggle 
string fname = "./mysphere.dat"; //�ε� �Ǵ� �⺻ ���� Sphere

void InitLight() { //Light
	GLfloat mat_diffuse[] = { 0.5, 0.4, 0.3, 1.0 }; //����
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 }; //����
	GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light_position[] = { 200, 200, -200.0, 0.0 };

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING); //�Ӽ� On
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

//Calculate Normal vector
Point cnormal(Point a, Point b, Point c) {
	Point p, q, r;
	double val;
	p.x = a.x - b.x; p.y = a.y - b.y; p.z = a.z - b.z;
	q.x = c.x - b.x; q.y = c.y - b.y; q.z = c.z - b.z;
	r.x = p.y * q.z - p.z * q.y;
	r.y = p.z * q.x - p.x * q.z;
	r.z = p.x * q.y - p.y * q.x;
	val = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
	r.x = r.x / val; r.y = r.y / val; r.z = r.z / val;
	return r;
}

void ReadModel() //mysphere.dat �д� �ڵ�
{
	FILE* f1; char s[81]; int i;

	//
//  = x, y, z���� ����Ǵ� vertex �迭
	//mface = 0, 1, 2���� ����Ǵ� vertex �迭
	if (mpoint != NULL) delete mpoint;
	if (mface != NULL) delete mface;
	if ((f1 = fopen(fname.c_str(), "rt")) == NULL) { printf("No file\n"); exit(0); }
	fscanf(f1, "%s", s);
	fscanf(f1, "%s", s);
	fscanf(f1, "%d", &pointnum);
	mpoint = new Point[pointnum];
	for (i = 0; i < pointnum; i++) {
		fscanf(f1, "%f", &mpoint[i].x); fscanf(f1, "%f", &mpoint[i].y); fscanf(f1, "%f", &mpoint[i].z);
	}
	fscanf(f1, "%s", s);
	fscanf(f1, "%s", s);
	fscanf(f1, "%d", &facenum);
	mface = new Face[facenum];
	for (i = 0; i < facenum; i++) {
		fscanf(f1, "%d", &mface[i].ip[0]); fscanf(f1, "%d", &mface[i].ip[1]); fscanf(f1, "%d", &mface[i].ip[2]);
	}
	fclose(f1);
}

void DrawWire(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //��� ���
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	if (Cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Line���� �׸���
	glCallList(1); //List �ҷ�����

	glColor3f(0.7, 0.7, 0.7);
	glPushMatrix();
	glTranslatef(0, -100, 0);
	glBegin(GL_QUADS); //�ٴ� �׸���
	glVertex3f(200, 0, 200); glVertex3f(200, 0, -200);
	glVertex3f(-200, 0, -200); glVertex3f(-200, 0, 200);
	glEnd();
	glPopMatrix();

	glutSwapBuffers();
}

void DrawShade(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //��� ���
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	if (Cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //�� ä���
	glCallList(1); //List �ҷ�����

	glColor3f(0.7, 0.7, 0.7); //�ٴ� ����
	glPushMatrix();
	glTranslatef(0, -100, 0);
	glBegin(GL_QUADS); //�ٴ� �׸���
	glVertex3f(200, 0, 200); glVertex3f(200, 0, -200);
	glVertex3f(-200, 0, -200); glVertex3f(-200, 0, 200);
	glEnd();
	glPopMatrix();

	glutSwapBuffers();
}

void MakeGL_Model(void) //�� �����
{
	int i;
	glShadeModel(GL_SMOOTH);

	if (glIsList(1)) glDeleteLists(1, 1);
	glNewList(1, GL_COMPILE);
	glPushMatrix();
	glRotatef(angle, 0.0, 1.0, 0.0); //���콺 ��Ŭ�� ȸ��
	glScalef(s_factor, s_factor, s_factor); //���콺 ��� Ŭ�� Ȯ�����
	glColor3f(0.3, 0.5, 0.7); //Sphere ����

	for (i = 0; i < facenum; i++) {
		Point norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]], mpoint[mface[i].ip[0]]);
		glBegin(GL_TRIANGLES);
		//x, y, z���� ���� �� ���� ��ǥ ����
		glNormal3f(norm.x, norm.y, norm.z);
		glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);
		glNormal3f(norm.x, norm.y, norm.z);
		glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);
		glNormal3f(norm.x, norm.y, norm.z);
		glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);
		glEnd();
	}
	glPopMatrix(); //�ﰢ�� �׸���
	glEndList();
}

void SmoothModel(void) //Smooth �� �����
{
	int i;
	glShadeModel(GL_SMOOTH);

	if (glIsList(1)) glDeleteLists(1, 1);
	glNewList(1, GL_COMPILE);
	glPushMatrix();
	glRotatef(angle, 0.0, 1.0, 0.0); //���콺 ��Ŭ�� ȸ��
	glScalef(s_factor, s_factor, s_factor); //���콺 ��� Ŭ�� Ȯ�����
	glColor3f(0.3, 0.5, 0.7); //Sphere ����
	glEnable(GL_NORMALIZE);

	for (i = 0; i < facenum; i++) {
		Point norm = (mpoint[mface[i].ip[0]]);
		glBegin(GL_TRIANGLES);

		//x, y, z���� ���� �� �� ��ǥ ����
		glNormal3f(norm.x, norm.y, norm.z);
		glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);
		norm = (mpoint[mface[i].ip[1]]);
		glNormal3f(norm.x, norm.y, norm.z);
		glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);
		norm = (mpoint[mface[i].ip[2]]);
		glNormal3f(norm.x, norm.y, norm.z);
		glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);
		glEnd();
	}
	glPopMatrix(); //�ﰢ�� �׸���
	glEndList();
}

void GLSetupRC(void)
{
	glEnable(GL_DEPTH_TEST);
	/* Setup the view and projection */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, // field of view in degree, ���� ����
		1.0, // aspect ratio
		1.0, // Z near
		2000.0); // Z far
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(400.0, 400.0, 400.0, // eye position
		0.0, 0.0, 0.0, // center is at (0,0,0)
		0.0, 1.0, 0.); // up is in positive Y direction
}

void display(void)
{
	glTranslatef(xLocation, yLocation, zLocation);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MakeGL_Model();

	if (status == 1)
		MakeGL_Model();
	if (status == 2)
		SmoothModel();
	if (status == 0)
		DrawWire();
	else
		DrawShade();
}

void keyboard(unsigned char key, int x, int y) //keyboard �̺�Ʈ
{
	switch (key)
	{
	case 'w': //���̾� ���
		status = 0;
		glutPostRedisplay();
		printf("WireFrame\n");
		break;
	case 'd': //���̵� ���
		status = 1;
		MakeGL_Model();
		glutPostRedisplay();
		printf("Flat Shading\n");
		break;
	case 's': //������ ���
		status = 2;
		SmoothModel();
		glutPostRedisplay();
		printf("Smooth Shading\n");
		break;

	case '1': //���� ����
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		xLocation = 0, yLocation = 0, zLocation = 0;
		glOrtho(-150, 150, -150, 150, -800, 800);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(400.0, 400.0, 400.0, // eye position
			0.0, 0.0, 0.0, // center is at (0,0,0)
			0.0, 1.0, 0.); // up is in positive Y direction
		glutPostRedisplay();
		printf("���� ����\n");
		break;

	case '2': //���� ����
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		xLocation = 0, yLocation = 0, zLocation = 0;
		gluPerspective(40.0, // field of view in degree
			1.0, // aspect ratio
			1.0, // Z near
			2000.0); // Z far
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(400.0, 400.0, 400.0, // eye position
			0.0, 0.0, 0.0, // center is at (0,0,0)
			0.0, 1.0, 0.); // up is in positive Y direction
		glutPostRedisplay();
		printf("���� ����\n");
		break;

	case '3': //����
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		xLocation = 0, yLocation = 0, zLocation = 0;
		gluPerspective(40.0, // field of view in degree
			1.0, // aspect ratio
			1.0, // Z near
			2000.0); // Z far
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(400.0, 0.0, 400.0, // eye position
			0.0, 0.0, 0.0, // center is at (0,0,0)
			0.0, 1.0, 0.); // up is in positive Y direction
		glutPostRedisplay();
		printf("���� ��ȯ(front)\n");
		break;

	}
}

void KeyboardMove(int key, int x, int y) //keyboard �̺�Ʈ
{
	switch (key)
	{
	case GLUT_KEY_LEFT: // ����Ű ����
		xLocation = xLocation - 10;
		zLocation = zLocation + 10;
		glutPostRedisplay();

		xLocation = xLocation;
		yLocation = yLocation;
		zLocation = zLocation;
		break;

	case GLUT_KEY_RIGHT: //����Ű ������
		xLocation = xLocation + 10;
		zLocation = zLocation - 10;
		glutPostRedisplay();

		xLocation = xLocation;
		yLocation = yLocation;
		zLocation = zLocation;
		break;

	case GLUT_KEY_UP: //����Ű ����
		zLocation = zLocation - 10;
		xLocation = xLocation - 10;
		glutPostRedisplay();

		xLocation = xLocation;
		yLocation = yLocation;
		zLocation = zLocation;
		break;

	case GLUT_KEY_DOWN: //����Ű �Ʒ���
		zLocation = zLocation + 10;
		xLocation = xLocation + 10;
		glutPostRedisplay();

		xLocation = xLocation;
		yLocation = yLocation;
		zLocation = zLocation;
		break;
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//���콺 ��Ŭ�� ��������
		moving = 1;
		mousestart = x;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		//���콺 ��Ŭ�� ������
		moving = 0;
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		//���콺 ��� ��������
		scale = 1;
		mousestart = x;
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
		//���콺 ��� ������
		scale = 0;
	}
}

void motion(int x, int y) { //���콺 �����̴� �翡 ���� ũ��, ���� ����
	if (scale) {
		s_factor = s_factor * (1.0 + (mousestart - x) * 0.0001);
		glutPostRedisplay();
	}
	if (moving) {
		angle = angle + (x - mousestart); mousestart = x;
		glutPostRedisplay();
	}
}

void Myhelp() //Mouse/Key Button Usage
{
	printf("--------------------------------------------------\n");
	printf("Mouse Left Button :  Rotate\n");
	printf("Mouse MIDDLE Button : Scale\n");
	printf("Direction Key(��,��,��,��) : Move\n");
	printf("w >>> WireFrame \n");
	printf("d >>> Flat Shading \n");
	printf("s >>> Smooth Shading \n");
	printf("1 >>> ���� ���� \n");
	printf("2 >>> ���� ���� \n");
	printf("3 >>> ���� ��ȯ(front)\n");
	printf("--------------------------------------------------\n");
}

int main(int argc, char** argv) //Main�Լ�
{
	glutInit(&argc, argv); glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Show 3D Model");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(KeyboardMove);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	ReadModel();
	GLSetupRC();
	Myhelp();

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	InitLight();

	glutMainLoop();
	return 0;
}
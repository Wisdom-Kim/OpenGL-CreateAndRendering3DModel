#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <vector>
#include <iostream>

using namespace std;
#define WIRE 0
#define SHADE 1

typedef struct {
    float x;
    float y;
    float z;
} Point;

typedef struct
{
    unsigned int ip[3];
} 
Face; int pnum; int fnum; Point* mpoint = NULL; Face* mface = NULL;
GLboolean SOR = true; //�Լ� ���� �ʱ⿡�� SOR�� �����ϵ��� �Ѵ�.
GLsizei winWidth = 500, winHeight = 500; // Initial display-window size.
GLboolean isRotate = false;
//ȸ����ų ���ΰ�

class xPoint3D
{
public:
    float x, y, z, w; // x, y, z ��
    xPoint3D() { x = y = z = 0; w = 1; };
};
//SOR�� �ϱ� ���� �ڷᱸ��
vector <xPoint3D> arInputPoints; // ���콺�� �Էµ� ������ ������ �ڷᱸ��
vector <xPoint3D> arRotPoints; // ��ǥ�� ���� ȸ���� ������ ������ �ڷᱸ��

//�𵨸��� �ϱ� ���� ����
GLfloat vx, vy, vz = 0;
GLfloat cx, cz = 0.0;
GLfloat cy = 1.0;
GLfloat angle_x = 0;
GLfloat angle_y = 0;/* in degrees */
int moving; int mousebegin; int light_moving; float scalefactor = 1.0; int scaling = 0; int status = 0;
string fname = ".\\mymodel.dat";


static xPoint3D pt;

int fRotAngle = 30; // ȸ�� ���� (�⺻�� = 30��)
int n_of_rotation = 360 / fRotAngle; //ȸ���ϴ� Ƚ�� (360/ȸ����)
float radian = fRotAngle * (M_PI / 180); // ���� ��ȯ
/* Move cursor while pressing c key enables freehand curve drawing. */

//������
void InitLight() {
    GLfloat mat_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat mat_shininess[] = { 15.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat light_position[] = { 0, 6, 3.0, 0.0 };
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
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

//�������� ���ϱ� ����
Point cnormal(Point a, Point b, Point c) {
    Point p, q, r;
    double val;
    p.x = a.x - b.x; p.y = a.y - b.y; p.z = a.z - b.z; //p���� (BA����)
    q.x = c.x - b.x; q.y = c.y - b.y; q.z = c.z - b.z; //q���� (BC����)
    r.x = p.y * q.z - p.z * q.y; //�������� x��
    r.y = p.z * q.x - p.x * q.z; //���� y��
    r.z = p.x * q.y - p.y * q.x; //���� z��
    val = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
    r.x = r.x / val; r.y = r.y / val; r.z = r.z / val; //���� ����ȭ
    return r;
}

//mymodel�� �޽��� �������� �����ϱ� ���� ���ο� �迭�� dat���Ϸκ��� ���� �Է¹���
void ReadModel() {
    FILE* f1;      char s[81];         int i;
    if (mpoint != NULL) delete mpoint;
    if (mface != NULL) delete mface;
    if ((f1 = fopen(fname.c_str(), "rt")) == NULL)
    {
        printf("No file\n"); exit(0);
    }
    fscanf(f1, "%s", s);
    printf("%s", s);
    fscanf(f1, "%s", s);
    printf("%s", s);
    fscanf(f1, "%d", &pnum);
    printf("%d\n", pnum);
    mpoint = new Point[pnum];
    for (i = 0; i < pnum; i++)
    {
        fscanf(f1, "%f", &mpoint[i].x);
        fscanf(f1, "%f", &mpoint[i].y);
        fscanf(f1, "%f", &mpoint[i].z);
        printf("%f %f %f\n", mpoint[i].x, mpoint[i].y, mpoint[i].z);
    }
    fscanf(f1, "%s", s);
    printf("%s", s);
    fscanf(f1, "%s", s);
    printf("%s", s);
    fscanf(f1, "%d", &fnum);
    printf("%d\n", fnum); mface = new Face[fnum];
    for (i = 0; i < fnum; i++)
    {
        fscanf(f1, "%d", &mface[i].ip[0]);
        fscanf(f1, "%d", &mface[i].ip[1]);
        fscanf(f1, "%d", &mface[i].ip[2]);
        printf("%d %d %d\n", mface[i].ip[0], mface[i].ip[1], mface[i].ip[2]);
    }
    fclose(f1);
}

//�Է¹��� �迭�� ���̾� ����
void DrawWire(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glCallList(1);
    glutSwapBuffers();
}
//�Է¹��� �迭�� ������ ����
void DrawShade(void)
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glCallList(1);
    glutSwapBuffers();
}
//�Է¹��� �迭�� ���� �� �׸���
void MakeGL_Model(void) //myModel ������Ʈ �����
{
    int i;
    if (glIsList(1)) glDeleteLists(1, 1);
    glNewList(1, GL_COMPILE);
    glPushMatrix();
    glRotatef(angle_x, 0, 1.0, 0.0);
    glScalef(scalefactor, scalefactor, scalefactor);
    glColor3f(0.1, 1, 0);
    for (i = 0; i < fnum; i++) {
        Point norm = cnormal(mpoint[mface[i].ip[1]], mpoint[mface[i].ip[0]], mpoint[mface[i].ip[2]]);
        glBegin(GL_TRIANGLES);
        glNormal3f(norm.x, norm.y, norm.z);
        glColor3f(0.1, 1, 0);
        glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);

        glNormal3f(norm.x, norm.y, norm.z);
        glColor3f(0.1, 1, 0.4);
        glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);

        glNormal3f(norm.x, norm.y, norm.z);
        glColor3f(0.4, 1, 0);
        glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);

        glEnd();
    }
    glPopMatrix();
    glEndList();
}

void GLSetupRC(void)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    /* Setup the view and projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, // field of view in degree 
        1.0, // aspect ratio 
        1.0, // Z near 
        2000.0); // Z far 
}

//�ʱ� x�� y�� ǥ�ø� ����
void line()
{
    glColor3f(1.0, 0, 0); glBegin(GL_LINE_LOOP); glVertex3f(-250, 0, 0); glVertex3f(250, 0, 0); glEnd();
    glColor3f(0.0, 1.0, 0); glBegin(GL_LINE_LOOP); glVertex3f(0, -250, 0); glVertex3f(0, 250, 0); glEnd();
    glFlush();
}

void init(void)
{
    arInputPoints.clear();
    arRotPoints.clear();
    glClearColor(0.9, 0.9, 0.9, 1.0); // Set display-window color to blue.
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glOrtho(-winWidth / 2, winWidth / 2, -winHeight / 2, winHeight / 2, -100, 100);
    line();//��ǥ�� ǥ�ø� ����
}

void drawCircle(xPoint3D pt) {
    glColor3f(0.1, 0.1, 0.1);
    glPointSize(4.0);
    glBegin(GL_POINTS);
    glVertex3f(pt.x, pt.y, pt.z);
    glEnd();
}

void func1() // ȸ���� ���� ������, ȭ���� ���ΰ�ħ�Ѵ�.
{
    if (!arRotPoints.empty())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glutPostRedisplay();
    }
    printf("Rotation Angle is %d\n", fRotAngle);
    printf("Rotation Number is %d\n\n", 360 / fRotAngle);
}
void rotatey() {
    cout << "�ش� ������ ȸ���� ������ �Է��ϼ��� :";
    cin >> fRotAngle;
    arRotPoints.clear();
    for (int i = 0; i < 360 / fRotAngle; i++) {
        radian = (i + 1) * fRotAngle * (M_PI / 180);
        for (int i = 0; i < arInputPoints.size(); i++) {
            xPoint3D newpt; // ���� �ڷᱸ�� ����
            newpt.x = arInputPoints[i].x * cos(radian) + arInputPoints[i].z * sin(radian);
            newpt.y = arInputPoints[i].y;
            newpt.z = arInputPoints[i].x * sin(radian) - arInputPoints[i].z * cos(radian);
            drawCircle(newpt);
            arRotPoints.push_back(newpt); // ���� ���� ����(��)�� �߰�(����)
        }
    }
    cout << "ȸ�� ��Ų ���� ����:" << arRotPoints.size();
}

void rotatex() {
    cout << "�ش� ������ ȸ���� ������ �Է��ϼ��� :";
    cin >> fRotAngle;
    arRotPoints.clear();
    for (int i = 0; i < 360 / fRotAngle; i++) {
        radian = (i + 1) * fRotAngle * (M_PI / 180);
        for (int i = 0; i < arInputPoints.size(); i++) {
            xPoint3D newpt;
            newpt.x = arInputPoints[i].x;
            newpt.y = arInputPoints[i].y * cos(radian) - arInputPoints[i].z * sin(radian);
            newpt.z = arInputPoints[i].y * sin(radian) + arInputPoints[i].z * cos(radian); drawCircle(newpt);
            drawCircle(newpt);
            arRotPoints.push_back(newpt);
        }
    }
    cout << "ȸ�� ��Ų ���� ����:" << arRotPoints.size();
}

void rotatez() {
    cout << "�ش� ������ ȸ���� ������ �Է��ϼ��� :";
    cin >> fRotAngle;
    arRotPoints.clear();
    for (int i = 0; i < 360 / fRotAngle; i++) {
        radian = (i + 1) * fRotAngle * (M_PI / 180);
        for (int i = 0; i < arInputPoints.size(); i++) {
            xPoint3D newpt;
            newpt.x = arInputPoints[i].x * cos(radian) - arInputPoints[i].y * sin(radian);
            newpt.y = arInputPoints[i].x * sin(radian) + arInputPoints[i].y * cos(radian);
            newpt.z = arInputPoints[i].z; drawCircle(newpt);
            arRotPoints.push_back(newpt);
        }
    }
    cout << "ȸ�� ��Ų ���� ����:" << arRotPoints.size();
}


void drawMeshes(vector <xPoint3D> pts)
{   
    int n = 360 / fRotAngle; //ȸ����Ų Ƚ��
    int s = arInputPoints.size(); //ó���� ���� ���� ����
    int m = arRotPoints.size(); //ȸ���� ���� ����
    glColor3f(0, 0, 0); //���������� �׸���, �β��� 1�� �����Ѵ�.
    glLineWidth(1.0);
    if ((!s == 0) && (!m == 0)) //������ ȸ�������� ���
    {
        // ����
        for (int k = 0; k < s; k++) {
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < n; i++) {
                glVertex3f(pts[i * s + k].x, pts[i * s + k].y, pts[i * s + k].z);
            }
            glEnd();
        }
        // ������
        for (int i = 0; i < n; i++) {
            glBegin(GL_LINE_STRIP);
            for (int k = 0; k < s; k++) {
                glVertex3f(pts[i * s + k].x, pts[i * s + k].y, pts[i * s + k].z);
            }
            glEnd();
        }
        // �밢��
        for (int i = 0; i < n - 1; i++) {
            for (int k = 0; k < s - 1; k++) {
                glBegin(GL_LINES);
                glVertex3f(pts[i * s + k].x, pts[i * s + k].y, pts[i * s + k].z);
                glVertex3f(pts[(i + 1) * s + k + 1].x, pts[(i + 1) * s + k + 1].y, pts[(i + 1) * s + k + 1].z); //���� �� -> ������ �Ʒ�
                glEnd();
                glBegin(GL_LINES);
                glVertex3f(pts[i * s + k + 1].x, pts[i * s + k + 1].y, pts[i * s + k + 1].z);
                glVertex3f(pts[(i + 1) * s + k].x, pts[(i + 1) * s + k].y, pts[(i + 1) * s + k].z); //������ �� -> ���� �Ʒ�
                glEnd();
            }
        }

    }
}


void SaveModel(vector <xPoint3D> pts )
{
    int n = 360 / fRotAngle; //ȸ����Ų Ƚ��
    int s = arInputPoints.size(); //ó���� ���� ���� ��
    int pnum = arInputPoints.size() + arRotPoints.size(); //������ �� ����
    int m = arRotPoints.size(); //ȸ���� ���� ����
    
//    char directory= "C:\\Users\\wisdom99\\Downloads\\Computer Graphics\\mymodel.dat";

    FILE* fout;
    fout = fopen(".\\mymodel.dat", "w");

    fprintf(fout, "VERTEX = %d\n", pts.size());
    for (int i = 0; i < m; i++)
    {
        fprintf(fout, "%.1f %.1f %1f\n", pts[i].x*50, pts[i].y*50, pts[i].z*50); //���� ��ǥ ������ �ʹ� �۾Ƽ�, ���Ƿ� Ű����
    }

    //face���
    fprintf(fout, "FACE = %d\n", (2 * n * s) - 4);
    for (int i = 0; i < n-1; i++) {
        for (int k = 0; k < s - 1; k++)
        {
            fprintf(fout, "%d %d %d\n", i * s + k + 1, i * s + k, (i + 1) * s + k + 1);
            fprintf(fout, "%d %d %d\n", (i + 1) * s + k, (i + 1) * s + k + 1, i * s + k);
        }
    }
    for (int k = 0; k < s - 1; k++)
    {
        fprintf(fout, "%d %d %d\n", m - s + k +1, m - s + k, k+1);
        fprintf(fout, "%d %d %d\n", k, k + 1 ,m - s + k);
    }
    
    for (int k = 0; k < n - 2; k++)
    {
        fprintf(fout, "%d %d %d\n", 0, (k + 2) * s,(k + 1) * s);
        fprintf(fout, "%d %d %d\n", s - 1, (k + 3) * s - 1, (k + 2) * s - 1);
    }
    fclose(fout);

}

void winReshapeFcn(int newWidth, int newHeight) {
    glViewport(0, 0, newWidth, newHeight); /* Reset viewport and projection
    parameters */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, GLdouble(newWidth), 0.0, GLdouble(newHeight));
    winWidth = newWidth; /* Reset display-window size parameters. */
    winHeight = newHeight;
}
//SOR���� display�Լ�
void displayFcn(void) { // Clear display window.
    glColor3f(1.0, 0.0, 0.0); // Set point color to red.
    glPointSize(3.0); // Set point size to 3.0.
    glRotatef(0.0, 0.0, 1.0, 0.0);
    glLoadIdentity();
    glFlush();
}
//ShowModel���� display�Լ�
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFrontFace(GL_CCW); glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH); //Ű���带 ���� ���� �����ϴ� 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(400.0, 400.0, 400.0, // eye poisition
        vx, vy, vz, // center is at (0,0,0) 
        cx, cy, cz); // up is in positive Y direction 

    MakeGL_Model();

    if (status == WIRE)
        DrawWire();
    else
        DrawShade();
}
//ShowModel���� ���콺�� Ű���� �Լ�
void keyboard(unsigned char key, int x, int y)
{
    printf("key %d\n", key);
    switch (key)
    {
    case 'w':
        status = WIRE; glutPostRedisplay(); break;
    case 's':
        status = SHADE; glShadeModel(GL_SMOOTH); glutPostRedisplay(); break;
    case 'f':
        status = SHADE; glShadeModel(GL_FLAT); glutPostRedisplay(); break;
    case '4': //2�� ������FRONT FACE
        glCullFace(GL_FRONT); glutPostRedisplay(); break;
    case '6': //4�� ������ BACKFACE
        glCullFace(GL_BACK); glutPostRedisplay(); break;
        //��ǥ�� ��ȯ
    case '7': //7�� ������ vx+=0.01
        vx += 1; glutPostRedisplay(); break;
    case '9': //9�� ������ vx-=0.01
        vx -= 1; glutPostRedisplay(); break;
    case '1': //1�� ������ vy+=0.01
        vy += 1; glutPostRedisplay(); break;
    case'3': //3�� ������ vy-=0.01
        vy -= 1; glutPostRedisplay(); break;
    case'2': //2�� ������ BACKFACE
        vz += 1; glutPostRedisplay(); break;
    case'8': //8�� ������ BACKFACE
        vz -= 1; glutPostRedisplay(); break;
        //ī�޶� ��ġ ��ȯ
    case 'q': //q�� ������ cx+=0.01
        cx += 1; glutPostRedisplay(); break;
    case 'e': //e�� ������ cx-=0.01
        cx -= 1; glutPostRedisplay(); break;
    case 'a': //a�� ������ cy+=0.01
        cy += 1; glutPostRedisplay(); break;
    case'd': //d�� ������ cy-=0.01
        cy -= 1; glutPostRedisplay(); break;
    case'z': //z�� ������ BACKFACE
        cz += 1; glutPostRedisplay(); break;
    case'c': //c�� ������ BACKFACE
        cz -= 1; glutPostRedisplay(); break;
    case 32 : //�����̽��ٸ� ������
        SOR = !SOR;
    }
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        moving = 1;
        mousebegin = x;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        moving = 0;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        scaling = 1;
        mousebegin = x;
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        scaling = 0;
    }
}


//SOR���� ���콺 �Լ�
void MyMouse(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
    int w = winWidth;
    int h = winHeight;
    GLfloat vx = (float)(xMouse - (float)w / 2.0) * (float)(1.0 / (float)(w / 2.0)); // ���콺�� ��ǥ�� ����
    GLfloat vy = -(float)(yMouse - (float)h / 2.0) * (float)(1.0 / (float)(h / 2.0));

    if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN) // ���� ���콺 Ŭ�� ��
    {
        pt.x = vx;
        pt.y = vy;
        pt.z = 0;
        drawCircle(pt); // �� �׸���
        arInputPoints.push_back(pt); // ���� ���� ����(��)�� �߰�(����)
        glutPostRedisplay();
        cout << "���콺 x��ǥ" << vx * 100 << "   " << "���콺 y��ǥ" << vy * 100 << endl;
    }
    glFlush();
}


//���� Ű���� �Լ�
void MyKeyboard(unsigned char key, int x, int y)
{
    printf("key %d\n", key);
    switch (key)
    {
    case 32: //�����̽��� ������ SOR��� ����
        SOR = !SOR;
    }
}

void motion(int x, int y) {
    if (scaling) {
        scalefactor = scalefactor * (1.0 + (mousebegin - x) * 0.0001);
        glutPostRedisplay();
    }
    if (moving) {
        angle_x = angle_x + (x - mousebegin);
        angle_y = angle_y + (y - mousebegin); mousebegin = y;
        glutPostRedisplay();
    }
}

//SOR�� �޴� ����� ��

void MyMainMenu(int entryID) {
    if (entryID == 1) init();//Clear Canvas
    else if (entryID == 2) drawMeshes(arRotPoints);
    else if (entryID == 3) SaveModel(arRotPoints);
    glutPostRedisplay();
}
void RotateMenu(int entryID) {
    if (entryID == 1)  rotatex();
    else if (entryID == 2)  rotatey();
    else if (entryID == 3)  rotatez();
}

//��� ������ ���� �ʱ� �޴� �Լ�

void SOR_main()
{
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("20204656 ������");
    init();
    glutDisplayFunc(displayFcn);
    glutKeyboardFunc(MyKeyboard);
    glutMouseFunc(MyMouse);
    glutReshapeFunc(winReshapeFcn);

    GLint RotateMenuID = glutCreateMenu(RotateMenu);
    glutAddMenuEntry("X-axis", 1);
    glutAddMenuEntry("Y-axis", 2);
    glutAddMenuEntry("Z-axis", 3);

    GLint MyMainMenuID = glutCreateMenu(MyMainMenu);//���θ޴� �����
    glutAddSubMenu("Rotating Points", RotateMenuID);
    glutAddMenuEntry("Clear Canvas", 1);
    glutAddMenuEntry("Generate Mesh", 2);
    glutAddMenuEntry("Save Model", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//showMOdel������ main�Լ�
void showModel_main()
{   
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("20204656 ������");
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    ReadModel();
    GLSetupRC();
    InitLight();

}

void main2(int argc, char** argv)
{
       glutInit(&argc, argv);
       showModel_main();
       glutMainLoop();
    
}
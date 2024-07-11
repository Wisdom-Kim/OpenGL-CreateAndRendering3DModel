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
GLboolean SOR = true; //함수 실행 초기에는 SOR을 실행하도록 한다.
GLsizei winWidth = 500, winHeight = 500; // Initial display-window size.
GLboolean isRotate = false;
//회전시킬 것인가

class xPoint3D
{
public:
    float x, y, z, w; // x, y, z 값
    xPoint3D() { x = y = z = 0; w = 1; };
};
//SOR을 하기 위한 자료구조
vector <xPoint3D> arInputPoints; // 마우스로 입력된 점들을 저장할 자료구조
vector <xPoint3D> arRotPoints; // 좌표축 기준 회전한 점들을 저장할 자료구조

//모델링을 하기 위한 변수
GLfloat vx, vy, vz = 0;
GLfloat cx, cz = 0.0;
GLfloat cy = 1.0;
GLfloat angle_x = 0;
GLfloat angle_y = 0;/* in degrees */
int moving; int mousebegin; int light_moving; float scalefactor = 1.0; int scaling = 0; int status = 0;
string fname = ".\\mymodel.dat";


static xPoint3D pt;

int fRotAngle = 30; // 회전 각도 (기본값 = 30도)
int n_of_rotation = 360 / fRotAngle; //회전하는 횟수 (360/회전각)
float radian = fRotAngle * (M_PI / 180); // 단위 변환
/* Move cursor while pressing c key enables freehand curve drawing. */

//조명설정
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

//외적값을 구하기 위함
Point cnormal(Point a, Point b, Point c) {
    Point p, q, r;
    double val;
    p.x = a.x - b.x; p.y = a.y - b.y; p.z = a.z - b.z; //p벡터 (BA벡터)
    q.x = c.x - b.x; q.y = c.y - b.y; q.z = c.z - b.z; //q벡터 (BC벡터)
    r.x = p.y * q.z - p.z * q.y; //법선벡터 x값
    r.y = p.z * q.x - p.x * q.z; //법벡 y값
    r.z = p.x * q.y - p.y * q.x; //법벡 z값
    val = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
    r.x = r.x / val; r.y = r.y / val; r.z = r.z / val; //법벡 정규화
    return r;
}

//mymodel의 메쉬와 폴리곤을 생성하기 위해 새로운 배열에 dat파일로부터 값을 입력받음
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

//입력받은 배열로 와이어 생성
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
//입력받은 배열로 폴리곤 생성
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
//입력받은 배열로 실제 모델 그리기
void MakeGL_Model(void) //myModel 오브젝트 만들기
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

//초기 x축 y축 표시를 위함
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
    line();//좌표계 표시를 위함
}

void drawCircle(xPoint3D pt) {
    glColor3f(0.1, 0.1, 0.1);
    glPointSize(4.0);
    glBegin(GL_POINTS);
    glVertex3f(pt.x, pt.y, pt.z);
    glEnd();
}

void func1() // 회전한 점이 있으면, 화면을 새로고침한다.
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
    cout << "해당 축으로 회전할 각도를 입력하세요 :";
    cin >> fRotAngle;
    arRotPoints.clear();
    for (int i = 0; i < 360 / fRotAngle; i++) {
        radian = (i + 1) * fRotAngle * (M_PI / 180);
        for (int i = 0; i < arInputPoints.size(); i++) {
            xPoint3D newpt; // 점의 자료구조 선언
            newpt.x = arInputPoints[i].x * cos(radian) + arInputPoints[i].z * sin(radian);
            newpt.y = arInputPoints[i].y;
            newpt.z = arInputPoints[i].x * sin(radian) - arInputPoints[i].z * cos(radian);
            drawCircle(newpt);
            arRotPoints.push_back(newpt); // 벡터 끝에 원소(점)를 추가(저장)
        }
    }
    cout << "회전 시킨 점의 개수:" << arRotPoints.size();
}

void rotatex() {
    cout << "해당 축으로 회전할 각도를 입력하세요 :";
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
    cout << "회전 시킨 점의 개수:" << arRotPoints.size();
}

void rotatez() {
    cout << "해당 축으로 회전할 각도를 입력하세요 :";
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
    cout << "회전 시킨 점의 개수:" << arRotPoints.size();
}


void drawMeshes(vector <xPoint3D> pts)
{   
    int n = 360 / fRotAngle; //회전시킨 횟수
    int s = arInputPoints.size(); //처음에 찍은 점들 개수
    int m = arRotPoints.size(); //회전된 점들 개수
    glColor3f(0, 0, 0); //검은색으로 그리며, 두께는 1로 설정한다.
    glLineWidth(1.0);
    if ((!s == 0) && (!m == 0)) //점들을 회전시켰을 경우
    {
        // 수평선
        for (int k = 0; k < s; k++) {
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < n; i++) {
                glVertex3f(pts[i * s + k].x, pts[i * s + k].y, pts[i * s + k].z);
            }
            glEnd();
        }
        // 수직선
        for (int i = 0; i < n; i++) {
            glBegin(GL_LINE_STRIP);
            for (int k = 0; k < s; k++) {
                glVertex3f(pts[i * s + k].x, pts[i * s + k].y, pts[i * s + k].z);
            }
            glEnd();
        }
        // 대각선
        for (int i = 0; i < n - 1; i++) {
            for (int k = 0; k < s - 1; k++) {
                glBegin(GL_LINES);
                glVertex3f(pts[i * s + k].x, pts[i * s + k].y, pts[i * s + k].z);
                glVertex3f(pts[(i + 1) * s + k + 1].x, pts[(i + 1) * s + k + 1].y, pts[(i + 1) * s + k + 1].z); //왼쪽 위 -> 오른쪽 아래
                glEnd();
                glBegin(GL_LINES);
                glVertex3f(pts[i * s + k + 1].x, pts[i * s + k + 1].y, pts[i * s + k + 1].z);
                glVertex3f(pts[(i + 1) * s + k].x, pts[(i + 1) * s + k].y, pts[(i + 1) * s + k].z); //오른쪽 위 -> 왼쪽 아래
                glEnd();
            }
        }

    }
}


void SaveModel(vector <xPoint3D> pts )
{
    int n = 360 / fRotAngle; //회전시킨 횟수
    int s = arInputPoints.size(); //처음에 찍은 점들 개
    int pnum = arInputPoints.size() + arRotPoints.size(); //점들의 총 개수
    int m = arRotPoints.size(); //회전된 점들 개수
    
//    char directory= "C:\\Users\\wisdom99\\Downloads\\Computer Graphics\\mymodel.dat";

    FILE* fout;
    fout = fopen(".\\mymodel.dat", "w");

    fprintf(fout, "VERTEX = %d\n", pts.size());
    for (int i = 0; i < m; i++)
    {
        fprintf(fout, "%.1f %.1f %1f\n", pts[i].x*50, pts[i].y*50, pts[i].z*50); //원본 좌표 지점이 너무 작아서, 임의로 키웠음
    }

    //face기록
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
//SOR에서 display함수
void displayFcn(void) { // Clear display window.
    glColor3f(1.0, 0.0, 0.0); // Set point color to red.
    glPointSize(3.0); // Set point size to 3.0.
    glRotatef(0.0, 0.0, 1.0, 0.0);
    glLoadIdentity();
    glFlush();
}
//ShowModel에서 display함수
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFrontFace(GL_CCW); glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH); //키보드를 통해 변경 가능하다 
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
//ShowModel에서 마우스와 키보드 함수
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
    case '4': //2을 누르면FRONT FACE
        glCullFace(GL_FRONT); glutPostRedisplay(); break;
    case '6': //4을 누르면 BACKFACE
        glCullFace(GL_BACK); glutPostRedisplay(); break;
        //목표점 변환
    case '7': //7을 누르면 vx+=0.01
        vx += 1; glutPostRedisplay(); break;
    case '9': //9을 누르면 vx-=0.01
        vx -= 1; glutPostRedisplay(); break;
    case '1': //1을 누르면 vy+=0.01
        vy += 1; glutPostRedisplay(); break;
    case'3': //3을 누르면 vy-=0.01
        vy -= 1; glutPostRedisplay(); break;
    case'2': //2을 누르면 BACKFACE
        vz += 1; glutPostRedisplay(); break;
    case'8': //8을 누르면 BACKFACE
        vz -= 1; glutPostRedisplay(); break;
        //카메라 위치 변환
    case 'q': //q을 누르면 cx+=0.01
        cx += 1; glutPostRedisplay(); break;
    case 'e': //e을 누르면 cx-=0.01
        cx -= 1; glutPostRedisplay(); break;
    case 'a': //a을 누르면 cy+=0.01
        cy += 1; glutPostRedisplay(); break;
    case'd': //d을 누르면 cy-=0.01
        cy -= 1; glutPostRedisplay(); break;
    case'z': //z을 누르면 BACKFACE
        cz += 1; glutPostRedisplay(); break;
    case'c': //c을 누르면 BACKFACE
        cz -= 1; glutPostRedisplay(); break;
    case 32 : //스페이스바를 누르면
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


//SOR에서 마우스 함수
void MyMouse(GLint button, GLint action, GLint xMouse, GLint yMouse)
{
    int w = winWidth;
    int h = winHeight;
    GLfloat vx = (float)(xMouse - (float)w / 2.0) * (float)(1.0 / (float)(w / 2.0)); // 마우스의 좌표계 수정
    GLfloat vy = -(float)(yMouse - (float)h / 2.0) * (float)(1.0 / (float)(h / 2.0));

    if (button == GLUT_LEFT_BUTTON && action == GLUT_DOWN) // 왼쪽 마우스 클릭 시
    {
        pt.x = vx;
        pt.y = vy;
        pt.z = 0;
        drawCircle(pt); // 점 그리기
        arInputPoints.push_back(pt); // 벡터 끝에 원소(점)를 추가(저장)
        glutPostRedisplay();
        cout << "마우스 x좌표" << vx * 100 << "   " << "마우스 y좌표" << vy * 100 << endl;
    }
    glFlush();
}


//메인 키보드 함수
void MyKeyboard(unsigned char key, int x, int y)
{
    printf("key %d\n", key);
    switch (key)
    {
    case 32: //스페이스바 누르면 SOR모드 변경
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

//SOR의 메뉴 만드는 곳

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

//모드 진입을 위한 초기 메뉴 함수

void SOR_main()
{
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("20204656 김지혜");
    init();
    glutDisplayFunc(displayFcn);
    glutKeyboardFunc(MyKeyboard);
    glutMouseFunc(MyMouse);
    glutReshapeFunc(winReshapeFcn);

    GLint RotateMenuID = glutCreateMenu(RotateMenu);
    glutAddMenuEntry("X-axis", 1);
    glutAddMenuEntry("Y-axis", 2);
    glutAddMenuEntry("Z-axis", 3);

    GLint MyMainMenuID = glutCreateMenu(MyMainMenu);//메인메뉴 만들기
    glutAddSubMenu("Rotating Points", RotateMenuID);
    glutAddMenuEntry("Clear Canvas", 1);
    glutAddMenuEntry("Generate Mesh", 2);
    glutAddMenuEntry("Save Model", 3);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//showMOdel에서의 main함수
void showModel_main()
{   
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("20204656 김지혜");
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
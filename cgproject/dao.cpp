#include <Windows.h>
#include <iostream>
#include <GL/glut.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // 이미지 로딩 라이브러리

#define PI 3.141592
#define ZOOM_IN_LIMIT 2
#define ZOOM_SPEED 0.3
#define START_ZOOM 12

// WIDTH, HEIGHT: 초기 윈도우 창 크기
#define WIDTH 400
#define HEIGHT 400

// 마우스로 화면 움직이는 감도
// 0.01: 아주 느리게 ~ 0.1: 아주 빠르게
#define MOVE 0.04

int beforeX, beforeY;  // 마우스 조작시 사용
float vTheta, hTheta;  // gluLookat에서의 카메라 위치를 계산할 때 사용
float radius;  // 카메라가 위치할 수 있는 구의 반지름을 설정, zoom in/out 에도 사용. (구의 반지름이 줄면 줌 인, 늘면 줌 아웃)
float x, y, z;  // gluLookat 에서의 카메라 위치 (eyeX, eyeY, eyeZ)

void InitLight();
void MyReshape(int, int);
void MyKeyboard(unsigned char, int, int);
void MyMouseClick(int, int, int, int);
void MyMouseMove(GLint, GLint);
void MyDisplay();

void createCylinder(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void createRing(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void createCircle(GLfloat, GLfloat, GLfloat);
void createSphere(GLfloat);
void createHalfSphere(GLfloat, GLfloat, GLfloat, GLfloat);

void drawModel(); // 전체 모델 그리는 함수

void draw_head(); // 머리 그리는 함수
void draw_leftEar();         // 왼쪽 귀마개
void draw_rightEar();        // 오른쪽 귀마개
void draw_leftEye();         // 왼쪽눈
void draw_leftEyePupil();    // 왼쪽 흰자
void draw_rightEye();        // 오른쪽 눈
void draw_rightEyePupil();   // 오른쪽 흰자
void draw_leftFace();        // 왼쪽 볼
void draw_rightFace();       // 오른쪽 볼
void draw_leftEyeBrow();     // 왼쪽 눈썹
void draw_rightEyeBrow();    // 오른쪽 눈썹
void draw_middle();          // 몸통
void draw_rightArm();        // 오른쪽 팔
void draw_leftArm();         // 왼쪽 팔
void draw_rightWrist();      // 오른쪽 손목
void draw_leftWrist();       // 왼쪽 손목
void draw_rightHand();       // 오른쪽 손
void draw_leftHand();        // 왼쪽 손
void draw_rightLeg();        // 오른쪽 다리
void draw_leftLeg();         // 왼쪽 다리
void draw_righttSock();      // 오른쪽 양말
void draw_leftSock();        // 왼쪽 양말
void draw_rightShoe();       // 오른쪽 신발
void draw_leftShoe();        // 왼쪽 신발


void InitLight() {
    printf("InitLight\n");

    glClearColor(0.5, 0.5, 0.5, 0.0); // 배경색 지정 (R, G, B, A)

    // 경계에서 색이 부드럽게 이어지도록 한다.
    glShadeModel(GL_SMOOTH);

    // Depth가 작은 쪽은 뒤로 밀리고, Depth가 큰 쪽은 앞으로 나오도록 설정
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);  // 빛을 사용하겠다 선언
    glEnable(GL_LIGHT0);  // LIGHT 0 ~ 7 까지 총 8개의 light를 사용 가능. 우리는 light 0으로 1개의 빛만 사용
    glEnable(GL_COLOR_MATERIAL);  // 물체의 원래 빛이 나오도록 설정 (이게 없다면 빛의 색깔로만 물체가 보임)

    // 조명의 분산광, 반사광, 주변광을 설정
    GLfloat light_specular[] = { .5, .5, .5, 0.5 };
    GLfloat light_diffuse[] = { .5, .5, .5, 0.5 };
    GLfloat light_ambient[] = { .7, .7, .7, 0.5 };
    GLfloat light_position[] = { -3, 6 ,3.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // 빛 위치 지정
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);  // 분산광 (물체에 반사되어 마구 퍼져나가는 색상)
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);  // 반사광 (물체에 반사되어 입사각, 반사각의 원리에 의해 튀어나가는 색상)
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);  // 주변광 (우리 주위에 은은하게 존재하는 빛)

    // 물체의 분산광, 반사광, 주변광을 설정
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 15.0 };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  // 분산광
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);  // 반사광
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);  // 주변광
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);  // 물체가 반짝이는 정도를 조절
}

void MyReshape(int w, int h) {
    printf("MyReshape\n");

    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(50, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    radius = START_ZOOM;
    hTheta = 0.0;
    vTheta = 0.0;
}

// 키보드
void MyKeyboard(unsigned char key, int x, int y) {
    printf("MyKeyboard\n");
    switch (key) {
        //case 'q': case 'Q':
        //   exit(0);
        //   break;
    case 'a':
        if (radius > ZOOM_IN_LIMIT) {
            radius -= ZOOM_SPEED;
        }
        break;
    case 'z':
        radius += ZOOM_SPEED;
        break;
    }
    glutPostRedisplay();
}

// 마우스 클릭시 호출되는 함수
void MyMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        beforeX = x;
        beforeY = y;
    }
}

// 마우스가 클릭 된 상태에서 움직일 때 호출되는 함수
void MyMouseMove(GLint x, GLint y) {
    printf("MyMouseMove\n");
    printf("x: %d, y: %d", x, y);
    if (beforeX > x) {  // 마우스를 왼쪽으로 드래그
        hTheta += MOVE;
        if (beforeY > y)  // 마우스를 아래로 드래그
            vTheta += MOVE * 2;
        else if (beforeY < y)  // 마우스를 위로 드래그
            vTheta -= MOVE * 2;
    }
    else if (beforeX < x) {  // 마우스를 오른쪽으로 드래그
        hTheta -= MOVE;
        if (beforeY > y)
            vTheta += MOVE * 2;
        else if (beforeY < y)
            vTheta -= MOVE * 2;
    }

    beforeX = x;
    beforeY = y;

    glutPostRedisplay();

    // 360도가 넘게 된다면 물체가 확 돌지 않도록 처리(cf: 360도 = 2 * PI 라디안)
    if (hTheta > 2.0 * PI)
        hTheta -= (2.0 * PI);
    else if (hTheta < 0.0)
        hTheta += (2.0 * PI);
}

void createCylinder(GLfloat centerx, GLfloat centery, GLfloat centerz, GLfloat radius, GLfloat h)
{
    /* function createCyliner()
    원기둥의 중심 x,y,z좌표, 반지름, 높이를 받아 원기둥을 생성하는 함수(+z방향으로 원에서 늘어남)
    centerx : 원기둥 원의 중심 x좌표
    centery : 원기둥 원의 중심 y좌표
    centerz : 원기둥 원의 중심 z좌표
    radius : 원기둥의 반지름
    h : 원기둥의 높이
    */
    GLfloat x, y, angle;

    glBegin(GL_TRIANGLE_FAN);           //원기둥의 윗면
    glNormal3f(0.0f, 0.0f, -1.0f);
    glColor3ub(139, 69, 19);
    glVertex3f(centerx, centery, centerz);


    for (angle = 0.0f; angle < (2.0f * PI); angle += (PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(x, y, centerz);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);            //원기둥의 옆면
    for (angle = 0.0f; angle < (2.0f * PI); angle += (PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(sin(angle), cos(angle), 0.0f);
        glVertex3f(x, y, centerz);
        glVertex3f(x, y, centerz + h);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);           //원기둥의 밑면
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(centerx, centery, centerz + h);
    for (angle = (2.0f * PI); angle > 0.0f; angle -= (PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(x, y, centerz + h);
    }
    glEnd();
}

void createRing(GLfloat centerx, GLfloat centery, GLfloat centerz, GLfloat radius, GLfloat h, GLfloat thick)
{
    /* function createRing()
    링의 중심 x,y,z좌표, 반지름, 높이, 두께를 받아 링을 생성하는 함수
    centerx : 링의 중심 x좌표
    centery : 링의 중심 y좌표
    centerz : 링의 중심 z좌표
    radius : 링의 반지름
    h  : 링의 높이
    thick : 링의 두께
    */
    GLfloat x, y, angle;

    glColor3ub(148, 0, 211);
    glBegin(GL_QUAD_STRIP);           //링의 윗면
    for (angle = (2.0f * PI); angle > 0.0f; angle -= (PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(x, y, centerz);
        x = centerx + (radius - thick) * sin(angle);
        y = centery + (radius - thick) * cos(angle);
        glVertex3f(x, y, centerz);
    }
    glEnd();

    int color = 0;
    glBegin(GL_QUAD_STRIP);            //링의 바깥쪽 옆면
    for (angle = 0.0f; angle < (2.0f * PI); angle += (PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(sin(angle), cos(angle), 0.0f);
        glVertex3f(x, y, centerz);
        glVertex3f(x, y, centerz + h);
        color++;
    }
    glEnd();

    glColor3ub(148, 0, 211);
    glBegin(GL_QUAD_STRIP);            //링의 안쪽 옆면
    for (angle = (2.0f * PI); angle > 0.0f; angle -= (PI / 8.0f))
    {
        x = centerx + (radius - thick) * sin(angle);
        y = centery + (radius - thick) * cos(angle);
        glNormal3f(-sin(angle), -cos(angle), 0.0f);
        glVertex3f(x, y, centerz);
        glVertex3f(x, y, centerz + h);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);           //원기둥의 밑면
    for (angle = 0.0f; angle < (2.0f * PI); angle += (PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(x, y, centerz + h);
        x = centerx + (radius - thick) * sin(angle);
        y = centery + (radius - thick) * cos(angle);
        glVertex3f(x, y, centerz + h);
    }
    glEnd();
}

void createCircle(GLfloat r, GLfloat pos, GLfloat move) {
    GLfloat centerx = 0, centery = 0, centerz = 0;
    GLfloat x, y, angle;

    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.0f, 0.0f);
    glNormal3f(0.0f, 0.0f, pos);
    glVertex3f(centerx, centery, centerz + move);
    for (angle = (2.0f * PI); angle > 0.0f; angle -= (PI / 8.0f))
    {
        x = centerx + r * sin(angle);
        y = centery + r * cos(angle);
        glTexCoord2f(x, y);
        glNormal3f(0.0f, 0.0f, pos);
        glVertex3f(x, y, centerz + move);
    }
    glEnd();
}

void createSphere(GLfloat r)
{
    GLUquadricObj* sphere = gluNewQuadric(); // Quadrics obj 생성

    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricNormals(sphere, GLU_SMOOTH); // 법선벡터 빛 영향, GLU_FLAT는 면이 깍인것처럼 보이도록, GL_SMOOTH는 면이 부드럽게
    gluQuadricOrientation(sphere, GLU_OUTSIDE);
    gluQuadricTexture(sphere, GL_TRUE);
    glPolygonMode(GL_FRONT, GL_FILL);

    gluSphere(sphere, r, 50, 50); // 반지름이 r인 구가 생성되고, 50은 각각 수평적, 수직적 선의 수를 의미한다.
}

void createHalfSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat radius)
{
    /* function createHalfSphere()
    구의 중심 x, y, z 좌표를 받아 반구를 만드는 함수
    x : 반구의 중심 x 좌표
    y : 반구의 중심 y 좌표
    z : 반구의 중심 z 좌표
    raidus : 반구의 반지름
    */
    GLfloat angley;  //y축 값을 구하기 위한 각도
    GLfloat nexty;  //다음 y축 값을 구하기 위한 각도
    GLfloat anglex;  //x, y축 값을 구하기 위한 각도

    glColor3ub(0, 0, 255);  //반구의 색 지정
    glBegin(GL_QUAD_STRIP);
    for (angley = 0.0f; angley <= (0.5f * PI); angley += ((0.5f * PI) / 8.0f))  //반구만 그려야 하므로 0.5곱함
    {
        y = radius * sin(angley);     //y축 값 계산
        nexty = angley + ((0.5f * PI) / 8.0f);  //다음 angley값 저장
        for (anglex = 0.0f; anglex < (2.0f * PI); anglex += (PI / 8.0f))
        {
            x = radius * cos(angley) * sin(anglex);
            z = radius * cos(angley) * cos(anglex);
            glNormal3f(-cos(angley) * sin(anglex), -sin(angley), -cos(angley) * cos(anglex)); //반구의 안쪽으로 normal 벡터 생성
            glVertex3f(x, y, z);

            x = radius * cos(nexty) * sin(anglex);
            z = radius * cos(nexty) * cos(anglex);
            glNormal3f(-cos(nexty) * sin(anglex), -sin(nexty), -cos(nexty) * cos(anglex));
            glVertex3f(x, radius * sin(nexty), z);
        }
    }
    glEnd();
}

void getTexture(char* filename) {
    // 이미지 로딩
    int width, height, numChannels;
    unsigned char* image = stbi_load(filename, &width, &height, &numChannels, 0);
    if (!image) {
        printf("image file cannot open...\n");
        return;
    }

    // 텍스처 생성 및 설정
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);

    // 텍스처 매핑 설정
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D, textureID);
}



void MyDisplay() {
    printf("MyDisplay\n");

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluPerspective(50, 1.0, 1.0, 100.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 마우스 드래그를 통해 수평으로 이동한 각도(hTheta), 수직으로 이동한 각도(vTheta)를 계산하여
    // 카메라의 위치와 up vector를 조절
    x = radius * cos(hTheta) * cos(vTheta);
    y = radius * sin(hTheta) * cos(vTheta);
    z = -radius * sin(vTheta);
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, cos(vTheta));

    drawModel(); // 캐릭터 그리기

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL");

    // 조명 설정
    InitLight();

    // 키보드 이벤트 발생시 MyKeyboard 호출하도록 콜백 등록
    glutKeyboardFunc(MyKeyboard);

    // 화면 표시
    glutDisplayFunc(MyDisplay);

    // 마우스 클릭시: MyMouseClick 호출하도록 콜백 등록
    // 마우스 클릭한 상태에서 움직이면: MyMouseMove 호출하도록 콜백 등록
    glutMouseFunc(MyMouseClick);
    glutMotionFunc(MyMouseMove);

    // Reshape이벤트 발생시 MyReshape 호출하도록 콜백 등록
    // 1. 처음 윈도우를 열 때
    // 2. 윈도우 위치를 옮길 때
    // 3. 윈도우 크기를 조절할 때
    glutReshapeFunc(MyReshape);

    glutMainLoop();
    return 0;
}

/** 캐릭터 그리기 **/
void drawModel() {
    /** 머리 부분 제작 ***/
    draw_head();            // 머리
    draw_leftEar();         // 왼쪽 귀마개
    draw_rightEar();        // 오른쪽 귀마개
    draw_leftEye();         // 왼쪽눈
    draw_leftEyePupil();    // 왼쪽 흰자
    draw_rightEye();        // 오른쪽 눈
    draw_rightEyePupil();   // 오른쪽 흰자
    draw_leftFace();        // 왼쪽 볼
    draw_rightFace();       // 오른쪽 볼
    draw_leftEyeBrow();     // 왼쪽 눈썹
    draw_rightEyeBrow();    // 오른쪽 눈썹

    /*** 몸통 부분 시작 ****/
    draw_middle();          // 몸통
    draw_rightArm();        // 오른쪽 팔
    draw_leftArm();         // 왼쪽 팔
    draw_rightWrist();      // 오른쪽 손목
    draw_leftWrist();       // 왼쪽 손목
    draw_rightHand();       // 오른쪽 손
    draw_leftHand();        // 왼쪽 손
    draw_rightLeg();        // 오른쪽 다리
    draw_leftLeg();         // 왼쪽 다리
    draw_righttSock();      // 오른쪽 양말
    draw_leftSock();        // 왼쪽 양말
    draw_rightShoe();       // 오른쪽 신발
    draw_leftShoe();        // 왼쪽 신발
}

void draw_head() {
    char imageName[30] = "image/face.bmp";

    glPushMatrix(); {
        getTexture(imageName);

        glColor3f(0.901f, 0.69f, 0.545f);
        glTranslatef(0.0, 0.0, 1.8);
        glRotatef(60, 0.0, 0.0, 1.0);
        glScalef(1.0, 1.0, 1.0);

        // 구 그리기
        GLUquadricObj* sphere = gluNewQuadric(); // Quadrics obj 생성
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
        gluQuadricOrientation(sphere, GLU_OUTSIDE);
        gluQuadricTexture(sphere, GL_TRUE);
        gluSphere(sphere, 2.0f, 50, 50); // 반지름이 2.0인 구를 생성하고 텍스처를 적용합니다.
        gluDeleteQuadric(sphere); // Quadrics obj 제거

        // 텍스처 매핑 비활성화
        glBindTexture(GL_TEXTURE_2D, 0);

        // 렌더링 상태 초기화
        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
}

/** 왼쪽 귀마개 **/
void draw_leftEar() {
    glPushMatrix(); {
        glTranslatef(0.0, -1.6, 2.0);
        glRotatef(180, 0, 0, 1);
        createHalfSphere(0.0, 0.0, 0.0, 1.0);
    }
    glPopMatrix();
}

/** 오른쪽 귀마개 **/
void draw_rightEar() {
    glPushMatrix(); {
        glTranslatef(0.0, 1.6, 2.0);
        createHalfSphere(0.0, 0.0, 0.0, 1.0);
    }
    glPopMatrix();
}

/** 왼쪽 눈 **/
void draw_leftEye() {
    glPushMatrix(); {
        glColor3f(0, 0, 0);
        glTranslatef(1.9, -0.5, 2.0);
        glScalef(1.0, 1.0, 2.0);
        createSphere(0.17);
    }
    glPopMatrix();
}

/** 왼쪽 흰자 **/
void draw_leftEyePupil() {
    glPushMatrix(); {
        glColor3f(255, 255, 255);
        glTranslatef(1.99, -0.43, 2.05);
        glScalef(1.0, 1.0, 1.7);
        createSphere(0.1);
    }
    glPopMatrix();
}

/** 오른쪽 눈 **/
void draw_rightEye() {
    glPushMatrix(); {
        glColor3f(0, 0, 0);
        glTranslatef(1.9, 0.5, 2.0);
        glScalef(1.0, 1.0, 2.0);
        createSphere(0.16);
    }
    glPopMatrix();
}

/** 오른쪽 흰자 **/
void draw_rightEyePupil() {
    glPushMatrix(); {
        glColor3f(255, 255, 255);
        glTranslatef(1.99, 0.57, 2.05);
        glScalef(1.0, 1.0, 1.7);
        createSphere(0.1);
    }
    glPopMatrix();
}

/** 왼쪽 볼 **/
void draw_leftFace() {
    glPushMatrix(); {
        glColor3f(255, 0, 0);
        glTranslatef(1.7, -1.1, 1.3);
        glScalef(0.9, 2.0, 1.0);
        createSphere(0.18);
    }
    glPopMatrix();
}

/** 오른쪽 볼 **/
void draw_rightFace() {
    glPushMatrix(); {
        glColor3f(255, 0, 0);
        glTranslatef(1.7, 1.1, 1.3);
        glScalef(0.9, 2.0, 1.0);
        createSphere(0.18);
    }
    glPopMatrix();
}

/** 왼쪽 눈썹 **/
void draw_leftEyeBrow() {
    {
        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, -1.2, 2.5);
            glRotatef(45, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.5, 1.0);
            glutSolidCube(0.30);

        }
        glPopMatrix();

        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.955, 2.71);
            glRotatef(30, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.29);

        }
        glPopMatrix();

        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.75, 2.79);
            glRotatef(10, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.28);

        }
        glPopMatrix();

        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.65, 2.8);
            glScalef(1.0, 0.6, 1.0);
            glutSolidCube(0.28);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.55, 2.78);
            glRotatef(70, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.27);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.45, 2.732);
            glRotatef(60, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.26);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.37, 2.68);
            glRotatef(45, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.25);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.3, 2.60);
            glRotatef(30, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.24);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.22, 2.42);
            glRotatef(15, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.23);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.2, 2.3);
            glScalef(1.0, 1.0, 1.0);
            glRotatef(5, 1.0, 0.0, 0.0);
            glutSolidCube(0.22);

        }
        glPopMatrix();
    }
}

void draw_rightEyeBrow() {
    {
        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, 1.2, 2.5);
            glRotatef(-45, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.5, 1.0);
            glutSolidCube(0.30);

        }
        glPopMatrix();

        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.955, 2.71);
            glRotatef(60, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.29);

        }
        glPopMatrix();

        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.75, 2.79);
            glRotatef(80, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.28);

        }
        glPopMatrix();

        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.65, 2.8);
            glScalef(1.0, 0.6, 1.0);
            glutSolidCube(0.28);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.55, 2.78);
            glRotatef(20, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.27);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.45, 2.732);
            glRotatef(30, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.26);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.37, 2.68);
            glRotatef(45, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.25);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.3, 2.60);
            glRotatef(60, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.24);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.22, 2.42);
            glRotatef(75, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.23);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.2, 2.3);
            glScalef(1.0, 1.0, 1.0);
            glRotatef(85, 1.0, 0.0, 0.0);
            glutSolidCube(0.22);

        }
        glPopMatrix();
    }
}

void draw_middle() {
    char imageName[30] = "image/dao_belt.bmp";

    glPushMatrix(); {
        getTexture(imageName);

        glColor3f(0.08f, 0.4f, 0.9f);
        glTranslatef(0.0f, 0.0f, -1.0f);   // z축으로 -1만큼 이동
        glRotatef(110.0f, 0.5f, -0.5f, 0.9f);
        glScalef(1.0f, 1.0f, 1.0f);        // 크기 변환 (유지)

        // 구 그리기
        GLUquadricObj* sphere = gluNewQuadric(); // Quadrics obj 생성
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
        gluQuadricOrientation(sphere, GLU_OUTSIDE);
        gluQuadricTexture(sphere, GL_TRUE);
        gluSphere(sphere, 1.0f, 50, 50); // 반지름이 1.0인 구를 생성하고 텍스처를 적용합니다.
        gluDeleteQuadric(sphere); // Quadrics obj 제거

        // 텍스처 매핑 비활성화
        glBindTexture(GL_TEXTURE_2D, 0);

        // 렌더링 상태 초기화
        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
}

/** 오른쪽 팔 **/
void draw_rightArm() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(0.965f, 0.741f, 0.584f); // 살색 색깔 지정
        glTranslatef(0.5f, 1.3f, -1.5f); // 이동
        // 팔 부분의 회전 변환을 몸통에 부착하기 위해 몸통에 대한 변환
        glRotatef(-65.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(40.0f, 1.0f, 1.0f, 0.0f);
        gluCylinder(quad, 0.2f, 0.2f, 1.0f, 40, 40); // 객체, 하단, 상단, 높이, 둘레, 높이 방향 분할(스택 수) 
    }
    glPopMatrix();
}

/** 왼쪽 팔 **/
void draw_leftArm() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(0.965f, 0.741f, 0.584f); // 살색 색깔 지정
        glTranslatef(0.5f, -1.3f, -1.5f); // 이동
        // 팔 부분의 회전 변환을 몸통에 부착하기 위해 몸통에 대한 변환
        glRotatef(-45.0f, 1.0f, 1.0f, 0.0f);
        gluCylinder(quad, 0.2f, 0.2f, 1.0f, 40, 40); // 객체, 하단, 상단, 높이, 둘레, 높이 방향 분할(스택 수) 
    }
    glPopMatrix();
}

/** 오른쪽 손목 **/
void draw_rightWrist() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(1.0f, 1.0f, 1.0f); // 흰색 색깔 지정
        glTranslatef(0.5f, 1.3f, -1.5f); // 이동
        // 팔 부분의 회전 변환을 몸통에 부착하기 위해 몸통에 대한 변환
        glRotatef(-65.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(40.0f, 1.0f, 1.0f, 0.0f);
        gluCylinder(quad, 0.2f, 0.25f, 0.3f, 40, 40); // 객체, 하단, 상단, 높이, 둘레, 높이 방향 분할(스택 수)    
    }
    glPopMatrix();
}

/** 왼쪽 손목 **/
void draw_leftWrist() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(1.0f, 1.0f, 1.0f); // 흰색 색깔 지정
        glTranslatef(0.5f, -1.3f, -1.5f); // 이동
        // 팔 부분의 회전 변환을 몸통에 부착하기 위해 몸통에 대한 변환
        glRotatef(-45.0f, 1.0f, 1.0f, 0.0f);
        gluCylinder(quad, 0.2f, 0.25f, 0.25f, 40, 40); // 객체, 하단, 상단, 높이, 둘레, 높이 방향 분할(스택 수) 
    }
    glPopMatrix();
}

/** 오른쪽 손 **/
void draw_rightHand() {
    glPushMatrix(); {
        GLUquadric* sphere = gluNewQuadric();

        glColor3f(1.0f, 1.0f, 1.0f); // 흰색 색깔 지정

        glTranslatef(0.5f, 1.3f, -1.5f); // 이동
        // 팔 부분의 회전 변환을 몸통에 부착하기 위해 몸통에 대한 변환
        glRotatef(-65.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(40.0f, 1.0f, 1.0f, 0.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // yz 평면에 보이도록 회전

        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
        gluQuadricTexture(sphere, GL_TRUE);
        glPolygonMode(GL_FRONT, GL_FILL);
        gluSphere(sphere, 0.3f, 7, 50);
    }
    glPopMatrix();
}

/** 왼쪽 손 **/
void draw_leftHand() {
    glPushMatrix(); {
        GLUquadric* sphere = gluNewQuadric();

        glColor3f(1.0f, 1.0f, 1.0f); // 흰색 색깔 지정
        glTranslatef(0.5f, -1.3f, -1.5f); // 이동
        // 팔 부분의 회전 변환을 몸통에 부착하기 위해 몸통에 대한 변환
        glRotatef(-45.0f, 1.0f, 1.0f, 0.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // yz 평면에 보이도록 회전

        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
        gluQuadricTexture(sphere, GL_TRUE);
        glPolygonMode(GL_FRONT, GL_FILL);
        gluSphere(sphere, 0.3f, 7, 50);
    }
    glPopMatrix();
}

/** 오른쪽 다리 **/
void draw_rightLeg() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(0.965f, 0.741f, 0.584f); // 살색 색깔 지정
        glTranslatef(0.0f, 0.4f, -2.2f); // 이동
        // 팔 부분의 회전 변환을 몸통에 부착하기 위해 몸통에 대한 변환
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f); // z축과 평행하게 회전

        gluCylinder(quad, 0.2f, 0.2f, 0.5f, 40, 40); // 객체, 하단, 상단, 높이, 둘레, 높이 방향 분할(스택 수) 
    }
    glPopMatrix();
}

/** 왼쪽 다리 **/
void draw_leftLeg() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(0.965f, 0.741f, 0.584f); // 살색 색깔 지정
        glTranslatef(0.0f, -0.4f, -2.2f); // 이동
        // 팔 부분의 회전 변환을 몸통에 부착하기 위해 몸통에 대한 변환
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f); // z축과 평행하게 회전

        gluCylinder(quad, 0.2f, 0.2f, 0.5f, 40, 40); // 객체, 하단, 상단, 높이, 둘레, 높이 방향 분할(스택 수) 
    }
    glPopMatrix();
}

/** 오른쪽 양말 **/
void draw_righttSock() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(1.0f, 1.0f, 1.0f); // 흰색 색깔 지정
        glTranslatef(0.0f, 0.4f, -2.2f); // 이동
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f); // z축과 평행하게 회전
        gluCylinder(quad, 0.21f, 0.21f, 0.15f, 40, 40); // 객체, 하단, 상단, 높이, 둘레, 높이 방향 분할(스택 수)    
    }
    glPopMatrix();
}

/** 왼쪽 양말 **/
void draw_leftSock() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(1.0f, 1.0f, 1.0f); // 흰색 색깔 지정
        glTranslatef(0.0f, -0.4f, -2.2f); // 이동
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f); // z축과 평행하게 회전
        gluCylinder(quad, 0.21f, 0.21f, 0.15f, 40, 40); // 객체, 하단, 상단, 높이, 둘레, 높이 방향 분할(스택 수)    
    }
    glPopMatrix();
}

/** 오른쪽 신발 **/
void draw_rightShoe() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();
        glColor3f(0.08f, 0.4f, 0.9f);
        glTranslatef(0.0f, 0.4f, -2.35f);  // 육각형 상단을 위해 위치 이동
        glRotatef(100.0f, 0.0f, 0.0f, 1.0f); // z축과 평행하게 회전
        gluCylinder(quad, 0.3f, 0.3f, 0.2f, 6, 40);  // 원기둥 생성
    }
    glPopMatrix();
}

/** 왼쪽 신발 **/
void draw_leftShoe() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();
        glColor3f(0.08f, 0.4f, 0.9f);
        glTranslatef(0.0f, -0.4f, -2.35f);  // 육각형 상단을 위해 위치 이동
        glRotatef(80.0f, 0.0f, 0.0f, 1.0f); // z축과 평행하게 회전
        gluCylinder(quad, 0.3f, 0.3f, 0.2f, 6, 1);  // 원기둥 생성
    }
    glPopMatrix();
}
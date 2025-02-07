#define _USE_MATH_DEFINES

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/vec3.hpp>
#include <vector>
#include <math.h>
#include <random>

using namespace std;

int windowWidth = 900;
int windowHeight = 600;
bool gamePause = false;
bool gameReset = false;
bool debugMode = false;
bool renderingMode = true;
bool viewingEdge = true;
int viewingMode = 0; // 0: Default(3인칭 시점), 1: 1인칭 시점, 2: Side view
float ex = 0; float ey = 0; float ez = 1;
float cx = 0; float cy = 0; float cz = 0;
float ux = 1; float uy = 1; float uz = 0;  // params for gluLookAt

float degree_sun = 0;
GLfloat grassZ = 0.06f;

//random variable
random_device rd;
mt19937 mt(rd());
uniform_int_distribution<int> dist(0, 1000);

class Car;
class Wheel;
class Player;
class Tree;
class Boat;
class Shoe;
class Mushroom;
class JumpPad;

class CarManager;
class PlayerManager;
class TreeManager;
class BoatManager;


/**************draw functions*********************/
void drawLine(float x1, float y1, float z1, float x2, float y2, float z2, GLfloat width) {
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(width);

    // 선 그리기
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1); // 시작점
    glVertex3f(x2, y2, z2); // 끝점
    glEnd();
}

void drawStippleLine(float y, GLfloat width) {
    glColor3f(1, 1, 1);
    glLineWidth(width);

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00FF);
    glBegin(GL_LINES);
    glVertex2f(-3, y);
    glVertex2f(3, y);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}


void drawVerticalRect(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, bool drawEdge, float r, float g, float b)
{
    // function that draws "vertical" rectangle using (x1, y1, z1) and (x2, y2, z2) where x1 != x2, y1 != y2, z1 != z2 using color (r, g, b)
    glColor3f(r, g, b);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x1, y1, z2); glVertex3f(x2, y2, z2); glVertex3f(x2, y2, z1); glEnd();
    if (drawEdge)
    {
        glColor3f(0.0, 0.0, 0.0);
        glLineWidth(2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x1, y1, z2); glVertex3f(x2, y2, z2); glVertex3f(x2, y2, z1); glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void drawHorizontalRect(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, bool drawEdge, float r, float g, float b)
{
    // function that draws "orthogonal to xz-plane" rectangle using (x1, y1, z1) and (x2, y2, z2) where x1 != x2, y1 != y2, z1 != z2 using color (r, g, b)
    glColor3f(r, g, b);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x1, y2, z1); glVertex3f(x2, y2, z2); glVertex3f(x2, y1, z2); glEnd();
    if (drawEdge)
    {
        glColor3f(0.0, 0.0, 0.0);
        glLineWidth(2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x1, y2, z1); glVertex3f(x2, y2, z2); glVertex3f(x2, y1, z2); glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void drawCuboid(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, bool drawEdge)
{
    // (x1, y1, z1) -> smaller x, y, z
    // (x2, y2, z2) -> bigger x, y, z

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x1, y2, z1); glVertex3f(x2, y2, z1); glVertex3f(x2, y1, z1); glEnd(); // 아래
    glBegin(GL_POLYGON); glVertex3f(x1, y2, z1); glVertex3f(x1, y2, z2); glVertex3f(x2, y2, z2); glVertex3f(x2, y2, z1); glEnd(); // 뒤
    glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x1, y1, z2); glVertex3f(x1, y2, z2); glVertex3f(x1, y2, z1); glEnd(); // 왼
    glBegin(GL_POLYGON); glVertex3f(x2, y1, z1); glVertex3f(x2, y2, z1); glVertex3f(x2, y2, z2); glVertex3f(x2, y1, z2); glEnd(); // 오
    glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x2, y1, z1); glVertex3f(x2, y1, z2); glVertex3f(x1, y1, z2); glEnd(); // 앞
    glBegin(GL_POLYGON); glVertex3f(x1, y1, z2); glVertex3f(x2, y1, z2); glVertex3f(x2, y2, z2); glVertex3f(x1, y2, z2); glEnd(); // 위

    if (drawEdge)
    {
        glColor3f(0.0, 0.0, 0.0);
        glLineWidth(2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x1, y2, z1); glVertex3f(x2, y2, z1); glVertex3f(x2, y1, z1); glEnd(); // 아래
        glBegin(GL_POLYGON); glVertex3f(x1, y2, z1); glVertex3f(x1, y2, z2); glVertex3f(x2, y2, z2); glVertex3f(x2, y2, z1); glEnd(); // 뒤
        glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x1, y1, z2); glVertex3f(x1, y2, z2); glVertex3f(x1, y2, z1); glEnd(); // 왼
        glBegin(GL_POLYGON); glVertex3f(x2, y1, z1); glVertex3f(x2, y2, z1); glVertex3f(x2, y2, z2); glVertex3f(x2, y1, z2); glEnd(); // 오
        glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x2, y1, z1); glVertex3f(x2, y1, z2); glVertex3f(x1, y1, z2); glEnd(); // 앞
        glBegin(GL_POLYGON); glVertex3f(x1, y1, z2); glVertex3f(x2, y1, z2); glVertex3f(x2, y2, z2); glVertex3f(x1, y2, z2); glEnd(); // 위
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void drawVerticalCircle(float cx, float cy, float cz, float r)
{
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (int i = 0; i < 20; i++)
    {
        float theta = 2 * M_PI * i / 20;
        float nTheta = 2 * M_PI * (i + 1) / 20;
        glBegin(GL_TRIANGLE_STRIP);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(r * cos(theta), 0.0, r * sin(theta));
        glVertex3f(r * cos(nTheta), 0.0, r * sin(nTheta));
        glEnd();
    }
    glPopMatrix();
}

void drawCylinder(float cx, float cy, float cz, float r, float h, bool drawEdge)
{
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (int i = 0; i < 20; i++)
    {
        float theta = 2 * M_PI * i / 20;
        float nTheta = 2 * M_PI * (i + 1) / 20;
        glBegin(GL_TRIANGLE_STRIP);
        glVertex3f(0.0, 0.0, h / 2);
        glVertex3f(r * cos(theta), r * sin(theta), h / 2);
        glVertex3f(r * cos(nTheta), r * sin(nTheta), h / 2);

        glVertex3f(r * cos(theta), r * sin(theta), -h / 2);
        glVertex3f(r * cos(nTheta), r * sin(nTheta), -h / 2);
        glVertex3f(0.0, 0.0, -h / 2);
        glEnd();
    }

    if (drawEdge)
    {
        glColor3f(0.0, 0.0, 0.0);
        glLineWidth(2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (int i = 0; i < 20; i++)
        {
            float theta = 2 * M_PI * i / 20;
            float nTheta = 2 * M_PI * (i + 1) / 20;
            glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(0.0, 0.0, h / 2);
            glVertex3f(r * cos(theta), r * sin(theta), h / 2);
            glVertex3f(r * cos(nTheta), r * sin(nTheta), h / 2);

            glVertex3f(r * cos(theta), r * sin(theta), -h / 2);
            glVertex3f(r * cos(nTheta), r * sin(nTheta), -h / 2);
            glVertex3f(0.0, 0.0, -h / 2);
            glEnd();
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glPopMatrix();
}


/************************** LANE **************************/

float lane_pos[4] = { -5.0 / 8 ,-3.0 / 8 , 1.0 / 8 , 3.0 / 8 };
int lane_dx[4] = { -1,1,-1,1 };
float lane_speed[4];

void init_laneSpeed()
{
    for (int i = 0; i < 2; i++)
        lane_speed[i] = 0.001f * (dist(mt) % 3 + 2);
    for (int i = 2; i < 4; i++)
        lane_speed[i] = 0.001f * (dist(mt) % 4 + 3);
}

float field0_y = -7.0 / 8;
float field1_y = -1.0 / 8;

/**************************CUBOID***************************/
class Cuboid
{
public:
    Cuboid(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, float _r, float _g, float _b, float _tag)
        :x1(_x1), y1(_y1), z1(_z1), x2(_x2), y2(_y2), z2(_z2), r(_r), g(_g), b(_b), tag(_tag)
    {
        width = x2 - x1;
        length = y2 - y1;
        height = z2 - z1;
        cx = (x1 + x2) / 2;
        cy = (y1 + y2) / 2;
        cz = (z1 + z2) / 2;
    };

    float x1, y1, z1, x2, y2, z2, r, g, b;
    float width, length, height, cx, cy, cz;
    int tag;


    void draw(bool drawEdge)
    {
        glColor3f(r, g, b);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x1, y2, z1); glVertex3f(x2, y2, z1); glVertex3f(x2, y1, z1); glEnd(); // 아래
        glBegin(GL_POLYGON); glVertex3f(x1, y2, z1); glVertex3f(x1, y2, z2); glVertex3f(x2, y2, z2); glVertex3f(x2, y2, z1); glEnd(); // 뒤
        glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x1, y1, z2); glVertex3f(x1, y2, z2); glVertex3f(x1, y2, z1); glEnd(); // 왼
        glBegin(GL_POLYGON); glVertex3f(x2, y1, z1); glVertex3f(x2, y2, z1); glVertex3f(x2, y2, z2); glVertex3f(x2, y1, z2); glEnd(); // 오
        glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x2, y1, z1); glVertex3f(x2, y1, z2); glVertex3f(x1, y1, z2); glEnd(); // 앞
        glBegin(GL_POLYGON); glVertex3f(x1, y1, z2); glVertex3f(x2, y1, z2); glVertex3f(x2, y2, z2); glVertex3f(x1, y2, z2); glEnd(); // 위

        if (drawEdge)
        {
            glColor3f(0.0, 0.0, 0.0);
            glLineWidth(2);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x1, y2, z1); glVertex3f(x2, y2, z1); glVertex3f(x2, y1, z1); glEnd(); // 아래
            glBegin(GL_POLYGON); glVertex3f(x1, y2, z1); glVertex3f(x1, y2, z2); glVertex3f(x2, y2, z2); glVertex3f(x2, y2, z1); glEnd(); // 뒤
            glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x1, y1, z2); glVertex3f(x1, y2, z2); glVertex3f(x1, y2, z1); glEnd(); // 왼
            glBegin(GL_POLYGON); glVertex3f(x2, y1, z1); glVertex3f(x2, y2, z1); glVertex3f(x2, y2, z2); glVertex3f(x2, y1, z2); glEnd(); // 오
            glBegin(GL_POLYGON); glVertex3f(x1, y1, z1); glVertex3f(x2, y1, z1); glVertex3f(x2, y1, z2); glVertex3f(x1, y1, z2); glEnd(); // 앞
            glBegin(GL_POLYGON); glVertex3f(x1, y1, z2); glVertex3f(x2, y1, z2); glVertex3f(x2, y2, z2); glVertex3f(x1, y2, z2); glEnd(); // 위
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glColor3f(r, g, b);
        }
    }

    void checkTrigger(bool* triggerCoords, float cx2, float cy2, float cz2, float width2, float length2, float height2)
    {
        float diffX = abs(cx - cx2);
        float diffY = abs(cy - cy2);
        float diffZ = abs(cz - cz2);

        triggerCoords[0] = diffX < 0.5 * (width + width2);
        triggerCoords[1] = diffY < 0.5 * (length + length2);
        triggerCoords[2] = diffZ < 0.5 * (height + height2);
    }
};

/************************** Footbridge **************************/
class Footbridge {
public:
    float height_1st = grassZ + 0.075f; // 첫 번째 계단 높이
    float height_2nd = height_1st + 0.075f; // 두 번째 계단 높이
    float height_3rd = height_2nd + 0.075f; // 세 번째 계단 높이
    float height_4th = height_3rd + 0.075f; // 육교 높이

    float x_1st = -0.4f; // 첫 번째 계단 x
    float x_2nd = -0.5f; // 두 번째 계단 x
    float x_3rd = -0.6f; // 세 번째 계단 x
    float x_right_4th = -0.7f; // 육교 왼쪽 x
    float x_mid_4th = -0.8f; // 육교 중간 x
    float x_left_4th = -0.9f; // 육교 오른쪽 x

    float y_lower_1st = -0.9375; // 첫 들판 작은 y
    float y_upper_1st = -0.8175; // 첫 들판 큰 y
    float y_lower_2nd = -0.1875; // 두 번째 들판 작은 y
    float y_upper_2nd = -0.0625; // 두 번째 들판 큰 y

    Footbridge()
    {
        cuboids.push_back(new Cuboid(x_2nd, y_lower_1st, grassZ, x_1st, y_upper_1st, height_1st, 0.6f, 0.3f, 0.1f, 1));
        cuboids.push_back(new Cuboid(x_3rd, y_lower_1st, height_1st, x_2nd, y_upper_1st, height_2nd, 0.6f, 0.3f, 0.1f, 1));
        cuboids.push_back(new Cuboid(x_right_4th, y_lower_1st, height_2nd, x_3rd, y_upper_1st, height_3rd, 0.6f, 0.3f, 0.1f, 1));
        cuboids.push_back(new Cuboid(x_2nd, y_lower_2nd, grassZ, x_1st, y_upper_2nd, height_1st, 0.6f, 0.3f, 0.1f, 1));
        cuboids.push_back(new Cuboid(x_3rd, y_lower_2nd, height_1st, x_2nd, y_upper_2nd, height_2nd, 0.6f, 0.3f, 0.1f, 1));
        cuboids.push_back(new Cuboid(x_right_4th, y_lower_2nd, height_2nd, x_3rd, y_upper_2nd, height_3rd, 0.6f, 0.3f, 0.1f, 1));
        cuboids.push_back(new Cuboid(x_left_4th, y_lower_1st, grassZ, x_mid_4th, y_upper_1st, height_3rd, 0.6f, 0.3f, 0.1f, 1));
        cuboids.push_back(new Cuboid(x_left_4th, y_lower_2nd, grassZ, x_mid_4th, y_upper_2nd, height_3rd, 0.6f, 0.3f, 0.1f, 1));
        cuboids.push_back(new Cuboid(x_left_4th, y_lower_1st, height_3rd, x_right_4th, y_upper_2nd, height_4th, 0.8f, 0.5f, 0.3f, 1));

    }

    void draw()
    {
        for (int i = 0; i < cuboids.size(); i++)
            cuboids[i]->draw(viewingEdge);
    }

    vector<Cuboid*> cuboids;
};

Footbridge footbridge;


/************************** BACKGROUND **************************/
class BackGround
{
public:
    BackGround()
    {
        cuboids.push_back(new Cuboid(-10.0f, 0.75f, -0.1f, 5.0f, 3.0f, grassZ, 0.4f, 0.9f, 0.2f, 0)); //마지막 잔디
        cuboids.push_back(new Cuboid(-10.0f, 0.5, -0.1f, 5.0f, 0.75, -0.05f, 0.0f, 0.5f, 1.0f, 0)); //강
        cuboids.push_back(new Cuboid(-10.0f, 0.0f, -0.1f, 5.0f, 0.5, 0.0f, 0.3f, 0.3f, 0.3f, 0)); //도로
        cuboids.push_back(new Cuboid(-10.0f, -0.25f, -0.1f, 5.0f, 0, grassZ, 0.4f, 0.9f, 0.2f, 0)); //중간 잔디
        cuboids.push_back(new Cuboid(-10.0f, -0.75, -0.1f, 5.0f, -0.25, 0.0f, 0.3f, 0.3f, 0.3f, 0)); //도로
        cuboids.push_back(new Cuboid(-10.0f, -2.0, -0.1f, 5.0f, -0.75f, grassZ, 0.4f, 0.9f, 0.2f, 0));//처음 잔디

        //tunnel
        cuboids.push_back(new Cuboid(-10.0f, 0.0f, 0.0f, -1.2f, 0.03f, 0.4f, 0.4f, 0.4f, 0.4f, 0));
        cuboids.push_back(new Cuboid(-10.0f, 0.24f, 0.0f, -1.2f, 0.26f, 0.4f, 0.4f, 0.4f, 0.4f, 0));
        cuboids.push_back(new Cuboid(-10.0f, 0.47f, 0.0f, -1.2f, 0.5f, 0.4f, 0.4f, 0.4f, 0.4f, 0));
        cuboids.push_back(new Cuboid(-10.0f, 0.0f, 0.4f, -1.2f, 0.5f, 0.5f, 0.6f, 0.6f, 0.6f, 0));

        //tunnel 2
        cuboids.push_back(new Cuboid(1.2f, 0.0f, 0.0f, 10.0f, 0.03f, 0.4f, 0.4f, 0.4f, 0.4f, 0));
        cuboids.push_back(new Cuboid(1.2f, 0.24f, 0.0f, 10.0f, 0.26f, 0.4f, 0.4f, 0.4f, 0.4f, 0));
        cuboids.push_back(new Cuboid(1.2f, 0.47f, 0.0f, 10.0f, 0.5f, 0.4f, 0.4f, 0.4f, 0.4f, 0));
        cuboids.push_back(new Cuboid(1.2f, 0.0f, 0.4f, 10.0f, 0.5f, 0.5f, 0.6f, 0.6f, 0.6f, 0));

        //box
        cuboids.push_back(new Cuboid(1.2f, -0.1f, 0.0f, 1.4f, -0.0f, 0.25f, 0.6f, 0.3f, 0.1f, 0));
    }
    vector<Cuboid*> cuboids;

    void draw()
    {
        for (int i = 0; i < cuboids.size(); i++)
            cuboids[i]->draw(viewingEdge);
    }
};

BackGround backGround;

/************************** WHEEL **************************/
class Wheel
{
public:
    Wheel(float _radius, float _angularSpeed)
        :radius(_radius), angularSpeed(_angularSpeed) {};

    float radius, angularSpeed;
    int degree = 0;

    void draw(float x, float y, float z) {
        if (!gamePause)
            degree += angularSpeed;

        if (degree > 360)
            degree -= 360;
        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(90, 1, 0, 0);
        glRotatef(degree, 0.0, 0.0, 1.0);
        glColor3f(0.0, 0.0, 0.0);

        drawCylinder(0, 0, 0, radius, 0.02, viewingEdge);

        glColor3f(1.0, 1.0, 1.0);
        glRectf(-0.8 * radius, -0.2 * radius, 0.8 * radius, 0.2 * radius);
        drawCuboid(-0.7 * radius, -0.2 * radius, -0.012, 0.7 * radius, 0.2 * radius, 0.012, viewingEdge);
        glPopMatrix();
    }
};


/************************** CAR **************************/
class Car {
public:
    Car(float _x, float _y, float _speed, int _dx, int _type);
    float x, y, z = 0, speed;
    int dx; // -1: left, 1: right
    int type; // 0: 승용차, 1: 트럭
    int crain_degree = 0;
    bool isOpening = 1; // 0: 상판 닫히는 중, 1: 상판 열리는 중
    float length = 0.25 * 0.4;
    float width = 0.25 * 1.3;
    float height = 0.15;

    int ds = 1;
    vector<Wheel> wheels;

    void move()
    {
        x += dx * speed;
    }

    void draw()
    {
        glPushMatrix();
        glTranslatef(x, y, z);
        if (dx == 1) // right direction move
            glRotatef(180, 0, 0, 1);

        if (type) // truck
        {
            glColor3f(0.2f, 0.2f, 0.7f);
            drawCuboid(-0.5 * width, -0.5 * length, height * 0.3, 0.5 * width, 0.5 * length, height * 0.7, viewingEdge);
            glColor3f(0.2f, 0.2f, 0.7f);
            drawCuboid(-0.5 * width, -0.5 * length, height * 0.7, 0, 0.5 * length, height * 1.1, viewingEdge);

            if (!gamePause)
                crain_degree += ds;

            glPushMatrix();
            glRotatef(-crain_degree, 0, 1, 0);
            if (crain_degree > 30 || crain_degree < 0)
                ds *= -1;
            glColor3f(0.2f, 0.2f, 0.7f);
            drawCuboid(0, -0.5 * length, height * 0.7, 0.5 * width, 0.5 * length, height * 0.9, viewingEdge); // crain
            glPopMatrix();

        }
        else // car
        {
            glColor3f(1.0f, 0.7f, 0.2f);
            drawCuboid(-0.5 * width, -0.5 * length, height * 0.3, 0.5 * width, 0.5 * length, height * 0.7, viewingEdge);
            glColor3f(1.0f, 0.7f, 0.2f);
            drawCuboid(-0.3 * width, -0.5 * length, height * 0.7, 0.3 * width, 0.5 * length, height * 1.1, viewingEdge);

            glColor3f(0.2f, 0.2f, 0.7f);
        }

        int wheelCount = wheels.size();
        //draw wheel
        for (int i = 0; i < wheelCount; i += 2)
        {
            float wheelX = -(width * 1.4) / 2 + (width * 1.4) / (wheelCount / 2 + 1) * (i / 2 + 1);
            float wheelY = -length * 0.5;
            wheels[i].draw(wheelX, wheelY, height * 0.3);
            wheels[i + 1].draw(wheelX, -wheelY, height * 0.3);
        }

        glPopMatrix();
    }

};

Car::Car(float _x, float _y, float _speed, int _dx, int _type)
{
    x = _x;
    y = _y;
    dx = _dx;
    speed = _speed;
    type = _type;

    for (int i = 0; i < 4; i++)
    {
        Wheel* wheel;
        wheel = new Wheel(height * 0.3, speed / 0.04 * 180 / M_PI);
        wheels.push_back(*wheel);
    }
}


vector<Car*> cars;

/************************** BOAT **************************/
class Boat {
public:
    Boat(float _x, float _y, float _speed)
        :x(_x), y(_y), speed(_speed) {};

    float x, y, speed;
    float z = 0.02;
    int dx = -1;
    float height = 0.25 * 0.8;

    float width = height * 2;
    int degree = 30; // paddle 각도 -30~30
    int ds = 1;

    void move()
    {
        x += dx * speed;
        if ((x <= -1.5f) || (x >= 1.5f))
        {
            x *= -1;
            x += dx * speed;
        }
    }

    void draw()
    {
        if (!gamePause)
            degree -= ds;
        if (degree > 30 || degree < -30)
            ds *= -1;
        glPushMatrix(); // boat
        glTranslatef(x, y, z);
        glRotatef(degree * 0.15, 0, 1, 0);
        glColor3f(0.5f, 0.1f, 0.0f);
        glBegin(GL_POLYGON);
        glVertex3f(-width * 0.5f, 0, -0.05f); glVertex3f(-width * 0.35f, height * 0.3f, -0.05f);
        glVertex3f(-width * 0.1f, height * 0.4f, -0.05f); glVertex3f(width * 0.1f, height * 0.4f, -0.05f);
        glVertex3f(width * 0.48f, height * 0.3f, -0.05f); glVertex3f(width * 0.5f, 0, -0.05f);
        glVertex3f(width * 0.48f, -height * 0.3f, -0.05f); glVertex3f(width * 0.1f, -height * 0.4f, -0.05f);
        glVertex3f(-width * 0.1f, -height * 0.4f, -0.05f); glVertex3f(-width * 0.35f, -height * 0.3f, -0.05f);
        glEnd();
        drawVerticalRect(-width * 0.5f, 0, -0.09f, -width * 0.35f, height * 0.3f, 0, viewingEdge, 0.6f, 0.2f, 0.0f);
        drawVerticalRect(-width * 0.35f, height * 0.3f, -0.09f, -width * 0.1f, height * 0.4f, 0, viewingEdge, 0.6f, 0.2f, 0.0f);
        drawVerticalRect(-width * 0.1f, height * 0.4f, -0.09f, width * 0.1f, height * 0.4f, 0, viewingEdge, 0.6f, 0.2f, 0.0f);
        drawVerticalRect(width * 0.1f, height * 0.4f, -0.09f, width * 0.48f, height * 0.3f, 0, viewingEdge, 0.6f, 0.2f, 0.0f);
        drawVerticalRect(width * 0.48f, height * 0.3f, -0.09f, width * 0.5f, 0, 0, viewingEdge, 0.6f, 0.2f, 0.0f);
        drawVerticalRect(width * 0.5f, 0, -0.09f, width * 0.48f, -height * 0.3f, 0, viewingEdge, 0.6f, 0.2f, 0.0f);
        drawVerticalRect(width * 0.48f, -height * 0.3f, -0.09f, width * 0.1f, -height * 0.4f, 0, viewingEdge, 0.6f, 0.2f, 0.0f);
        drawVerticalRect(width * 0.1f, -height * 0.4f, -0.09f, -width * 0.1f, -height * 0.4f, 0, viewingEdge, 0.6f, 0.2f, 0.0f);
        drawVerticalRect(-width * 0.1f, -height * 0.4f, -0.09f, -width * 0.35f, -height * 0.3f, 0, viewingEdge, 0.6f, 0.2f, 0.0f);
        drawVerticalRect(-width * 0.35f, -height * 0.3f, -0.09f, -width * 0.5f, 0, 0, viewingEdge, 0.6f, 0.2f, 0.0f);

        glPushMatrix(); // upper paddle
        glTranslatef(0, height * 0.4f, 0);
        glRotatef(degree, 0, 0, 1);
        glColor3f(0.4f, 0.0f, 0.0f);
        glBegin(GL_POLYGON); // handle
        glVertex3f(-height * 0.03f, -height * 0.3f, -0.02f); glVertex3f(-height * 0.03f, height * 0.05f, -0.02f);
        glVertex3f(height * 0.03f, height * 0.05f, -0.02f); glVertex3f(height * 0.03f, -height * 0.3f, -0.02f);
        glEnd();
        drawVerticalRect(-height * 0.03f, -height * 0.3f, -0.04f, -height * 0.03f, height * 0.05f, -0.02f, viewingEdge, 0.4f, 0.0f, 0.0f);
        drawVerticalRect(height * 0.03f, height * 0.05f, -0.04f, height * 0.03f, -height * 0.3f, -0.02f, viewingEdge, 0.4f, 0.0f, 0.0f);
        drawVerticalRect(height * 0.03f, -height * 0.3f, -0.04f, -height * 0.03f, -height * 0.3f, -0.02f, viewingEdge, 0.4f, 0.0f, 0.0f);
        glColor3f(0.4f, 0.0f, 0.0f);
        glBegin(GL_POLYGON); // blade
        glVertex3f(-height * 0.03f, height * 0.05f, -0.02f); glVertex3f(-height * 0.08f, height * 0.1f, -0.02f); glVertex3f(-height * 0.08f, height * 0.2f, -0.02f);
        glVertex3f(height * 0.08f, height * 0.2f, -0.02f); glVertex3f(height * 0.08f, height * 0.1f, -0.02f); glVertex3f(height * 0.03f, height * 0.05f, -0.02f);
        glEnd();
        drawVerticalRect(-height * 0.03f, height * 0.05f, -0.02f, -height * 0.08f, height * 0.1f, -0.04f, viewingEdge, 0.4f, 0.0f, 0.0f);
        drawVerticalRect(-height * 0.08f, height * 0.1f, -0.02f, -height * 0.08f, height * 0.2f, -0.04f, viewingEdge, 0.4f, 0.0f, 0.0f);
        drawVerticalRect(-height * 0.08f, height * 0.2f, -0.02f, height * 0.08f, height * 0.2f, -0.04f, viewingEdge, 0.4f, 0.0f, 0.0f);
        drawVerticalRect(height * 0.08f, height * 0.2f, -0.02f, height * 0.08f, height * 0.1f, -0.04f, viewingEdge, 0.4f, 0.0f, 0.0f);
        drawVerticalRect(height * 0.08f, height * 0.1f, -0.02f, height * 0.03f, height * 0.05f, -0.04f, viewingEdge, 0.4f, 0.0f, 0.0f);
        glPopMatrix();

        glPushMatrix(); // lower paddle
        glTranslatef(0, -height * 0.4f, 0);
        glRotatef(-degree, 0, 0, 1);
        glColor3f(0.4f, 0.0f, 0.0f);
        glBegin(GL_POLYGON); // handle
        glVertex3f(-height * 0.03f, height * 0.3f, -0.02f); glVertex3f(-height * 0.03f, -height * 0.05f, -0.02f);
        glVertex3f(height * 0.03f, -height * 0.05f, -0.02f); glVertex3f(height * 0.03f, height * 0.3f, -0.02f);
        glEnd();
        drawVerticalRect(-height * 0.03f, height * 0.3f, -0.04f, -height * 0.03f, -height * 0.05f, -0.02f, viewingEdge, 0.4f, 0.0f, 0.0f);
        drawVerticalRect(height * 0.03f, -height * 0.05f, -0.04f, height * 0.03f, height * 0.3f, -0.02f, viewingEdge, 0.4f, 0.0f, 0.0f);
        drawVerticalRect(height * 0.03f, height * 0.3f, -0.04f, -height * 0.03f, height * 0.3f, -0.02f, viewingEdge, 0.4f, 0.0f, 0.0f);
        glColor3f(0.4f, 0.0f, 0.0f);
        glBegin(GL_POLYGON); // blade
        glVertex3f(-height * 0.03f, -height * 0.05f, -0.02f); glVertex3f(-height * 0.08f, -height * 0.1f, -0.02f); glVertex3f(-height * 0.08f, -height * 0.2f, -0.02f);
        glVertex3f(height * 0.08f, -height * 0.2f, -0.02f); glVertex3f(height * 0.08f, -height * 0.1f, -0.02f); glVertex3f(height * 0.03f, -height * 0.05f, -0.02f);
        glEnd();
        drawVerticalRect(-height * 0.03f, -height * 0.05f, -0.02f, -height * 0.08f, -height * 0.1f, -0.04f, viewingEdge, 0.4f, 0.0f, 0.0f);
        drawVerticalRect(-height * 0.08f, -height * 0.1f, -0.02f, -height * 0.08f, -height * 0.2f, -0.04f, viewingEdge, 0.4f, 0.0f, 0.0f);
        drawVerticalRect(-height * 0.08f, -height * 0.2f, -0.02f, height * 0.08f, -height * 0.2f, -0.04f, viewingEdge, 0.4f, 0.0f, 0.0f);
        drawVerticalRect(height * 0.08f, -height * 0.2f, -0.02f, height * 0.08f, -height * 0.1f, -0.04f, viewingEdge, 0.4f, 0.0f, 0.0f);
        drawVerticalRect(height * 0.08f, -height * 0.1f, -0.02f, height * 0.03f, -height * 0.05f, -0.04f, viewingEdge, 0.4f, 0.0f, 0.0f);
        glPopMatrix();

        glPopMatrix();
    }
};

vector<Boat*> boats;
/************************** TREE **************************/
class Tree {
public:
    Tree(float x_input, int field_input);
    float x, y;
    float z = grassZ;
    float length = 0.25 * 0.8;
    float width = length;
    float height = 0.4;
    vector <Cuboid*> cuboids;

    void draw()
    {
        glPushMatrix();
        glTranslatef(x, y, grassZ);

        glPushMatrix(); // wood
        glColor3f(0.8f, 0.4f, 0.4f); // 갈색
        GLfloat wood_x_half = 0.1f * width;
        GLfloat wood_y_half = 0.1f * length;
        drawCuboid(-wood_x_half, -wood_y_half, 0, wood_x_half, wood_y_half, height / 2, viewingEdge);
        glPopMatrix();

        glPushMatrix(); // 나뭇잎
        glTranslatef(0, 0, height / 2);
        GLfloat lowerleaf_x_half = 0.5f * width;
        GLfloat lowerleaf_y_half = 0.5f * length;
        glColor3f(0.2f, 0.7f, 0.0f); // 초록색
        drawCuboid(-lowerleaf_x_half, -lowerleaf_y_half, 0, lowerleaf_x_half, lowerleaf_y_half, height / 4, viewingEdge);
        glColor3f(0.2f, 0.7f, 0.0f); // 초록색
        drawCuboid(-lowerleaf_x_half, -lowerleaf_y_half, 0, lowerleaf_x_half, lowerleaf_y_half, height / 4, viewingEdge);
        glTranslatef(0, 0, height / 4);
        GLfloat upperleaf_x_half = 0.3f * width;
        GLfloat upperleaf_y_half = 0.3f * length;
        glColor3f(0.2f, 0.7f, 0.0f); // 초록색
        drawCuboid(-upperleaf_x_half, -upperleaf_y_half, 0, upperleaf_x_half, upperleaf_y_half, height / 4, viewingEdge);
        glColor3f(0.2f, 0.7f, 0.0f); // 초록색
        drawCuboid(-upperleaf_x_half, -upperleaf_y_half, 0, upperleaf_x_half, upperleaf_y_half, height / 4, viewingEdge);
        glPopMatrix();

        glPopMatrix();
    }
};
Tree::Tree(float x_input, int field)
{
    x = x_input;
    switch (field)
    {
    case 0:
        y = field0_y;
        break;
    case 1:
        y = field1_y;
        break;
    default:
        y = 0.0f;
        break;
    }

    cuboids.push_back(new Cuboid(x - 0.5 * width, y - 0.5 * width, z, x + 0.5 * width, y + 0.5 * width, z + height, 0, 0, 0, 2));
}

vector<Tree*> trees;

/************************** SHOE **************************/
class Shoe {
public:
    Shoe(float x_input, float y_input);
    float height = 0.25 * 0.3;
    float length = height * 2.8;
    float width = height / 3;
    float x, y, z = grassZ + height * 0.5;

    void move()
    {

    }
    void draw()
    {
        glPushMatrix();
        glTranslatef(x, y, z);
        glColor3f(1.0f, 0.1f, 0.5f);

        glBegin(GL_POLYGON); // 신발 뒷면
        glVertex3f(-length * 0.5f, width * 0.5f, -height * 0.28f);
        glVertex3f(length * 0.5f, width * 0.5f, -height * 0.28f);
        glVertex3f(length * 0.2f, width * 0.5f, height * 0.14f);
        glVertex3f(-length * 0.1f, width * 0.5f, height * 0.5f);
        glVertex3f(-length * 0.25f, width * 0.5f, height * 0.28f);
        glVertex3f(-length * 0.45f, width * 0.5f, height * 0.42f);
        glEnd();

        glBegin(GL_POLYGON); // 신발 앞면
        glVertex3f(-length * 0.5f, -width * 0.5f, -height * 0.28f);
        glVertex3f(length * 0.5f, -width * 0.5f, -height * 0.28f);
        glVertex3f(length * 0.2f, -width * 0.5f, height * 0.14f);
        glVertex3f(-length * 0.1f, -width * 0.5f, height * 0.5f);
        glVertex3f(-length * 0.25f, -width * 0.5f, height * 0.28f);
        glVertex3f(-length * 0.45f, -width * 0.5f, height * 0.42f);
        glEnd();

        drawHorizontalRect(-length * 0.5f, -width * 0.5f, -height * 0.28f, -length * 0.45f, width * 0.5f, height * 0.42f, viewingEdge, 1.0f, 0.1f, 0.5f);
        drawHorizontalRect(-length * 0.45f, -width * 0.5f, height * 0.42f, -length * 0.25f, width * 0.5f, height * 0.28f, viewingEdge, 1.0f, 0.1f, 0.5f);
        drawHorizontalRect(-length * 0.25f, -width * 0.5f, height * 0.28f, -length * 0.1f, width * 0.5f, height * 0.5f, viewingEdge, 1.0f, 0.1f, 0.5f);
        drawHorizontalRect(-length * 0.1f, -width * 0.5f, height * 0.5f, length * 0.2f, width * 0.5f, height * 0.14f, viewingEdge, 1.0f, 0.1f, 0.5f);
        drawHorizontalRect(length * 0.2f, -width * 0.5f, height * 0.14f, length * 0.5f, width * 0.5f, -height * 0.28f, viewingEdge, 1.0f, 0.1f, 0.5f);

        glColor3f(1.0f, 1.0f, 1.0f);
        drawCuboid(-length * 0.5f, -width * 0.5f, -height * 0.5f, length * 0.5f, width * 0.5f, -height * 0.28f, viewingEdge); // 밑창

        glLineWidth(40 * length);

        glBegin(GL_LINES); // 삼선 앞면
        glVertex3f(-length * 0.2f, width * 0.5f + 0.0001, -height * 0.28f); glVertex3f(-length * 0.095f, width * 0.5f + 0.0001, height * 0.14f);
        glVertex3f(-length * 0.08f, width * 0.5f + 0.0001, -height * 0.28f); glVertex3f(length * 0.0075f, width * 0.5f + 0.0001, height * 0.07f);
        glVertex3f(length * 0.04f, width * 0.5f + 0.0001, -height * 0.28f); glVertex3f(length * 0.11f, width * 0.5f + 0.0001, height * 0);
        glEnd();

        glBegin(GL_LINES); // 삼선 앞면
        glVertex3f(-length * 0.2f, -width * 0.5f - 0.0001, -height * 0.28f); glVertex3f(-length * 0.095f, -width * 0.5f - 0.0001, height * 0.14f);
        glVertex3f(-length * 0.08f, -width * 0.5f - 0.0001, -height * 0.28f); glVertex3f(length * 0.0075f, -width * 0.5f - 0.0001, height * 0.07f);
        glVertex3f(length * 0.04f, -width * 0.5f - 0.0001, -height * 0.28f); glVertex3f(length * 0.11f, -width * 0.5f - 0.0001, height * 0);
        glEnd();
        glPopMatrix();
    }

};

Shoe::Shoe(float x_input, float y_input)
{
    x = x_input;
    y = y_input;
    return;
}
Shoe* shoe = new Shoe(0.9f, field1_y);
vector<Shoe*> shoes = { shoe, };


/************************** Mushroom **************************/
class Mushroom {
public:
    Mushroom(float _x, float _y);
    float x, y;
    float z = grassZ;
    float length = 0.25 * 0.4;
    float height = 0.15;
    float width = length;

    void move()
    {
    }

    void draw()
    {
        glPushMatrix();
        glTranslatef(x, y, grassZ);
        glColor3f(1.0f, 8.0f, 1.0f);
        drawCuboid(-width * 0.1, -length * 0.1, 0, width * 0.1, length * 0.1, height * 0.5, viewingEdge);
        glColor3f(0.8f, 0.2f, 0.7f);
        drawCylinder(0, 0, height * 0.6, width * 0.5, height * 0.4, viewingEdge);
        glColor3f(0.8f, 0.2f, 0.7f);
        drawCylinder(0, 0, height * 0.8, width * 0.3, height * 0.3, viewingEdge);
        glPopMatrix();
    }

};

Mushroom::Mushroom(float _x, float _y)
{
    x = _x;
    y = _y;
    return;
}
Mushroom* mushroom = new Mushroom(0.1f, field1_y);
vector<Mushroom*> mushrooms = { mushroom, };


/************************** JumpPad **************************/
class JumpPad {
public:
    float x, y;
    float z = grassZ;
    float width = 0.15;
    float length = width;
    float height = 0.03;

    JumpPad(float _x, float _y) : x(_x), y(_y) {}

    void draw()
    {
        glPushMatrix();
        glTranslatef(x, y, z);

        glColor3f(0.65, 0.3, 0);
        drawCuboid(-0.5 * width, -0.5 * length, 0, 0.5 * width, 0.5 * length, height, viewingEdge);

        glTranslatef(0, 0, height + 0.001);
        glColor3f(1, 1, 0);
        glBegin(GL_POLYGON);
        glVertex3f(-0.25 * width, 0.1 * length, 0);
        glVertex3f(0, 0.3 * length, 0);
        glVertex3f(0.25 * width, 0.1 * length, 0);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex3f(0.1 * width, 0.1 * length, 0);
        glVertex3f(0.1 * width, -0.3 * length, 0);
        glVertex3f(-0.1 * width, -0.3 * length, 0);
        glVertex3f(-0.1 * width, 0.1 * length, 0);
        glEnd();
        glPopMatrix();
    }
};

JumpPad* jumpPad = new JumpPad(0.7f, field0_y);
vector<JumpPad*> jumpPads = { jumpPad, };

/************************** PLAYER **************************/
class Player {
public:

    float initZ = grassZ;
    float x, y, z = initZ;
    float speedX, speedY, speedZ = 0;
    int dx = 0, dy = 0, dz = -1;

    float height = 0.25;
    float width = height * 0.4;
    float length = height * 0.25;

    int moveKeyLogger[4] = { 0,0,0,0 };

    bool isMoving = false;
    bool didGetShoe = false;

    //jump
    bool isJumping = false;

    //jumpPad
    bool isJumpPad = false;
    float jumpSpeed = 0.01;
    float jumpSpeedY = jumpSpeed * 0.7;
    float jumpSpeedZ = jumpSpeed * 0.7;
    float g = (5 * jumpSpeed * jumpSpeed) / 3;

    //emotion (when car is near or get mushroom)
    bool action_mode[2] = { false,false };

    //amimation angle
    int ds = -1;
    float degree_arms_legs = 25;

    Player(float _x, float _y, float _speedX, float _speedY)
    {
        x = _x;
        y = _y;
        speedX = _speedX;
        speedY = _speedY;
        if (y <= 0.75 || (-0.25 < y && y <= 0) || y > 0.75) // when player is on grass
        {
            z = grassZ;
        }
        else if ((y > -0.75 && y <= -0.25) || (y > 0 && y <= 0.5)) // when player is on road
        {
            z = 0;
        }
        else if (y > 0.5 && y <= 0.75) // when players is on boat
        {
            z = -0.05;
        }
    }

    void checkMoveable(bool* moveable, float x, float y, float z, float nx, float ny, float nz)
    {
        bool preTrigger[3] = {};
        bool nextTrigger[3] = {};

        for (int j = 0; j < 3; j++)
            moveable[j] = true;

        for (int i = 0; i < backGround.cuboids.size(); i++)
        {
            backGround.cuboids[i]->checkTrigger(nextTrigger, nx, ny, nz + 0.5 * height, width, length, height);
            if (nextTrigger[0] & nextTrigger[1] & nextTrigger[2])
            {
                backGround.cuboids[i]->checkTrigger(preTrigger, x, y, z + 0.5 * height, width, length, height);
                for (int j = 0; j < 3; j++)
                    moveable[j] = (!(nextTrigger[j] ^ preTrigger[j])) & moveable[j];
            }
        }

        for (int i = 0; i < footbridge.cuboids.size(); i++)
        {
            footbridge.cuboids[i]->checkTrigger(nextTrigger, nx, ny, nz + 0.5 * height, width, length, height);
            if (nextTrigger[0] & nextTrigger[1] & nextTrigger[2])
            {
                footbridge.cuboids[i]->checkTrigger(preTrigger, x, y, z + 0.5 * height, width, length, height);
                for (int j = 0; j < 3; j++)
                    moveable[j] = (!(nextTrigger[j] ^ preTrigger[j])) & moveable[j];
            }
        }

        if (!debugMode)
        {
            for (int i = 0; i < trees.size(); i++)
            {
                for (int j = 0; j < trees[i]->cuboids.size(); j++)
                {
                    trees[i]->cuboids[j]->checkTrigger(nextTrigger, nx, ny, nz + 0.5 * height, width, length, height);
                    if (nextTrigger[0] & nextTrigger[1] & nextTrigger[2])
                    {
                        trees[i]->cuboids[j]->checkTrigger(preTrigger, x, y, z + 0.5 * height, width, length, height);
                        for (int k = 0; k < 3; k++)
                            moveable[k] = (!(nextTrigger[k] ^ preTrigger[k])) & moveable[k];
                    }
                }
            }
        }
    }

    bool checkCrash(void)
    {
        float distance_x, distance_y, distance_z;
        float difference_x, difference_y, difference_z;

        if (debugMode)
            return false;

        action_mode[0] = false;
        for (int i = 0; i < cars.size(); i = i + 1)
        {
            Car* temp = cars[i];
            distance_x = abs(temp->x - x);
            distance_y = abs(temp->y - y);
            distance_z = abs((temp->z + 0.5 * temp->height) - (z + 0.5 * height));
            //distance_z = abs(temp->z - z);

            difference_x = (temp->width + width) * 0.5f - distance_x;
            difference_y = (temp->length + length) * 0.5f - distance_y;
            difference_z = (temp->height + height) * 0.5f - distance_z;

            if ((difference_x > 0) && (difference_y > 0) && (difference_z > 0))
            {
                return true;
            }
            else if ((difference_x > -0.15) && (difference_y > -0.01))
            {
                action_mode[0] = true;
            }
        }
        return false;
    }

    void reset()
    {
        if (didGetShoe)
        {
            speedX /= 2;
            speedY /= 2;
            ds /= 2;
            didGetShoe = false;
        }

        if (action_mode[1])
        {
            action_mode[1] = false;
            speedX *= -1;
            speedY *= -1;
        }

        x = 0.0f;
        y = -7.0 / 8;
        z = initZ + 0.1;
    }

    bool checkInRiver(void)
    {
        if (debugMode)
            return false;

        float river_y = 5.0 / 8;
        float river_height = 0.25;

        if ((y < river_y + 0.5 * river_height) && (y > river_y - 0.5 * river_height) && (z < 0))
            return true;
        return false;
    }

    void checkGetItem(void)
    {
        for (int i = 0; i < shoes.size(); i = i + 1)
        {
            float distance_x = abs(shoes[i]->x - x);
            float distance_y = abs(shoes[i]->y - y);
            float  distance_z = abs(shoes[i]->z - z);
            if (((shoes[i]->length + width) * 0.5f > distance_x) & ((shoes[i]->width + length) * 0.5f > distance_y) & ((shoes[i]->height + length) * 0.5f > distance_z))
            {
                didGetShoe = true;
                ds *= 2;
                shoes.erase(shoes.begin() + i);
                speedX *= 2;
                speedY *= 2;
            }
        }

        for (int i = 0; i < mushrooms.size(); i = i + 1)
        {
            float distance_x = abs(mushrooms[i]->x - x);
            float distance_y = abs(mushrooms[i]->y - y);
            if (((mushrooms[i]->width + width) * 0.5f > distance_x) && ((mushrooms[i]->length + length) * 0.5f > distance_y))
            {
                mushrooms.erase(mushrooms.begin() + i);
                speedX *= -1;
                speedY *= -1;
                action_mode[1] = true;
            }
        }
    }

    bool checkInJumpPad(void)
    {
        for (int i = 0; i < jumpPads.size(); i = i + 1)
        {
            float distance_x = abs(jumpPads[i]->x - x);
            float distance_y = abs(jumpPads[i]->y - y);
            if ((jumpPads[i]->width * 0.25f > distance_x) && (jumpPads[i]->length * 0.25f > distance_y))
            {
                return true;
            }
        }
        return false;
    }

    bool checkGameClear(void)
    {
        bool didClear = false;

        if (y >= 7.0 / 8) didClear = true;

        return didClear;
    }

    void move()
    {
        if (isJumpPad)
        {
            isMoving = true;
            isJumping = false;
            y += jumpSpeedY * 1.1;
            z += jumpSpeedZ * 4;
            jumpSpeedZ -= g;

            if (z < initZ)
            {
                z = initZ;
                jumpSpeedY = jumpSpeed * 0.7;
                jumpSpeedZ = jumpSpeed * 0.7;
                isJumpPad = false;
            }
        }

        else
        {
            dx = moveKeyLogger[3] - moveKeyLogger[2];
            dy = moveKeyLogger[0] - moveKeyLogger[1];
            dz = -1;

            if ((dx != 0) || (dy != 0))
                isMoving = true;
            else
                isMoving = false;

            bool moveable[3] = {};
            float nx = x + dx * speedX;
            float ny = y + dy * speedY;
            float nz = z + speedZ;

            checkMoveable(moveable, x, y, z, nx, ny, nz);

            if (!moveable[2])
                isJumping = false;

            x += dx * speedX * moveable[0];
            y += dy * speedY * moveable[1];
            z += speedZ * moveable[2];

            if (moveable[2])
                speedZ -= g;
            else
                speedZ = 0;

            //game boundary
            if (x >= 1.25)
                x = 1.25;
            else if (x <= -1.25)
                x = -1.25;
            if (y < -7.0 / 8)
                y = -7.0 / 8;
        }
    }

    float getTransportationMove()
    {

        float distance_x, distance_y;
        float difference_x, difference_y;

        for (int i = 0; i < boats.size(); i = i + 1)
        {
            Boat* temp = boats[i];
            distance_x = abs(temp->x - x);
            distance_y = abs(temp->y - y);

            if ((distance_x < (0.5 * temp->width)))
            {
                return temp->speed * temp->dx;
            }
        }
        return 0;
    }

    void draw()
    {
        glPushMatrix(); // player
        glTranslatef(x, y, z);
        if (dx != 0 | dy != 0)
        {
            float direction = acos(-(float)dy / sqrt(pow(dx, 2) + pow(dy, 2))) * 180 / M_PI;
            if (dx < 0)
                direction *= -1;
            glRotatef(direction, 0, 0, 1);
        }
        if (y > 0.5 && y <= 0.75 && !debugMode && !isJumping) glRotatef(boats[0]->degree * 0.15, 0, 1, 0);

        glPushMatrix(); // torso
        glTranslatef(0, 0, height * 0.5);
        glColor3f(0.3f, 0.8f, 0.7f); // mint
        drawCuboid(-width * 0.25, -length * 0.25, -height * 0.125, width * 0.25, length * 0.25, height * 0.25, viewingEdge);


        glPushMatrix(); // head
        glTranslatef(0, 0, height * 0.375);
        if (isMoving)
        {
            glRotatef(degree_arms_legs * 2, 0, 0, 1);
            degree_arms_legs -= ds;
            if (degree_arms_legs > 25 || degree_arms_legs < -25)
                ds *= -1;
        }
        glColor3f(1.0f, 0.9f, 0.7f); // skin
        drawCuboid(-width * 0.25, -length * 0.5, -height * 0.125, width * 0.25, length * 0.5, height * 0.125, viewingEdge); // head

        glPushMatrix(); // 얼굴 앞면
        glTranslatef(0, -length * 0.5 - 0.0001, 0);
        drawVerticalRect(-width * 0.25 - 0.0001, 0, 0, -width * 0.125, 0, height * 0.125 + 0.0001, false, 0.25, 0.2, 0.1); // left fringe
        drawVerticalRect(-width * 0.125, 0, height * 0.0625, width * 0.125, 0, height * 0.125 + 0.0001, false, 0.25, 0.2, 0.1); // fringe
        drawVerticalRect(width * 0.125, 0, 0, width * 0.25 + 0.0001, 0, height * 0.125 + 0.0001, false, 0.25, 0.2, 0.1); // right fringe
        drawVerticalRect(-width * 0.1875, 0, -height * 0.03125, -width * 0.125, 0, 0, false, 1.0f, 1.0f, 1.0f); // left eye white
        drawVerticalRect(-width * 0.125, 0, -height * 0.03125, -width * 0.0625, 0, 0, false, 0.2f, 0.2f, 1.0f); // left eye
        drawVerticalRect(width * 0.125, 0, -height * 0.03125, width * 0.1875, 0, 0, false, 1.0f, 1.0f, 1.0f); // right eye white
        drawVerticalRect(width * 0.0625, 0, -height * 0.03125, width * 0.125, 0, 0, false, 0.2f, 0.2f, 1.0f); // right eye
        drawVerticalRect(-width * 0.0625, 0, -height * 0.09375, width * 0.0625, 0, -height * 0.0625, false, 0.5f, 0.1f, 0.1f); // mouth
        glPopMatrix();
        glColor3f(0.25, 0.2, 0.1);
        glBegin(GL_POLYGON); glVertex3f(width * 0.25 + 0.0001, 0, 0); glVertex3f(width * 0.25 + 0.0001, length * 0.5 + 0.0001, 0); glVertex3f(width * 0.25 + 0.00001, length * 0.5 + 0.0001, -height * 0.0625); glEnd(); // left back hair
        glBegin(GL_POLYGON); glVertex3f(-width * 0.25 - 0.0001, 0, 0); glVertex3f(-width * 0.25 - 0.0001, length * 0.5 + 0.0001, 0); glVertex3f(-width * 0.25 - 0.00001, length * 0.5 + 0.0001, -height * 0.0625); glEnd(); // right back hair
        drawVerticalRect(width * 0.25 + 0.0001, -length * 0.5 - 0.0001, 0, width * 0.25 + 0.0001, length * 0.5 + 0.0001, height * 0.125 + 0.0001, false, 0.25, 0.2, 0.1); // left hair 
        drawVerticalRect(-width * 0.25 - 0.0001, -length * 0.5 - 0.0001, 0, -width * 0.25 - 0.0001, length * 0.5 + 0.0001, height * 0.125 + 0.0001, false, 0.25, 0.2, 0.1); // right hair
        drawVerticalRect(-width * 0.25 - 0.0001, length * 0.5 + 0.0001, -height * 0.0625 - 0.0001, width * 0.25 + 0.0001, length * 0.5 + 0.0001, height * 0.125, false, 0.25, 0.2, 0.1); // back hair
        drawHorizontalRect(-width * 0.25 - 0.0001, -length * 0.5 - 0.0001, height * 0.125 + 0.0001, width * 0.25 + 0.0001, length * 0.5 + 0.0001, height * 0.125 + 0.0001, false, 0.25, 0.2, 0.1); // upper hair

        glPopMatrix(); // head pop


        glPushMatrix(); // left arm
        glTranslatef(-width * 0.25, 0, height * 0.1875);
        if (isMoving && !checkGameClear()) glRotatef(-degree_arms_legs, 1, 0, 0);
        if (checkGameClear()) glRotatef(180, 1, 0, 0);
        glColor3f(0.3f, 0.8f, 0.7f); // mint
        drawCuboid(-width * 0.25, -length * 0.25, -height * 0.0625, 0, length * 0.25, height * 0.0625, viewingEdge);
        glColor3f(1.0f, 0.9f, 0.7f); // skin
        drawCuboid(-width * 0.25, -length * 0.25, -height * 0.3125, 0, length * 0.25, -height * 0.0625, viewingEdge);
        glPopMatrix(); // left arm pop


        glPushMatrix(); // right arm
        glTranslatef(width * 0.25, 0, height * 0.1875);
        if (isMoving && !checkGameClear()) glRotatef(degree_arms_legs, 1, 0, 0);
        if (checkGameClear()) glRotatef(180, 1, 0, 0);
        glColor3f(0.3f, 0.8f, 0.7f); // mint
        drawCuboid(0, -length * 0.25, -height * 0.0625, width * 0.25, length * 0.25, height * 0.0625, viewingEdge);
        glColor3f(1.0f, 0.9f, 0.7f); // skin
        drawCuboid(0, -length * 0.25, -height * 0.3125, width * 0.25, length * 0.25, -height * 0.0625, viewingEdge);
        glPopMatrix(); // right arm pop


        glPushMatrix(); // left leg
        glTranslatef(-width * 0.125, 0, -height * 0.125);
        if (isMoving && !checkGameClear()) glRotatef(degree_arms_legs, 1, 0, 0);
        glColor3f(0.2f, 0.1f, 0.7f);
        drawCuboid(-width * 0.125, -length * 0.25, -height * 0.375, width * 0.125, length * 0.25, 0, viewingEdge);
        if (didGetShoe)
        {
            glColor3f(1.0f, 0.1f, 0.5f); // pink
            drawCuboid(-width * 0.125 - 0.001, -length * 0.25 - 0.001, -height * 0.375, width * 0.125 + 0.001, length * 0.25 + 0.001, -height * 0.3125, viewingEdge); // shoe color
        }
        glPopMatrix(); // left leg pop


        glPushMatrix(); // right leg
        glTranslatef(width * 0.125, 0, -height * 0.125);
        if (isMoving && !checkGameClear()) glRotatef(-degree_arms_legs, 1, 0, 0);
        glColor3f(0.2f, 0.1f, 0.7f);
        drawCuboid(-width * 0.125, -length * 0.25, -height * 0.375, width * 0.125, length * 0.25, 0, viewingEdge);
        if (didGetShoe)
        {
            glColor3f(1.0f, 0.1f, 0.5f); // pink
            drawCuboid(-width * 0.125 - 0.001, -length * 0.25 - 0.001, -height * 0.375, width * 0.125 + 0.001, length * 0.25 + 0.001, -height * 0.3125, viewingEdge); // shoe color
        }
        glPopMatrix(); // right leg pop

        glPopMatrix(); // torso pop

        if (action_mode[0]) // 차에 가까워지면
        {
            glPushMatrix(); // exclamation mark
            glTranslatef(0.35 * width, -0.5 * length, 0.9 * height);
            glColor3f(1.0f, 0.0f, 0.0f);
            drawCuboid(-0.15 * width, -0.15 * width, 0.1 * height, -0.05 * width, -0.05 * width, -0.05 * height, false);
            drawCuboid(-0.15 * width, -0.15 * width, -0.1 * height, -0.05 * width, -0.05 * width, -0.15 * height, false);
            drawCuboid(0.05 * width, -0.15 * width, 0.1 * height, 0.15 * width, -0.05 * width, -0.05 * height, false);
            drawCuboid(0.05 * width, -0.15 * width, -0.1 * height, 0.15 * width, -0.05 * width, -0.15 * height, false);
            glPopMatrix(); // exclamation mark pop
        }

        if (action_mode[1]) // 버섯을 먹으면
        {
            glPushMatrix(); // mushroom
            glTranslatef(-0.35 * width, -0.5 * length, 0.9 * height);
            glColor3f(0.8f, 0.2f, 0.7f);
            drawCuboid(-0.15 * width, -0.15 * width, 0.1 * height, -0.05 * width, -0.05 * width, -0.05 * height, false);
            drawCuboid(-0.15 * width, -0.15 * width, -0.1 * height, -0.05 * width, -0.05 * width, -0.15 * height, false);
            drawCuboid(0.05 * width, -0.15 * width, 0.1 * height, 0.15 * width, -0.05 * width, -0.05 * height, false);
            drawCuboid(0.05 * width, -0.15 * width, -0.1 * height, 0.15 * width, -0.05 * width, -0.15 * height, false);
            glPopMatrix(); // mushroom pop
        }

        glPopMatrix(); // player pop
    }
};

Player player(0.0f, -7.0 / 8, 0.0035f, 0.0035f);
vector<Player> players = { player };

/************************** Managers **************************/
class PlayerManager {

public:
    PlayerManager() {}

    static void updatePlayer(int value)
    {
        if (!gamePause)
        {
            for (int i = 0; i < players.size(); i++)
            {
                players[i].checkGetItem();

                players[i].move();
                if (players[i].checkInRiver())
                {
                    float transportaionMove = players[i].getTransportationMove();
                    if (transportaionMove == 0)
                    {
                        players[i].reset();
                    }
                    else
                        players[i].x += transportaionMove;
                }
                if (players[i].checkCrash() || gameReset)
                {
                    players[i].reset();
                    if (shoes.size() == 0)
                        shoes.push_back(shoe);
                    if (mushrooms.size() == 0)
                        mushrooms.push_back(mushroom);
                    gameReset = false;
                }

                if (players[i].checkInJumpPad() & !players[i].isJumping)
                {
                    players[i].isJumpPad = true;
                }
                else if (players[i].checkGameClear())
                {
                    gamePause = true;
                }

            }
        }

        glutPostRedisplay();
        glutTimerFunc(10, PlayerManager::updatePlayer, 0.1);
    }
};

PlayerManager playerManager;

class CarManager
{
public:

    void initCar()
    {
        float car_x = -1.0f;
        for (int i = 0; i < 2; i++) // lane0의 car들 생성
        {
            Car* newCarInLane0;
            newCarInLane0 = new Car(car_x, lane_pos[0], lane_speed[0], lane_dx[0], dist(mt) % 2);
            cars.push_back(newCarInLane0);
            car_x += 0.45f;
        }
        car_x = 1.0f;
        for (int i = 0; i < 2; i++) // lane1의 car들 생성
        {
            Car* newCarInLane1;
            newCarInLane1 = new Car(car_x, lane_pos[1], lane_speed[1], lane_dx[1], dist(mt) % 2);
            cars.push_back(newCarInLane1);
            car_x -= 0.6f;
        }
        for (int i = 0; i < 3; i++) // lane2의 car들 생성
        {
            Car* newCarInLane2;
            newCarInLane2 = new Car(car_x, lane_pos[2], lane_speed[2], lane_dx[2], dist(mt) % 2);
            cars.push_back(newCarInLane2);
            car_x += 0.45f;
        }
        for (int i = 0; i < 3; i++) // lane3의 car들 생성
        {
            Car* newCarInLane3;
            newCarInLane3 = new Car(car_x, lane_pos[3], lane_speed[3], lane_dx[3], dist(mt) % 2);
            cars.push_back(newCarInLane3);
            car_x -= 0.6f;
        }
    }

    static void moveCars(int value)
    {
        if (!gamePause)
        {
            for (int i = 0; i < cars.size(); i = i + 1)
            {
                Car* temp = cars[i];
                temp->move();

                if ((temp->x * temp->dx) > 5.0)
                    cars.erase(cars.begin() + i);
            }
            glutPostRedisplay();
            glutTimerFunc(10, CarManager::moveCars, 0.1);
        }
    }

    static void createCars(int value)
    {
        static Car* lastCar[4];

        if (!gamePause)
        {
            for (int i = 0; i < 4; i = i + 1)
            {
                int rand = dist(mt) % 5;

                if (rand == 1)
                {
                    Car* newCar;

                    if ((lastCar[i] == NULL) || ((lane_dx[i] * lastCar[i]->x) > -2.8))
                    {
                        newCar = new Car(-lane_dx[i] * 3.3, lane_pos[i], lane_speed[i], lane_dx[i], dist(mt) % 2);
                        cars.push_back(newCar);
                        lastCar[i] = newCar;
                    }
                }
            }
            glutPostRedisplay();
            glutTimerFunc(700, CarManager::createCars, 0.1);
        }
    }


    static void updateCar(int value)
    {
        moveCars(1);
        createCars(1);
    }

};

CarManager carManager;

class TreeManager
{
public:
    void initTree()
    {
        float tree_x = 0.2f;

        //Tree* newTreeInField0;
        //newTreeInField0 = new Tree(tree_x, 0);
        //trees.push_back(newTreeInField0);

        tree_x = 0.4f;
        for (int i = 2; i < 3; i++) // field1의 나무들 생성
        {
            Tree* newTreeInField1;
            newTreeInField1 = new Tree(tree_x, 1);
            trees.push_back(newTreeInField1);
            tree_x -= 0.4f + (0.3 * i);
        }
    }
};

TreeManager treeManager;

class BoatManager
{
public:
    void initBoat()
    {
        float x = -0.8f;
        float y = 5.0 / 8;
        float speed = 0.005;
        for (int i = 0; i < 3; i++)
        {
            Boat* boat;
            boat = new Boat(x, y, speed);
            boats.push_back(boat);
            x += 0.45 + i * 0.5;
        }
    }

    static void updateBoat(int value)
    {
        if (!gamePause)
        {
            for (int i = 0; i < boats.size(); i = i + 1)
            {
                Boat* temp = boats[i];
                temp->move();
            }
            glutPostRedisplay();
            glutTimerFunc(10, BoatManager::updateBoat, 0.1);
        }
    }
};

BoatManager boatManager;

void drawBackGround() {
    backGround.draw();
    footbridge.draw();
    drawStippleLine(0.25, 2);
    drawStippleLine(-0.5, 2);

    glPushMatrix(); // rainbow
    glTranslatef(0.0f, 3.1f, -0.3);
    glColor3f(1.0f, 0.0f, 0.0f); drawVerticalCircle(0.0f, 0.0f, 0.0f, 2.5f); // red 
    glColor3f(1.0f, 0.5f, 0.0f); drawVerticalCircle(0.0f, -0.01f, 0.0f, 2.25f); // orange
    glColor3f(1.0f, 1.0f, 0.0f); drawVerticalCircle(0.0f, -0.02f, 0.0f, 2.0f); // yellow
    glColor3f(0.0f, 1.0f, 0.0f); drawVerticalCircle(0.0f, -0.03f, 0.0f, 1.75f); // green
    glColor3f(0.0f, 0.0f, 1.0f); drawVerticalCircle(0.0f, -0.04f, 0.0f, 1.50f); // blue
    glColor3f(1.0f, 0.0f, 1.0f); drawVerticalCircle(0.0f, -0.05f, 0.0f, 1.25f); // purple
    glColor3f(0.5f, 0.9f, 1.0f); drawVerticalCircle(0.0f, -0.06f, 0.0f, 1.0f); // sky

    glPushMatrix(); // sun
    glTranslatef(0.0f, -0.7f, 0.0f);
    glRotatef(degree_sun, 0, 1, 0);
    if (degree_sun == 360) degree_sun = 0; else degree_sun++;
    glTranslatef(2.0f, 0.0f, 0.0f);
    glRotatef(90, 0, 1, 0);
    glColor3f(1.0f, 0.8f, 0.0f); drawVerticalCircle(0.0f, 0.0f, 0.0f, 0.4f); // sun

    if (players[0].checkGameClear())
    {
        drawVerticalRect(-0.3f, -0.01f, -0.1f, -0.1f, -0.01f, 0.1f, false, 0.0f, 0.0f, 0.0f);
        drawVerticalRect(0.1f, -0.01f, -0.1f, 0.3f, -0.01f, 0.1f, false, 0.0f, 0.0f, 0.0f);
        drawLine(-0.1f, -0.01f, 0.0f, 0.1f, -0.01f, 0.0f, 1);
        glColor3f(1.0f, 0.0f, 0.0f); // mouth
        glBegin(GL_POLYGON); glVertex3f(-0.1f, -0.01f, -0.15f); glVertex3f(0.1f, -0.01f, -0.15f); glVertex3f(0.0f, -0.01f, -0.3f); glEnd();
    }
    else
    {
        glColor3f(0.0f, 0.0f, 0.0f); // eyes
        drawVerticalCircle(-0.15f, -0.01f, 0.0f, 0.05f);
        drawVerticalCircle(0.15f, -0.01f, 0.0f, 0.05f);
        glColor3f(1.0f, 0.0f, 0.0f); // mouth
        drawVerticalCircle(0.0f, -0.01f, -0.15f, 0.06f);
    }
    glColor3f(1.0f, 0.7f, 0.0f);
    for (int i = 0; i < 360; i += 45)
    {
        glRotatef(i, 0, 1, 0);
        glBegin(GL_POLYGON); glVertex3f(-0.1f, 0.0f, 0.45f); glVertex3f(0.1f, 0.0f, 0.45f); glVertex3f(0.0f, 0.0f, 0.6f); glEnd();
    }

    glPopMatrix();
    glPopMatrix();

    //tree
    for (int i = 0; i < trees.size(); i = i + 1)
    {
        Tree* temp = trees[i];
        temp->draw();
    }

}

/************************** CALLBACK FUNCTIONS **************************/
void upKeys(int key, int x, int y) {
    if (!gamePause)
    {
        switch (key) {
        case GLUT_KEY_UP:
            players[0].moveKeyLogger[0] = 0;
            break;
        case GLUT_KEY_DOWN:
            players[0].moveKeyLogger[1] = 0;
            break;
        case GLUT_KEY_LEFT:
            players[0].moveKeyLogger[2] = 0;
            break;
        case GLUT_KEY_RIGHT:
            players[0].moveKeyLogger[3] = 0;
            break;
        }
        glutPostRedisplay();
    }
}

void downKeys(int key, int x, int y) {
    if (!gamePause)
    {
        switch (key) {
        case GLUT_KEY_UP:
            players[0].moveKeyLogger[0] = 1;
            break;
        case GLUT_KEY_DOWN:
            players[0].moveKeyLogger[1] = 1;
            break;
        case GLUT_KEY_LEFT:
            players[0].moveKeyLogger[2] = 1;
            break;
        case GLUT_KEY_RIGHT:
            players[0].moveKeyLogger[3] = 1;
            break;
        }
        glutPostRedisplay();
    }
}

void setLookAt()
{
    switch (viewingMode) {
    case 0: // default (3인칭 시점)
        ex = players[0].x; ey = players[0].y - 0.2f; ez = players[0].z + players[0].height;
        cx = ex; cy = ey + 0.2; cz = ez - 0.05;
        ux = 0; uy = 1; uz = 0.2;
        break;
    case 1: // 1인칭 시점
        ex = players[0].x; ey = players[0].y + 0.05f; ez = players[0].z + players[0].height;
        cx = ex; cy = ey + 0.2f; cz = ez - 0.05;
        ux = 0; uy = 1; uz = 0.1;
        break;
    case 2: // side view
        ex = 0.2f; ey = -1.0f; ez = 0.6f;
        cx = 0.1; cy = 0.2; cz = -0.2;
        ux = 0; uy = 0; uz = 1;
        break;

    }
    gluLookAt(ex, ey, ez, cx, cy, cz, ux, uy, uz);
}
void setRenderingMode()
{
    if (renderingMode)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
    else
        glDisable(GL_DEPTH_TEST);
}
void keyboards(unsigned char key, int x, int y)
{
    switch (key) {
    case 'P':
    case 'p':
        debugMode ^= 1;
        break;
    case 'Q':
    case 'q':
        gameReset = true;
        break;
    case 'V':
    case 'v':
        viewingMode = (viewingMode + 1) % 3;
        break;
    case 'e':
    case 'E':
        viewingEdge = !viewingEdge;
        break;
    case 'R':
    case 'r':
        renderingMode = !renderingMode;
        break;
    case 32:
        if (!players[0].isJumping)
        {
            players[0].speedZ = 0.01;
            players[0].isJumping = true;
        }
    }
    glutPostRedisplay();
}

void updateTimerFunctions()
{
    carManager.updateCar(1);
    boatManager.updateBoat(1);
    playerManager.updatePlayer(1);
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5f, 0.9f, 1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    setRenderingMode();
    setLookAt();

    drawBackGround();
    for (int i = 0; i < boats.size(); i = i + 1)
    {
        Boat* temp = boats[i];
        temp->draw();
    }

    for (int i = 0; i < cars.size(); i = i + 1)
    {
        Car* temp = cars[i];
        temp->draw();
    }

    for (int i = 0; i < jumpPads.size(); i++)
    {
        JumpPad* temp = jumpPads[i];
        temp->draw();
    }

    for (int i = 0; i < players.size(); i = i + 1)
    {
        Player* temp = &players[i];
        temp->draw();
    }

    for (int i = 0; i < shoes.size(); i = i + 1)
    {
        Shoe* temp = shoes[i];
        temp->draw();
    }
    for (int i = 0; i < mushrooms.size(); i = i + 1)
    {
        Mushroom* temp = mushrooms[i];
        temp->draw();
    }
    glutSwapBuffers();
}


void reshapeScene(int width, int height)
{
    float windowRatio = (float)width / height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120, windowRatio, 0.001, 5);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    setLookAt();
}

void main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Crossy Road");
    init_laneSpeed();
    carManager.initCar();
    treeManager.initTree();
    boatManager.initBoat();
    glutDisplayFunc(renderScene);
    updateTimerFunctions();
    glutSpecialFunc(downKeys);
    glutSpecialUpFunc(upKeys);
    glutReshapeFunc(reshapeScene);
    glutKeyboardFunc(keyboards);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glewInit();
    glutMainLoop();
    return;
}
#define _USE_MATH_DEFINES

#include <iostream>
#include <ctime>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <math.h>
#include <random>
#include "shape.h"
#include "shader.h"
#include "matrixStack.h"


using namespace std;

int windowWidth = 900;
int windowHeight = 600;
bool gamePause = false;
bool gameReset = false;
bool debugMode = false;
bool renderingMode = true;
bool viewingEdge = true;
int viewingMode = 0; // 0: Default(3인칭 시점), 1: 1인칭 시점, 2: Side view
int effectMode = 0;
float ex = 0; float ey = 0; float ez = 1;
float cx = 0; float cy = 0; float cz = 0;
float ux = 1; float uy = 1; float uz = 0;  // params for gluLookAt

float degree_sun = 0;
GLfloat grassZ = 0.06f;

//random variable
random_device rd;
mt19937 mt(rd());
uniform_int_distribution<int> dist(0, 1000);


//shader Loc
GLint colorLoc;
GLint projLoc;
GLint lookAtLoc;
GLint modelLoc;
GLint timeLoc;
GLint effectLoc;

//matrixStack
MatrixStack matrixStack;

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

    void create()
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
          cuboids[i]->draw(colorLoc,modelLoc,matrixStack.get(),viewingEdge);
    }

    vector<Cuboid*> cuboids;
};

Footbridge footbridge;


/************************** BACKGROUND **************************/
class BackGround
{
public:
    void create()
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

        Lines.push_back(new StripLine(-3, 0.25, 0.0, 3, 0.25, 0, 1, 1, 1, 0.02));
        Lines.push_back(new StripLine(-3, -0.5, 0.0, 3, -0.5, 0, 1, 1, 1, 0.02));
    }

    vector<Cuboid*> cuboids;
    vector<StripLine*> Lines;

    void draw()
    {
        for (int i = 0; i < cuboids.size(); i++)
            cuboids[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        for (int i = 0; i < Lines.size(); i++)
            Lines[i]->draw(colorLoc, modelLoc, matrixStack.get(), false);
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

    Cylinder* cylinder;
    Cuboid* cuboid;
    Rect* rect;

    void create()
    {
        cylinder = new Cylinder(0, 0, 0, radius, radius, 0.02, 0.0, 0.0, 0.0, 0);
        cuboid = new Cuboid(-0.7 * radius, -0.2 * radius, -0.012, 0.7 * radius, 0.2 * radius, 0.012, 1.0, 1.0, 1.0, 0);
        rect = new Rect(-0.8 * radius, -0.2 * radius, 0, 0.8 * radius, 0.2 * radius, 0, 1.0f, 1.0f, 1.0f, 0, 0);
    }

    void draw(float x, float y, float z) {
        if (!gamePause)
            degree += angularSpeed;

        if (degree > 360)
            degree -= 360;
        matrixStack.pushMatrix();
        matrixStack.translatef(x, y, z);
        matrixStack.rotatef(90, 1, 0, 0);
        matrixStack.rotatef(degree, 0.0, 0.0, 1.0);
        cylinder->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);

        rect->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        cuboid->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        matrixStack.popMatrix();
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
    vector<Cuboid*> body;
    vector<Cuboid*> crain;

    void create()
    {
        if (type) // truck
        {
            body.push_back(new Cuboid(-0.5 * width, -0.5 * length, height * 0.3, 0.5 * width, 0.5 * length, height * 0.7, 0.2f, 0.2f, 0.7f, 0));
            body.push_back(new Cuboid(-0.5 * width, -0.5 * length, height * 0.7, 0, 0.5 * length, height * 1.1, 0.2f, 0.2f, 0.7f, 0));

            crain.push_back(new Cuboid(0, -0.5 * length, height * 0.7, 0.5 * width, 0.5 * length, height * 0.9, 0.2f, 0.2f, 0.7f, 0));
        }
        else // car
        {
            body.push_back(new Cuboid(-0.5 * width, -0.5 * length, height * 0.3, 0.5 * width, 0.5 * length, height * 0.7, 1.0f, 0.7f, 0.2f, 0));
            body.push_back(new Cuboid(-0.3 * width, -0.5 * length, height * 0.7, 0.3 * width, 0.5 * length, height * 1.1, 1.0f, 0.7f, 0.2f, 0));
        }
        for (int i = 0; i < 4; i++)
            wheels[i].create();
    }

    void move()
    {
        x += dx * speed;
    }

    void draw()
    {
        matrixStack.pushMatrix();
        matrixStack.translatef(x, y, z);

        if (dx == 1) // right direction move
            matrixStack.rotatef(180, 0, 0, 1);

        for (int i = 0; i < body.size(); i++)
            body[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);

        if (type) // truck
        {

            if (!gamePause)
                crain_degree += ds;

            matrixStack.pushMatrix();
            matrixStack.rotatef(-crain_degree, 0, 1, 0);
            if (crain_degree > 30 || crain_degree < 0)
                ds *= -1;

            for (int i = 0; i < crain.size(); i++)
                crain[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);

            matrixStack.popMatrix();
        }

        int wheelCount = wheels.size();
        //draw wheel
        for (int i = 0; i < wheelCount; i += 2)
        {
            float wheelX = -(width * 1.4) / 2 + (width * 1.4) / (wheelCount / 2 + 1) * (i / 2 + 1);
            float wheelY = -length * 0.5;
            matrixStack.pushMatrix();
            wheels[i].draw(wheelX, wheelY, height * 0.3);
            wheels[i + 1].draw(wheelX, -wheelY, height * 0.3);
            matrixStack.popMatrix();
        }

        matrixStack.popMatrix();
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
    vector<Cuboid*> cuboids;

    void move()
    {
        x += dx * speed;
        if ((x <= -1.5f) || (x >= 1.5f))
        {
            x *= -1;
            x += dx * speed;
        }
    }

    void create()
    {
        cuboids.push_back(new Cuboid(-width * 0.5f, -width * 0.3f, -height * 0.5, width * 0.4f, width * 0.1f, -height * 0.2f, 0.5f, 0.1f, 0.0f,0));
    }

    void draw()
    {
        if (!gamePause)
            degree -= ds;
        if (degree > 30 || degree < -30)
            ds *= -1;
        matrixStack.pushMatrix(); // boat
        matrixStack.translatef(x, y, z);
        matrixStack.rotatef(degree * 0.15, 0, 1, 0);
        
        for (int i = 0; i < cuboids.size(); i++)
            cuboids[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        
        matrixStack.popMatrix();
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

    void create()
    {
        GLfloat wood_x_half = 0.1f * width;
        GLfloat wood_y_half = 0.1f * length;
        GLfloat lowerleaf_x_half = 0.5f * width;
        GLfloat lowerleaf_y_half = 0.5f * length;
        GLfloat upperleaf_x_half = 0.3f * width;
        GLfloat upperleaf_y_half = 0.3f * length;
        cuboids.push_back(new Cuboid(x - wood_x_half, y - wood_y_half, grassZ, x + wood_x_half, y + wood_y_half, grassZ + height / 2, 0.8f, 0.4f, 0.4f, 0));
        cuboids.push_back(new Cuboid(x - lowerleaf_x_half, y - lowerleaf_y_half, grassZ + height / 2, x + lowerleaf_x_half, y + lowerleaf_y_half, grassZ + height * 0.75, 0.2f, 0.7f, 0.0f, 0));
        cuboids.push_back(new Cuboid(x - upperleaf_x_half, y - upperleaf_y_half, grassZ + height * 0.75, x + upperleaf_x_half, y + upperleaf_y_half, grassZ + height, 0.2f, 0.7f, 0.0f, 0));
    }
    void draw()
    {
        for (int i = 0; i < cuboids.size(); i++)
            cuboids[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
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
}

vector<Tree*> trees;

/************************** SHOE **************************/
class Shoe {
public:
    Shoe(float x_input, float y_input);
    float height = 0.25 * 0.3;
    float length = height * 2;
    float width = height / 3;
    float x, y, z = grassZ + height * 0.5;

    vector<Cuboid*> cuboids;
    vector<Cylinder*> cylinders;

    void create()
    {
        cylinders.push_back(new Cylinder(x + 0.2 * length, y, z - 0.25 * height, 0.29 * length, 0.49 * width, 0.5 * height, 1.0f, 1.0f, 1.0f, 0)); // 앞코
        cylinders.push_back(new Cylinder(x + 0.2 * length, y, z - 0.4 * height, 0.3 * length, 0.5 * width, 0.2 * height, 1.0f, 1.0f, 1.0f, 0)); // 앞코 밑창
        cylinders.push_back(new Cylinder(x - 0.3 * length, y, z, 0.19 * length, 0.49 * width, height, 1.0f, 0.1f, 0.5f, 0)); // 뒷부분
        cylinders.push_back(new Cylinder(x - 0.3 * length, y, z - 0.4 * height, 0.2 * length, 0.5 * width, 0.2 * height, 1.0f, 1.0f, 1.0f, 0)); // 뒷부분 밑창
        cylinders.push_back(new Cylinder(x - 0.3 * length, y, z + 0.42 * height, 0.2 * length, 0.5 * width, 0.2 * height, 1.0f, 1.0f, 1.0f, 0)); // 뒷부분 위
        cuboids.push_back(new Cuboid(x - 0.3 * length, y - 0.49 * width, z - 0.5 * height, x + 0.2 * length, y + 0.49 * width, z + 0.05 * height, 1.0f, 0.1f, 0.5f, 0)); // 신발 몸체
        cuboids.push_back(new Cuboid(x - 0.3 * length, y - 0.5 * width, z - 0.5 * height, x + 0.2 * length, y + 0.5 * width, z - 0.3 * height, 1.0f, 1.0f, 1.0f, 0)); // 신발 몸체 밑창
    }

    void move()
    {

    }
    void draw()
    {
        for (int i = 0; i < cylinders.size(); i++)
            cylinders[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        for (int i = 0; i < cuboids.size(); i++)
            cuboids[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
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

    vector<Cylinder*> cylinders;
    Cuboid* cuboid;

    void create()
    {
        cuboid = new Cuboid(-width * 0.1, -length * 0.1, 0, width * 0.1, length * 0.1, height * 0.5, 1.0f, 0.8f, 1.0f, 0);
        cylinders.push_back(new Cylinder(0, 0, height * 0.6, width * 0.5, width * 0.5, height * 0.4, 0.8f, 0.2f, 0.7f, 0));
        cylinders.push_back(new Cylinder(0, 0, height * 0.8, width * 0.3, width * 0.3, height * 0.3, 0.8f, 0.2f, 0.7f, 0));
    }

    void move()
    {
    }

    void draw()
    {
        matrixStack.pushMatrix();
        matrixStack.translatef(x, y, grassZ);
        cuboid->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        for (int i = 0; i < cylinders.size(); i++)
            cylinders[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        matrixStack.popMatrix();
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

    Cuboid* cuboid;
    Triangle* triangle;
    Rect* rect;

    void create()
    {
        cuboid = new Cuboid(-0.5 * width, -0.5 * length, 0, 0.5 * width, 0.5 * length, height, 0.65, 0.3, 0, 0);
        triangle = new Triangle(-0.25 * width, 0.1 * length, 0, 0.25 * width, 0.1 * length, 0, 0, 0.3 * length, 0, 1.0f, 1.0f, 0.0f, 0);
        rect = new Rect(-0.1 * width, -0.3 * length, 0.0, 0.1 * width, 0.1 * length, 0.0, 1.0f, 1.0f, 0.0f, 0, 0);
    }

    void draw()
    {
        matrixStack.pushMatrix();
        matrixStack.translatef(x, y, z);
        cuboid->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);

        matrixStack.translatef(0, 0, height + 0.001);
        triangle->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        rect->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);

        matrixStack.popMatrix();
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

    Cuboid* torso;
    Cuboid* head;
    vector<Cuboid*> rightArm;
    vector<Cuboid*> leftArm;
    Cuboid* rightLeg;
    Cuboid* rightShoe;
    Cuboid* leftLeg;
    Cuboid* leftShoe;
    vector<Cuboid*> exclamationMark1;
    vector<Cuboid*> exclamationMark2;
    vector<Rect*> hair;
    vector<Rect*> face;

    void create()
    {
        exclamationMark1.push_back(new Cuboid(0.2 * width, -0.65 * width, height, 0.3 * width, -0.55 * width, 0.85 * height, 1.0f, 0.0f, 0.0f, 0));
        exclamationMark1.push_back(new Cuboid(0.2 * width, -0.65 * width, 0.8 * height, 0.3 * width, -0.55 * width, 0.75 * height, 1.0f, 0.0f, 0.0f, 0));
        exclamationMark1.push_back(new Cuboid(0.4 * width, -0.65 * width, height, 0.5 * width, -0.55 * width, 0.85 * height, 1.0f, 0.0f, 0.0f, 0));
        exclamationMark1.push_back(new Cuboid(0.4 * width, -0.65 * width, 0.8 * height, 0.5 * width, -0.55 * width, 0.75 * height, 1.0f, 0.0f, 0.0f, 0));

        exclamationMark2.push_back(new Cuboid(-0.5 * width, -0.65 * width, height, -0.4 * width, -0.55 * width, 0.85 * height, 0.8f, 0.2f, 0.7f, 0));
        exclamationMark2.push_back(new Cuboid(-0.5 * width, -0.65 * width, 0.8 * height, -0.4 * width, -0.55 * width, 0.75 * height, 0.8f, 0.2f, 0.7f, 0));
        exclamationMark2.push_back(new Cuboid(-0.3 * width, -0.65 * width, height, -0.2 * width, -0.55 * width, 0.85 * height, 0.8f, 0.2f, 0.7f, 0));
        exclamationMark2.push_back(new Cuboid(-0.3 * width, -0.65 * width, 0.8 * height, -0.2 * width, -0.55 * width, 0.75 * height, 0.8f, 0.2f, 0.7f, 0));

        torso = new Cuboid(-width * 0.25, -length * 0.25, -height * 0.125, width * 0.25, length * 0.25, height * 0.25, 0.3f, 0.8f, 0.7f, 0);
        head = new Cuboid(-width * 0.25, -length * 0.5, -height * 0.125, width * 0.25, length * 0.5, height * 0.125, 1.0f, 0.9f, 0.7f, 0);

        face.push_back(new Rect(-width * 0.25 - 0.0001, 0, 0, -width * 0.125, 0, height * 0.125 + 0.0001, 0.25, 0.2, 0.1, 1, 0)); // left fringe
        face.push_back(new Rect(-width * 0.125, 0, height * 0.0625, width * 0.125, 0, height * 0.125 + 0.0001, 0.25, 0.2, 0.1, 1, 0)); // fringe
        face.push_back(new Rect(width * 0.125, 0, 0, width * 0.25 + 0.0001, 0, height * 0.125 + 0.0001, 0.25, 0.2, 0.1, 1, 0)); // right fringe
        face.push_back(new Rect(-width * 0.1875, 0, -height * 0.03125, -width * 0.125, 0, 0, 1.0f, 1.0f, 1.0f, 1, 0)); // left eye white
        face.push_back(new Rect(-width * 0.125, 0, -height * 0.03125, -width * 0.0625, 0, 0, 0.2f, 0.2f, 1.0f, 1, 0)); // left eye
        face.push_back(new Rect(width * 0.125, 0, -height * 0.03125, width * 0.1875, 0, 0, 1.0f, 1.0f, 1.0f, 1, 0)); // right eye white
        face.push_back(new Rect(width * 0.0625, 0, -height * 0.03125, width * 0.125, 0, 0, 0.2f, 0.2f, 1.0f, 1, 0)); // right eye
        face.push_back(new Rect(-width * 0.0625, 0, -height * 0.09375, width * 0.0625, 0, -height * 0.0625, 0.5f, 0.1f, 0.1f, 1, 0)); // mouth

        hair.push_back(new Rect(width * 0.25 + 0.0001, -length * 0.5 - 0.0001, 0, width * 0.25 + 0.0001, length * 0.5 + 0.0001, height * 0.125 + 0.0001, 0.25, 0.2, 0.1, 1, 0)); // left hair 
        hair.push_back(new Rect(-width * 0.25 - 0.0001, -length * 0.5 - 0.0001, 0, -width * 0.25 - 0.0001, length * 0.5 + 0.0001, height * 0.125 + 0.0001, 0.25, 0.2, 0.1, 1, 0)); // right hair
        hair.push_back(new Rect(-width * 0.25 - 0.0001, length * 0.5 + 0.0001, -height * 0.0625 - 0.0001, width * 0.25 + 0.0001, length * 0.5 + 0.0001, height * 0.125, 0.25, 0.2, 0.1, 1, 0)); // back hair
        hair.push_back(new Rect(-width * 0.25 - 0.0001, -length * 0.5 - 0.0001, height * 0.125 + 0.0001, width * 0.25 + 0.0001, length * 0.5 + 0.0001, height * 0.125 + 0.0001, 0.25, 0.2, 0.1, 0, 0)); // upper hair

        rightArm.push_back(new Cuboid(0, -length * 0.25, -height * 0.0625, width * 0.25, length * 0.25, height * 0.0625, 0.3f, 0.8f, 0.7f, 0));
        rightArm.push_back(new Cuboid(0, -length * 0.25, -height * 0.3125, width * 0.25, length * 0.25, -height * 0.0625, 1.0f, 0.9f, 0.7f, 0));

        leftArm.push_back(new Cuboid(-width * 0.25, -length * 0.25, -height * 0.0625, 0, length * 0.25, height * 0.0625, 0.3f, 0.8f, 0.7f, 0));
        leftArm.push_back(new Cuboid(-width * 0.25, -length * 0.25, -height * 0.3125, 0, length * 0.25, -height * 0.0625, 1.0f, 0.9f, 0.7f, 0));

        rightLeg = new Cuboid(-width * 0.125, -length * 0.25, -height * 0.375, width * 0.125, length * 0.25, 0, 0.2f, 0.1f, 0.7f, 0);
        leftLeg = new Cuboid(-width * 0.125, -length * 0.25, -height * 0.375, width * 0.125, length * 0.25, 0, 0.2f, 0.1f, 0.7f, 0);

        rightShoe = new Cuboid(-width * 0.125 - 0.001, -length * 0.25 - 0.001, -height * 0.375, width * 0.125 + 0.001, length * 0.25 + 0.001, -height * 0.3125, 1.0f, 0.1f, 0.5f, 0);
        leftShoe = new Cuboid(-width * 0.125 - 0.001, -length * 0.25 - 0.001, -height * 0.375, width * 0.125 + 0.001, length * 0.25 + 0.001, -height * 0.3125, 1.0f, 0.1f, 0.5f, 0);
    }

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
            if (((shoes[i]->length + width) * 0.5f > distance_x) && ((shoes[i]->width + length) * 0.5f > distance_y) && ((shoes[i]->height + length) * 0.5f > distance_z))
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
            float  distance_z = abs(mushrooms[i]->z - z);
            if (((mushrooms[i]->width + width) * 0.5f > distance_x) && ((mushrooms[i]->length + length) * 0.5f > distance_y) && ((mushrooms[i]->height + height) * 0.5f > distance_z))
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
            if ((jumpPads[i]->width * 0.4f > distance_x) && (jumpPads[i]->length * 0.4f > distance_y))
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
        matrixStack.pushMatrix(); // player
        matrixStack.translatef(x, y, z);

        if (dx != 0 || dy != 0)
        {
            float direction = acos(-(float)dy / sqrt(pow(dx, 2) + pow(dy, 2))) * 180 / M_PI;
            if (dx < 0)
                direction *= -1;
            matrixStack.rotatef(direction, 0, 0, 1);
        }
        if (y > 0.5 && y <= 0.75 && (z-0.5*height) < 0 && !debugMode && !isJumping) matrixStack.rotatef(boats[0]->degree * 0.15, 0, 1, 0);

        matrixStack.pushMatrix(); // torso
        matrixStack.translatef(0, 0, height * 0.5);
        torso->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);

        matrixStack.pushMatrix(); // head
        matrixStack.translatef(0, 0, height * 0.375);

        if (isMoving)
        {
            matrixStack.rotatef(degree_arms_legs * 2, 0, 0, 1);
            degree_arms_legs -= ds;
            if (degree_arms_legs > 25 || degree_arms_legs < -25)
                ds *= -1;
        }
        head->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);

        matrixStack.pushMatrix(); // face
        matrixStack.translatef(0, -length * 0.5 - 0.0001, 0);
        for (int i = 0; i < face.size(); i++)
            face[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        matrixStack.popMatrix(); // face pop

        for (int i = 0; i < hair.size(); i++)
            hair[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        matrixStack.popMatrix(); // head pop

        matrixStack.pushMatrix(); // right arm
        matrixStack.translatef(width * 0.25, 0, height * 0.1875);
        if (isMoving && !checkGameClear()) matrixStack.rotatef(degree_arms_legs, 1, 0, 0);
        if (checkGameClear()) matrixStack.rotatef(180, 1, 0, 0);
        for (int i = 0; i < rightArm.size(); i++)
            rightArm[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        matrixStack.popMatrix(); // right arm pop

        matrixStack.pushMatrix(); // left arm
        matrixStack.translatef(-width * 0.25, 0, height * 0.1875);
        if (isMoving && !checkGameClear()) matrixStack.rotatef(-degree_arms_legs, 1, 0, 0);
        if (checkGameClear()) matrixStack.rotatef(180, 1, 0, 0);
        for (int i = 0; i < leftArm.size(); i++)
            leftArm[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        matrixStack.popMatrix(); // left arm pop

        matrixStack.pushMatrix(); // right leg
        matrixStack.translatef(width * 0.125, 0, -height * 0.125);
        if (isMoving && !checkGameClear()) matrixStack.rotatef(-degree_arms_legs, 1, 0, 0);
        rightLeg->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        if (didGetShoe) rightShoe->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        matrixStack.popMatrix(); // right leg pop

        matrixStack.pushMatrix(); // left leg
        matrixStack.translatef(-width * 0.125, 0, -height * 0.125);
        if (isMoving && !checkGameClear()) matrixStack.rotatef(degree_arms_legs, 1, 0, 0);
        leftLeg->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        if (didGetShoe) leftShoe->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);
        matrixStack.popMatrix(); // left leg pop

        matrixStack.popMatrix(); // torso pop

        if (action_mode[0]) // 차에 가까워지면
            for (int i = 0; i < exclamationMark1.size(); i++)
                exclamationMark1[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);

        if (action_mode[1]) // 버섯을 먹으면
            for (int i = 0; i < exclamationMark2.size(); i++)
                exclamationMark2[i]->draw(colorLoc, modelLoc, matrixStack.get(), viewingEdge);

        matrixStack.popMatrix(); // player pop
    }
};

Player* player = new Player(0.0f, -7.0 / 8, 0.0035f, 0.003f);
vector<Player*> players = { player };

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
                players[i]->checkGetItem();

                players[i]->move();
                if (players[i]->checkInRiver())
                {
                    float transportaionMove = players[i]->getTransportationMove();
                    if (transportaionMove == 0)
                    {
                        players[i]->reset();
                    }
                    else
                        players[i]->x += transportaionMove;
                }
                if (players[i]->checkCrash() || gameReset)
                {
                    players[i]->reset();
                    if (shoes.size() == 0)
                        shoes.push_back(shoe);
                    if (mushrooms.size() == 0)
                        mushrooms.push_back(mushroom);
                    gameReset = false;
                }

                if (players[i]->checkInJumpPad() & !players[i]->isJumping)
                {
                    players[i]->isJumpPad = true;
                }
                else if (players[i]->checkGameClear())
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
            newCarInLane0->create();
            cars.push_back(newCarInLane0);
            car_x += 0.45f;
        }
        car_x = 1.0f;
        for (int i = 0; i < 2; i++) // lane1의 car들 생성
        {
            Car* newCarInLane1;
            newCarInLane1 = new Car(car_x, lane_pos[1], lane_speed[1], lane_dx[1], dist(mt) % 2);
            newCarInLane1->create();
            cars.push_back(newCarInLane1);
            car_x -= 0.6f;
        }
        for (int i = 0; i < 3; i++) // lane2의 car들 생성
        {
            Car* newCarInLane2;
            newCarInLane2 = new Car(car_x, lane_pos[2], lane_speed[2], lane_dx[2], dist(mt) % 2);
            newCarInLane2->create();
            cars.push_back(newCarInLane2);
            car_x += 0.45f;
        }
        for (int i = 0; i < 3; i++) // lane3의 car들 생성
        {
            Car* newCarInLane3;
            newCarInLane3 = new Car(car_x, lane_pos[3], lane_speed[3], lane_dx[3], dist(mt) % 2);
            newCarInLane3->create();
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
                        newCar->create();
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
            newTreeInField1->create();
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
            boat->create();
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
            players[0]->moveKeyLogger[0] = 0;
            break;
        case GLUT_KEY_DOWN:
            players[0]->moveKeyLogger[1] = 0;
            break;
        case GLUT_KEY_LEFT:
            players[0]->moveKeyLogger[2] = 0;
            break;
        case GLUT_KEY_RIGHT:
            players[0]->moveKeyLogger[3] = 0;
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
            players[0]->moveKeyLogger[0] = 1;
            break;
        case GLUT_KEY_DOWN:
            players[0]->moveKeyLogger[1] = 1;
            break;
        case GLUT_KEY_LEFT:
            players[0]->moveKeyLogger[2] = 1;
            break;
        case GLUT_KEY_RIGHT:
            players[0]->moveKeyLogger[3] = 1;
            break;
        }
        glutPostRedisplay();
    }
}

void setLookAt()
{
    switch (viewingMode) {
    case 0: // default (3인칭 시점)
        ex = players[0]->x; ey = players[0]->y - 0.2f; ez = players[0]->z + players[0]->height;
        cx = ex; cy = ey + 0.2; cz = ez - 0.05;
        ux = 0; uy = 1; uz = 0.2;
        break;
    case 1: // 1인칭 시점
        ex = players[0]->x; ey = players[0]->y + 0.05f; ez = players[0]->z + players[0]->height;
        cx = ex; cy = ey + 0.2f; cz = ez - 0.05;
        ux = 0; uy = 1; uz = 0.1;
        break;
    case 2: // side view
        ex = 0.2f; ey = -1.0f; ez = 0.6f;
        cx = 0.1; cy = 0.2; cz = -0.2;
        ux = 0; uy = 0; uz = 1;
        break;

    }

    mat4 look;
    look = lookAt(vec3(ex, ey, ez), vec3(cx, cy, cz), vec3(ux, uy, uz));
    matrixStack.dot(look);
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
        effectMode = 0;
        glUniform1i(effectLoc, effectMode);
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
    case 'A':
    case 'a':
        effectMode = 1;
        glUniform1i(effectLoc, effectMode);
        break;
    case 'S':
    case 's':
        effectMode = 2;
        glUniform1i(effectLoc, effectMode);
        break;
    case 'D':
    case 'd':
        effectMode = 3;
        glUniform1i(effectLoc, effectMode);
        break;
    case 'F':
    case 'f':
        effectMode = 4;
        glUniform1i(effectLoc, effectMode);
        break;
    case 32:
        if (!players[0]->isJumping)
        {
            players[0]->speedZ = 0.01;
            players[0]->isJumping = true;
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

void setTime()
{
    float currentTime = clock()/1000.0;
    glUniform1f(timeLoc, currentTime);
}

void initObects()
{
    init_laneSpeed();
    carManager.initCar();
    treeManager.initTree();
    boatManager.initBoat();
    footbridge.create();
    backGround.create();
    for (int i = 0; i < mushrooms.size(); i++)
        mushrooms[i]->create();
    for (int i = 0; i < jumpPads.size(); i++)
        jumpPads[i]->create();
    for (int i = 0; i < players.size(); i++)
        players[i]->create();
    for (int i = 0; i < shoes.size(); i++)
        shoes[i]->create();
}
void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5f, 0.9f, 1.0f, 1.0f);
    setRenderingMode();
    setTime();

    matrixStack.loadIdentity();
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
        Player* temp = players[i];
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
    
    mat4 proj;
    proj = perspective(radians(120.0f), windowRatio, 0.001f, 5.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(proj));
}

void main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Crossy Road");

    glutDisplayFunc(renderScene);
    glutSpecialFunc(downKeys);
    glutSpecialUpFunc(upKeys);
    glutReshapeFunc(reshapeScene);
    glutKeyboardFunc(keyboards);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glewInit();

    //set shader
    Shader shader("vShader.glsl", "fShader.glsl");
    shader.run();
    colorLoc = glGetUniformLocation(shader.program, "color");
    projLoc = glGetUniformLocation(shader.program, "projection");
    modelLoc = glGetUniformLocation(shader.program, "modelView");
    timeLoc = glGetUniformLocation(shader.program, "time");
    effectLoc = glGetUniformLocation(shader.program, "effectMode");
    glUniform1i(effectLoc, effectMode);

    initObects();
    updateTimerFunctions();

    glutMainLoop();
    return;
}
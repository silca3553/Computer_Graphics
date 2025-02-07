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
float windowRatio = float(windowHeight) / windowWidth;
bool gamePause = false;
bool gameReset = false;
bool debugMode = false;

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

/************************** LANE **************************/

float lane_pos[4] = { -windowRatio / 8 * 5,-windowRatio / 8 * 3,windowRatio / 8 * 1, windowRatio / 8 * 3 };
int lane_dx[4] = { -1,1,-1,1 };
float lane_speed[4];

void init_laneSpeed()
{
    for (int i = 0; i < 2; i++)
        lane_speed[i] = 0.001f * (dist(mt) % 3 + 1);
    for (int i = 2; i < 4; i++)
        lane_speed[i] = 0.001f * (dist(mt) % 4 + 3);
}

float field0_y = -windowRatio / 8 * 7;
float field1_y = -windowRatio / 8 * 1;

/************************** WHEEL **************************/
class Wheel
{
public:
    Wheel(float _radius, float _angularSpeed)
        :radius(_radius), angularSpeed(_angularSpeed) {};

    float radius, angularSpeed;
    int degree = 0;

    void draw(float x, float y) {
        if (!gamePause)
            degree += angularSpeed;

        if (degree > 360)
            degree -= 360;
        glPushMatrix();
        glTranslatef(x, y, 0);
        glRotatef(degree, 0.0, 0.0, 1.0);
        glColor3f(0.0, 0.0, 0.0);

        glBegin(GL_POLYGON);
        for (int i = 0; i < 200; i++) {
            float theta = 2.0f * M_PI * float(i) / float(100);
            float x = radius * cos(theta);
            float y = radius * sin(theta);
            glVertex2f(x, y);
        }
        glEnd();

        glColor3f(1.0, 1.0, 1.0);
        glRectf(-0.8 * radius, -0.2 * radius, 0.8 * radius, 0.2 * radius);
        glPopMatrix();
    }
};

/************************** CAR **************************/
class Car {
public:
    Car(float _x, float _y, float _speed, int _dx, int _type);
    float x, y, speed;
    int dx; // -1: left, 1: right
    int type; // 0: 승용차, 1: 트럭
    int crain_degree = 0;
    bool isOpening = 1; // 0: 상판 닫히는 중, 1: 상판 열리는 중
    float car_height = windowRatio / 4 * 0.8;
    float car_width = car_height * 1.7;
    int ds = 1;
    vector<Wheel> wheels;

    void move()
    {
        x += dx*speed;
    }

    void draw()
    {
        glPushMatrix();
        glTranslatef(x, y, 0);
        if (dx == 1) // left direction move
            glRotatef(180, 0, 1, 0);

        if (type) // truck
        {
            glColor3f(0.2f, 0.2f, 0.7f);
            glRectf(-0.5 * car_width, -0.3 * car_height, 0.5 * car_width, 0); // 차 몸통
            glColor3f(0.2f, 0.2f, 0.7f);
            glRectf(-0.5 * car_width, 0, 0, 0.5 * car_height); // 트럭 머리
            glColor3f(0.7f, 0.9f, 0.95f);
            glRectf(-0.3 * car_width, 0.1 * car_height, -0.5 * car_width, 0.4 * car_height); // 창문
            glColor3f(0.1f, 0.1f, 0.1f);

            if (!gamePause)
                crain_degree += ds;

            glPushMatrix();
            glRotatef(crain_degree, 0, 0, 1);
            if (crain_degree > 45 || crain_degree < 0)
                ds *= -1;

            glRectf(0, -0.05 * car_height, 0.5 * car_width, 0.05 * car_height); // crain
            glPopMatrix();

        }
        else // car
        {
            glColor3f(1.0f, 0.7f, 0.2f);
            glRectf(-0.5 * car_width, -0.3 * car_height, 0.5 * car_width, 0.15 * car_height);
            glRectf(-0.3 * car_width, 0.15 * car_height, 0.3 * car_width, 0.5 * car_height);
            glColor3f(0.7f, 0.9f, 0.95f);
            glRectf(-0.2 * car_width, 0.15 * car_height, -0.05 * car_width, 0.45 * car_height); // 창문
            glRectf(0.2 * car_width, 0.15 * car_height, 0.05 * car_width, 0.45 * car_height); // 창문

            glColor3f(0.8f, 0.8f, 0.8f);
            glRectf(-0.42 * car_width, -0.1 * car_height, -0.5 * car_width, 0); // 전조등
            glColor3f(1.0f, 0.3f, 0.3f);
            glRectf(+0.42 * car_width, -0.1 * car_height, 0.5 * car_width, 0); // 후미등
        }

        int wheelsSize = wheels.size();
        //draw wheel
        for (int i = 1; i <= wheelsSize; i++)
        {
            float wheelCenterX = -(car_width * 1.4) / 2 + (car_width * 1.4) / (wheelsSize + 1) * i;
            float wheelCenterY = -car_height / 4;
            wheels[i - 1].draw(wheelCenterX, wheelCenterY);
        }

        glPopMatrix();
    }

};

Car::Car(float _x, float _y, float _speed, int _dx,int _type)
{
    x = _x;
    y = _y;
    dx = _dx;
    speed = _speed;
    type = _type;

    for (int i = 0; i < 2; i++)
    {
        Wheel* wheel;
        wheel = new Wheel(car_height *0.2, speed / 0.04 * 180 / M_PI);
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
    int dx = -1;
    float height = windowRatio / 4 * 0.8;
    float width = height * 2;
    int degree = 30; // paddle 각도 -30~30
    int ds = 1;

    void move()
    {
        x += dx * speed;
        if ((x < -1.15f) || (x > 1.15f))
        {
            x *= -1;
            x += dx * speed;
        }
    }

    void draw()
    {
        glPushMatrix(); // boat
        glTranslatef(x, y, 0);
        glColor3f(0.6f, 0.2f, 0.0f);
        glBegin(GL_POLYGON);
        glVertex3f(-width * 0.5f, 0, 0);
        glVertex3f(-width * 0.35f, height * 0.3f, 0);
        glVertex3f(-width * 0.1f, height * 0.4f, 0);
        glVertex3f(width * 0.1f, height * 0.4f, 0);
        glVertex3f(width * 0.48f, height * 0.3f, 0);
        glVertex3f(width * 0.5f, 0, 0);
        glVertex3f(width * 0.48f, -height * 0.3f, 0);
        glVertex3f(width * 0.1f, -height * 0.4f, 0);
        glVertex3f(-width * 0.1f, -height * 0.4f, 0);
        glVertex3f(-width * 0.35f, -height * 0.3f, 0);
        glEnd();

        glPushMatrix(); // upper paddle
        glTranslatef(0, height * 0.4f, 0);
        glRotatef(degree, 0, 0, 1);
        if (!gamePause)
            degree -= ds;
        if (degree > 30 || degree < -30)
            ds *= -1;
        glColor3f(0.4f, 0.0f, 0.0f);
        glBegin(GL_POLYGON); // handle
        glVertex3f(-height * 0.03f, -height * 0.3f, 0);
        glVertex3f(-height * 0.03f, height * 0.05f, 0);
        glVertex3f(height * 0.03f, height * 0.05f, 0);
        glVertex3f(height * 0.03f, -height * 0.3f, 0);
        glEnd();

        glBegin(GL_POLYGON); // blade
        glVertex3f(-height * 0.03f, height * 0.05f, 0);
        glVertex3f(-height * 0.08f, height * 0.1f, 0);
        glVertex3f(-height * 0.08f, height * 0.2f, 0);
        glVertex3f(height * 0.08f, height * 0.2f, 0);
        glVertex3f(height * 0.08f, height * 0.1f, 0);
        glVertex3f(height * 0.03f, height * 0.05f, 0);
        glEnd();
        glPopMatrix();

        glPushMatrix(); // lower paddle
        glTranslatef(0, -height * 0.4f, 0);
        glRotatef(-degree, 0, 0, 1);
        if (!gamePause)
            degree -= ds;
        if (degree > 30 || degree < -30)
            ds *= -1;
        glColor3f(0.4f, 0.0f, 0.0f);
        glBegin(GL_POLYGON); // handle
        glVertex3f(-height * 0.03f, height * 0.3f, 0);
        glVertex3f(-height * 0.03f, -height * 0.05f, 0);
        glVertex3f(height * 0.03f, -height * 0.05f, 0);
        glVertex3f(height * 0.03f, height * 0.3f, 0);
        glEnd();
        glBegin(GL_POLYGON); // blade
        glVertex3f(-height * 0.03f, -height * 0.05f, 0);
        glVertex3f(-height * 0.08f, -height * 0.1f, 0);
        glVertex3f(-height * 0.08f, -height * 0.2f, 0);
        glVertex3f(height * 0.08f, -height * 0.2f, 0);
        glVertex3f(height * 0.08f, -height * 0.1f, 0);
        glVertex3f(height * 0.03f, -height * 0.05f, 0);
        glEnd();
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
    float height = windowRatio / 4 * 0.8;
    float width = height * 0.8;

    void draw()
    {
        glColor3f(0.2f, 0.7f, 0.0f);
        glRectf(x - 0.5 * width, y - 0.2 * height, x + 0.5 * width, y + 0.5 * height);
        glColor3f(0.8f, 0.4f, 0.4f);
        glRectf(x - 0.1 * width, y - 0.5 * height, x + 0.1 * width, y + 0.1 * height);
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
    return;
}

vector<Tree*> trees;

/************************** SHOE **************************/
class Shoe {
public:
    Shoe(float x_input, float y_input);
    float x, y;
    float height = windowRatio / 4 * 0.3;
    float width = height * 2.3;

    void move()
    {

    }
    void draw()
    {
        glPushMatrix();
        glTranslatef(x, y, 0);
        glColor3f(1.0f, 0.1f, 0.5f);
        glBegin(GL_POLYGON); // 신발
        glVertex3f(-width * 0.5f, -height * 0.28f, 0);
        glVertex3f(width * 0.5f, -height * 0.28f, 0);
        glVertex3f(width * 0.2f, height * 0.14f, 0);
        glVertex3f(-width * 0.1f, height * 0.5f, 0);
        glVertex3f(-width * 0.25f, height * 0.28f, 0);
        glVertex3f(-width * 0.45f, height * 0.42f, 0);
        glEnd();
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POLYGON); // 밑창
        glVertex3f(-width * 0.5f, -height * 0.28f, 0);
        glVertex3f(-width * 0.5f, -height * 0.5f, 0);
        glVertex3f(width * 0.5f, -height * 0.5f, 0);
        glVertex3f(width * 0.5f, -height * 0.28f, 0);
        glEnd();
        glLineWidth(40 * width);
        glBegin(GL_LINES);
        glVertex2f(-width * 0.2f, -height * 0.28f); glVertex2f(-width * 0.095f, height * 0.14f);
        glVertex2f(-width * 0.08f, -height * 0.28f); glVertex2f(width * 0.0075f, height * 0.07f);
        glVertex2f(width * 0.04f, -height * 0.28f); glVertex2f(width * 0.11f, height * 0);
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
Shoe* shoe = new Shoe(-0.85f, field1_y);
vector<Shoe*> shoes = { shoe, };


/************************** Mushroom **************************/
class Mushroom {
public:
    Mushroom(float x_input, float y_input);
    float x, y;
    float height = windowRatio / 4 * 0.5;
    float width = height;

    void move()
    {
    }

    void draw()
    {
        glPushMatrix();
        glTranslatef(x, y, 0);
        glColor3f(0.8f, 0.2f, 0.7f);
        glBegin(GL_POLYGON);
        glVertex3f(-0.5 * width, 0.1 * height, 0);
        glVertex3f(-0.45 * width, 0.3 * height, 0);
        glVertex3f(-0.2 * width, 0.5 * height, 0);
        glVertex3f(0.2 * width, 0.5 * height, 0);
        glVertex3f(0.45 * width, 0.3 * height, 0);
        glVertex3f(0.5 * width, 0.1 * height, 0);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex3f(0.1 * width, 0.1 * height, 0);
        glVertex3f(0.1 * width, -0.4 * height, 0);
        glVertex3f(-0.1 * width, -0.4 * height, 0);
        glVertex3f(-0.1 * width, 0.1 * height, 0);
        glEnd();
        glPopMatrix();
    }

};

Mushroom::Mushroom(float x_input, float y_input)
{
    x = x_input;
    y = y_input;
    return;
}
Mushroom* mushroom = new Mushroom(-0.2f, field1_y);
vector<Mushroom*> mushrooms = { mushroom, };


/************************** JumpPad **************************/
class JumpPad {
public:
    float x,y;
    float height = windowRatio * 0.2;
    float width = height;

    JumpPad(float _x, float _y) : x(_x), y(_y) {}

    void draw()
    {   
        glPushMatrix();
        glTranslatef(x,y,0);
        glColor3f(0.65, 0.3, 0);
        glRectf(-0.5 * width, -0.5 * height, 0.5 * width, 0.5 * height);
        glColor3f(1, 1, 0);
        glBegin(GL_POLYGON);
        glVertex3f(-0.25 * width, 0.1 * height, 0);
        glVertex3f(0, 0.3 * height, 0);
        glVertex3f(0.25 * width, 0.1 * height, 0);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex3f(0.1 * width, 0.1 * height, 0);
        glVertex3f(0.1 * width, -0.3 * height, 0);
        glVertex3f(-0.1 * width, -0.3 * height, 0);
        glVertex3f(-0.1 * width, 0.1 * height, 0);
        glEnd();
        glPopMatrix();
    }
};

JumpPad* jumpPad = new JumpPad(0.7f, field0_y);
vector<JumpPad*> jumpPads = { jumpPad, };

/**************************  **************************/


/************************** PLAYER **************************/
class Player {
public:
    float x, y, speedX, speedY;
    float scale = 1;
    bool ghostMode = false;

    float height = windowRatio / 4 * 0.6;
    float width = height * 1.1;
    int moveKeyLogger[4] = { 0,0,0,0 };
    bool isMoving = false;
    bool didGetShoe = false;
    
    bool isJumping = false;
    float jumpSpeed = 0.01;
    float jumpSpeedY = jumpSpeed * 0.7;
    float jumpSpeedZ = jumpSpeed * 0.7;

    float g = (4 * jumpSpeed * jumpSpeed ) / (3 * windowRatio);
    

    bool action_mode[2] = { false,false };
    int dy_left_panny = 1;
    float trans_left_panny = 0;
    int dy_right_panny = -1;
    float trans_right_panny = 0;
    int dx_legs = 1;
    float trans_legs = 0;
    int ds = -1;
    float degree_arms = 25;

    Player(float _x, float _y, float _speedX, float _speedY)
        : x(_x), y(_y), speedX(_speedX), speedY(_speedY) {}


    bool checkTreeOverlap(float newX, float newY)
    {
        if (debugMode)
            return false;
        for (int i = 0; i < trees.size(); i++)
        {
            Tree* temp = trees[i];
            float distance_x = abs(temp->x - newX);
            float distance_y = abs(temp->y - newY);
            bool overlap_x = ((temp->width + width) * 0.5 - distance_x) > 0.01;
            bool overlap_y = ((temp->height + height) * 0.5 - distance_y) > 0.01;

            if (overlap_x && overlap_y)
                return true;
        }
        return false;
    }

    bool checkCrash(void)
    {
        float distance_x, distance_y;
        float difference_x, difference_y;
         
        if (debugMode || ghostMode)
            return false;

        action_mode[0] = false;
        for (int i = 0; i < cars.size(); i = i + 1)
        {
            Car* temp = cars[i];
            distance_x = abs(temp->x - x);
            distance_y = abs(temp->y - y);
            difference_x = (temp->car_width + width) * 0.5f - distance_x;
            difference_y = (temp->car_height + height) * 0.5f - distance_y;

            if ((difference_x > 0) && (difference_y > 0))
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
        y = -windowRatio * 7 / 8;
    }
    bool checkInRiver(void)
    {
        if (debugMode)
            return false;

        float river_y = windowRatio / 8 * 5;
        float river_height = windowRatio / 4;

        if ((y < river_y + 0.5 * river_height) && (y > river_y - 0.5 * river_height))
            return true;
        return false;
    }

    void checkGetItem(void)
    {
        for (int i = 0; i < shoes.size(); i = i + 1)
        {
            float distance_x = abs(shoes[i]->x - x);
            float distance_y = abs(shoes[i]->y - y);
            if (((shoes[i]->width + width) * 0.5f > distance_x) && ((shoes[i]->height + height) * 0.5f > distance_y))
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
            if (((mushrooms[i]->width + width) * 0.5f > distance_x) && ((mushrooms[i]->height + height) * 0.5f > distance_y))
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
            if ((jumpPads[i]->width * 0.25f > distance_x) && (jumpPads[i]->height * 0.25f > distance_y))
            {
                return true;
            }
        }
        return false;
    }

    bool checkGameClear(void)
    {
        bool didClear = false;

        if (y >= (windowRatio / 8 * 7)) didClear = true;

        return didClear;
    }

    void move()
    {
        int dx = moveKeyLogger[3] - moveKeyLogger[2];
        int dy = moveKeyLogger[0] - moveKeyLogger[1];

        if ((dx != 0) || (dy != 0))
            isMoving = true;
        else
            isMoving = false;

        if (isJumping)
        {
            isMoving = true;
            y += jumpSpeedY;
            scale += jumpSpeedZ * 8;
            jumpSpeedZ -= g;
            
            if (scale < 1)
            {
                scale = 1;
                jumpSpeedY = jumpSpeed * 0.7;
                jumpSpeedZ = jumpSpeed * 0.7;
                isJumping = false;
                ghostMode = false;
            }

        }

        else if (!checkTreeOverlap(x + dx * speedX, y + dy * speedY))
        {   
            x += dx * speedX;
            y += dy * speedY;

            if (x >= 1)
                x = -1;
            else if (x <= -1)
                x = 1;

            if (y < -windowRatio * 7 / 8)
                y = -windowRatio * 7 / 8;
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
        glPushMatrix(); // torso == pants
        glTranslatef(x, y, 0);
        glScalef(scale, scale, 1);

        if (isMoving)
        {   

            glPushMatrix(); // left arm
            glTranslatef(-0.35 * width, 0.125 * height, 0);
            glRotatef(degree_arms, 0, 0, 1);
            
            degree_arms -= ds;
            if (degree_arms > 25 || degree_arms < -25)
                ds *= -1;
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_POLYGON);
            glVertex3f(0, 0, 0);
            glVertex3f(0.075 * width, -0.3 * height, 0);
            glVertex3f(-0.15 * width, 0 * height, 0);
            glVertex3f(-0.15 * width, 0.15 * height, 0);
            glEnd();

            glPushMatrix(); // left hand
            glTranslatef(-0.15 * width, 0.075 * height, 0);
            glColor3f(1.0f, 0.9f, 0.7f);
            glBegin(GL_POLYGON);
            for (int i = 0; i < 200; i++) {
                GLfloat theta = 2.0f * M_PI * float(i) / float(100);
                GLfloat left_hand_x = 0.075 * height * cos(theta);
                GLfloat left_hand_y = 0.075 * height * sin(theta);
                glVertex2f(left_hand_x, left_hand_y);
            }
            glEnd();
            glPopMatrix();
            glPopMatrix();

            glPushMatrix(); // right arm
            glTranslatef(0.35 * width, 0.125 * height, 0);
            glRotatef(degree_arms, 0, 0, 1);
            
            degree_arms -= ds;
            if (degree_arms > 25 || degree_arms < -25)
                ds *= -1;
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_POLYGON);
            glVertex3f(0, 0, 0);
            glVertex3f(-0.075 * width, -0.3 * height, 0);
            glVertex3f(0.15 * width, 0 * height, 0);
            glVertex3f(0.15 * width, 0.15 * height, 0);
            glEnd();

            glPushMatrix(); // right hand
            glTranslatef(0.15 * width, 0.075 * height, 0);
            glColor3f(1.0f, 0.9f, 0.7f);
            glBegin(GL_POLYGON);
            for (int i = 0; i < 200; i++) {
                GLfloat theta = 2.0f * M_PI * float(i) / float(100);
                GLfloat left_hand_x = 0.075 * height * cos(theta);
                GLfloat left_hand_y = 0.075 * height * sin(theta);
                glVertex2f(left_hand_x, left_hand_y);
            }
            glEnd();
            glPopMatrix();
            glPopMatrix();

            glPushMatrix(); // left leg
            glTranslatef(-0.15 * width, -0.2 * height, 0);
            glTranslatef(trans_legs, 0, 0);
            trans_legs += dx_legs * 0.01 * width;
            if (trans_legs > 0.07 * width || trans_legs < -0.07 * width)
                dx_legs *= -1;
            glColor3f(1.0f, 0.9f, 0.7f);
            glBegin(GL_POLYGON);
            glVertex3f(0, 0, 0); glVertex3f(-0.1 * width, 0, 0);
            glVertex3f(-0.2 * width, -0.1 * height, 0); glVertex3f(-0.1 * width, -0.1 * height, 0);
            glEnd();
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_POLYGON);
            glVertex3f(-0.2 * width, -0.1 * height, 0); glVertex3f(-0.1 * width, -0.1 * height, 0);
            glVertex3f(0, -0.2 * height, 0); glVertex3f(-0.1 * width, -0.2 * height, 0);
            glEnd();
            if (didGetShoe) glColor3f(1.0f, 0.1f, 0.5f);
            else glColor3f(1.0f, 1.0f, 0.0f);
            glBegin(GL_POLYGON);
            glVertex3f(0, -0.2 * height, 0); glVertex3f(0, -0.3 * height, 0);
            glVertex3f(-0.17 * width, -0.3 * height, 0); glVertex3f(-0.2 * width, -0.25 * height, 0); glVertex3f(-0.17 * width, -0.2 * height, 0);
            glEnd();
            glPopMatrix();

            glPushMatrix(); // right leg
            glTranslatef(0.15 * width, -0.2 * height, 0);
            glTranslatef(trans_legs, 0, 0);
            trans_legs += dx_legs * 0.01 * width;
            if (trans_legs > 0.07 * width || trans_legs < -0.07 * width)
                dx_legs *= -1;
            glColor3f(1.0f, 0.9f, 0.7f);
            glBegin(GL_POLYGON);
            glVertex3f(0, 0, 0); glVertex3f(0.1 * width, 0, 0);
            glVertex3f(0.2 * width, -0.1 * height, 0); glVertex3f(0.1 * width, -0.1 * height, 0);
            glEnd();
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_POLYGON);
            glVertex3f(0.2 * width, -0.1 * height, 0); glVertex3f(0.1 * width, -0.1 * height, 0);
            glVertex3f(0, -0.2 * height, 0); glVertex3f(0.1 * width, -0.2 * height, 0);
            glEnd();
            if (didGetShoe) glColor3f(1.0f, 0.1f, 0.5f);
            else glColor3f(1.0f, 1.0f, 0.0f);
            glBegin(GL_POLYGON);
            glVertex3f(0, -0.2 * height, 0); glVertex3f(0, -0.3 * height, 0);
            glVertex3f(0.17 * width, -0.3 * height, 0); glVertex3f(0.2 * width, -0.25 * height, 0); glVertex3f(0.17 * width, -0.2 * height, 0);
            glEnd();
            glPopMatrix();

            glPushMatrix(); // left hip
            glTranslatef(-0.175 * width, 0.185 * height, 0);
            glTranslatef(0, trans_left_panny, 0);
            if (didGetShoe) trans_left_panny += dy_left_panny * 0.02 * height;
            else trans_left_panny += dy_left_panny * 0.01 * height;
            if (trans_left_panny > 0.07 * height || trans_left_panny < -0.07 * height)
                dy_left_panny *= -1;
            glColor3f(1.0f, 0.9f, 0.7f);
            glBegin(GL_POLYGON);
            for (int i = 0; i < 200; i++) {
                GLfloat theta = 2.0f * M_PI * float(i) / float(100);
                GLfloat left_fanny_x = 0.19 * width * cos(theta);
                GLfloat left_fanny_y = 0.19 * width * sin(theta);
                glVertex2f(left_fanny_x, left_fanny_y);
            }
            glEnd();
            glPopMatrix();

            glPushMatrix(); // right hip
            glTranslatef(0.175 * width, 0.185 * height, 0);
            glTranslatef(0, trans_right_panny, 0);
            if (didGetShoe) trans_right_panny += dy_right_panny * 0.02 * height;
            else trans_right_panny += dy_right_panny * 0.01 * height;
            if (trans_right_panny > 0.07 * height || trans_right_panny < -0.07 * height)
                dy_right_panny *= -1;
            glColor3f(1.0f, 0.9f, 0.7f);
            glBegin(GL_POLYGON);
            for (int i = 0; i < 200; i++) {
                GLfloat theta = 2.0f * M_PI * float(i) / float(100);
                GLfloat right_fanny_x = 0.19 * width * cos(theta);
                GLfloat right_fanny_y = 0.19 * width * sin(theta);
                glVertex2f(right_fanny_x, right_fanny_y);
            }
            glEnd();
            glPopMatrix();

            glColor3f(1.0f, 1.0f, 0.0f); // yellow
            glBegin(GL_POLYGON); // left pants
            glVertex2f(0, -0.1 * height);
            glVertex2f(0, 0.125 * height);
            glVertex2f(-0.35 * width, 0.125 * height);
            glVertex2f(-0.4 * width, -0.25 * height);
            glVertex2f(-0.1 * width, -0.25 * height);
            glEnd();
            glBegin(GL_POLYGON); // right pants
            glVertex2f(0, -0.1 * height);
            glVertex2f(0, 0.125 * height);
            glVertex2f(0.35 * width, 0.125 * height);
            glVertex2f(0.4 * width, -0.25 * height);
            glVertex2f(0.1 * width, -0.25 * height);
            glEnd();
        }

        else // 키 입력 안 받을 때
        {
            glColor3f(1.0f, 0.0f, 0.0f); // red
            glRectf(-0.25 * width, -0.2 * height, 0.25 * width, 0 * height);

            glPushMatrix(); // left upper arm
            glTranslatef(-0.25 * width, 0, 0);

            glPushMatrix(); // left lower arm
            glTranslatef(-0.125 * width, 0.1 * height, 0);

            glColor3f(1.0f, 0.9f, 0.7f);
            glBegin(GL_POLYGON);
            glVertex3f(0, 0, 0);
            glVertex3f(0, -0.1 * height, 0);
            glVertex3f(-0.125 * width, 0, 0);
            glVertex3f(-0.125 * width, 0.1 * height, 0);
            glEnd();
            glPopMatrix();
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_POLYGON);
            glVertex3f(0, 0, 0);
            glVertex3f(0, -0.15 * height, 0);
            glVertex3f(-0.125 * width, 0, 0);
            glVertex3f(-0.125 * width, 0.1 * height, 0);
            glEnd();
            glPopMatrix();

            glPushMatrix(); // right upper arm
            glTranslatef(0.25 * width, 0, 0);

            glPushMatrix(); // right lower arm
            glTranslatef(0.125 * width, -0.1 * height, 0);

            glColor3f(1.0f, 0.9f, 0.7f);
            glBegin(GL_POLYGON);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0.1 * height, 0);
            glVertex3f(0.125 * width, 0.1 * height, 0);
            glVertex3f(0.125 * width, 0.05 * height, 0);
            glEnd();
            glPopMatrix();
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_POLYGON);
            glVertex3f(0, 0, 0);
            glVertex3f(0, -0.15 * height, 0);
            glVertex3f(0.125 * width, -0.1 * height, 0);
            glVertex3f(0.125 * width, 0, 0);
            glEnd();
            glPopMatrix();

            glPushMatrix(); // legs
            glTranslatef(0, -0.2 * height, 0);
            glColor3f(1.0f, 1.0f, 0.0f); // yellow
            glBegin(GL_POLYGON); glVertex2f(0, 0); glVertex2f(0, -0.05 * height); glVertex2f(-0.05 * width, -0.12 * height); glVertex2f(-0.25 * width, -0.12 * height); glVertex2f(-0.25 * width, 0); glEnd(); // left pants
            glBegin(GL_POLYGON); glVertex2f(0, 0); glVertex2f(0, -0.05 * height); glVertex2f(0.05 * width, -0.12 * height); glVertex2f(0.25 * width, -0.12 * height); glVertex2f(0.25 * width, 0); glEnd(); // right pants
            glTranslatef(0, -0.12 * height, 0);
            glColor3f(1.0f, 0.9f, 0.7f);
            glBegin(GL_POLYGON); glVertex2f(-0.1 * width, 0); glVertex2f(-0.2 * width, 0); glVertex2f(-0.2 * width, -0.07 * height); glVertex2f(-0.1 * width, -0.07 * height); glEnd(); // left leg
            glBegin(GL_POLYGON); glVertex2f(0.1 * width, 0); glVertex2f(0.2 * width, 0); glVertex2f(0.2 * width, -0.07 * height); glVertex2f(0.1 * width, -0.07 * height); glEnd(); // right leg
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_POLYGON); glVertex2f(-0.1 * width, -0.07 * height); glVertex2f(-0.2 * width, -0.07 * height); glVertex2f(-0.2 * width, -0.13 * height); glVertex2f(-0.1 * width, -0.13 * height); glEnd(); // left sock
            glBegin(GL_POLYGON); glVertex2f(0.1 * width, -0.07 * height); glVertex2f(0.2 * width, -0.07 * height); glVertex2f(0.2 * width, -0.13 * height); glVertex2f(0.1 * width, -0.13 * height); glEnd(); // right sock
            if (didGetShoe) glColor3f(1.0f, 0.1f, 0.5f);
            else glColor3f(1.0f, 1.0f, 0.0f);
            glBegin(GL_POLYGON); glVertex2f(-0.1 * width, -0.13 * height); glVertex2f(-0.25 * width, -0.13 * height); glVertex2f(-0.3 * width, -0.155 * height); glVertex2f(-0.25 * width, -0.18 * height); glVertex2f(-0.1 * width, -0.18 * height); glEnd(); // left shoe
            glBegin(GL_POLYGON); glVertex2f(0.1 * width, -0.13 * height); glVertex2f(0.25 * width, -0.13 * height); glVertex2f(0.3 * width, -0.155 * height); glVertex2f(0.25 * width, -0.18 * height); glVertex2f(0.1 * width, -0.18 * height); glEnd(); // right shoe
            glPopMatrix();

            glPushMatrix(); // head
            glTranslatef(0.05 * width, 0.2 * height, 0);
            glColor3f(1.0f, 0.9f, 0.7f);
            glBegin(GL_POLYGON); for (int i = 0; i < 200; i++) { GLfloat theta = 2.0f * M_PI * float(i) / float(100); GLfloat face_x = 0.32 * height * cos(theta); GLfloat face_y = 0.3 * height * sin(theta); glVertex2f(face_x, face_y); } glEnd();
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_POLYGON); for (int i = 7; i < 35; i++) { GLfloat theta = 2.0f * M_PI * float(i) / float(100); GLfloat hair_x = 0.32 * height * cos(theta); GLfloat hair_y = 0.3 * height * sin(theta); glVertex2f(hair_x, hair_y); } glEnd();
            glBegin(GL_POLYGON); for (int i = -5; i < 15; i++) { GLfloat theta = 2.0f * M_PI * float(i) / float(100); GLfloat hair_x = 0.32 * height * cos(theta); GLfloat hair_y = 0.3 * height * sin(theta); glVertex2f(hair_x, hair_y); } glEnd();
            glTranslatef(-0.2 * width, -0.125 * height, 0);
            glColor3f(1.0f, 0.9f, 0.7f);
            glBegin(GL_POLYGON); for (int i = 0; i < 200; i++) { GLfloat theta = 2.0f * M_PI * float(i) / float(100); GLfloat face_x = 0.2 * height * cos(theta); GLfloat face_y = 0.2 * height * sin(theta); glVertex2f(face_x, face_y); } glEnd();

            glTranslatef(0, -0.2 * height, 0);
            glBegin(GL_POLYGON); glVertex3f(0, 0, 0); glVertex3f(0.2 * width, 0.025 * height, 0); glVertex3f(0.2 * width, 0.2 * height, 0); glEnd();
            glTranslatef(0.05 * width, 0.125 * height, 0);
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_POLYGON); for (int i = 0; i < 200; i++) { GLfloat theta = 2.0f * M_PI * float(i) / float(100); GLfloat mouth_x = 0.04 * height * cos(theta); GLfloat mouth_y = 0.06 * height * sin(theta); glVertex2f(mouth_x, mouth_y); } glEnd(); // mouth
            glTranslatef(0 * width, 0.22 * height, 0);
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_POLYGON); for (int i = 0; i < 200; i++) { GLfloat theta = 2.0f * M_PI * float(i) / float(100); GLfloat eye_x = 0.1 * height * cos(theta); GLfloat eye_y = 0.1 * height * sin(theta); glVertex2f(eye_x, eye_y); } glEnd(); // left eye (black)
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_POLYGON); for (int i = 0; i < 200; i++) { GLfloat theta = 2.0f * M_PI * float(i) / float(100); GLfloat eye_x = 0.04 * height * cos(theta); GLfloat eye_y = 0.04 * height * sin(theta); glVertex2f(eye_x, eye_y); } glEnd(); // left eye (white)
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_POLYGON); glVertex2f(0.1 * width, 0.1 * height); glVertex2f(0.1 * width, 0.16 * height); glVertex2f(-0.05 * width, 0.21 * height); glVertex2f(-0.05 * width, 0.15 * height); glEnd(); // left eyebrow
            glTranslatef(0.23 * width, -0.05 * height, 0);
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_POLYGON); for (int i = 0; i < 200; i++) { GLfloat theta = 2.0f * M_PI * float(i) / float(100); GLfloat eye_x = 0.1 * height * cos(theta); GLfloat eye_y = 0.1 * height * sin(theta); glVertex2f(eye_x, eye_y); } glEnd(); // right eye (black)
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_POLYGON); for (int i = 0; i < 200; i++) { GLfloat theta = 2.0f * M_PI * float(i) / float(100); GLfloat eye_x = 0.04 * height * cos(theta); GLfloat eye_y = 0.04 * height * sin(theta); glVertex2f(eye_x, eye_y); } glEnd(); // right eye (white)
            glColor3f(0.0f, 0.0f, 0.0f);
            glRectf(-0.06 * width, 0.12 * height, 0.09 * width, 0.18 * height); // right eyebrow
            glPopMatrix();
        }

        if (action_mode[0]) // 차에 가까워지면
        {
            glPushMatrix();
            glTranslatef(0.25 * width, 0.25 * height, 0);
            glColor3f(1.0f, 0.0f, 0.0f);
            glRectf(-0.15 * width, 0.15 * height, -0.05 * width, -0.05 * height);
            glRectf(-0.15 * width, -0.1 * height, -0.05 * width, -0.2 * height);
            glRectf(0.15 * width, 0.15 * height, 0.05 * width, -0.05 * height);
            glRectf(0.15 * width, -0.1 * height, 0.05 * width, -0.2 * height);
            glPopMatrix();
        }

        if (action_mode[1]) // 버섯을 먹으면
        {
            glPushMatrix();
            glTranslatef(-0.25 * width, 0.25 * height, 0);
            glColor3f(0.8f, 0.2f, 0.7f);
            glRectf(-0.15 * width, 0.15 * height, -0.05 * width, -0.05 * height);
            glRectf(-0.15 * width, -0.1 * height, -0.05 * width, -0.2 * height);
            glRectf(0.15 * width, 0.15 * height, 0.05 * width, -0.05 * height);
            glRectf(0.15 * width, -0.1 * height, 0.05 * width, -0.2 * height);
            glPopMatrix();
        }

        glPopMatrix();
    }
};

Player player(0.0f, -windowRatio / 8 * 7, 0.0035f, windowRatio / 4 * 0.025);
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
                        players[i].x = 0.0f;
                        players[i].y = -windowRatio * 7 / 8;
                    }
                    else
                        players[i].x += transportaionMove;
                }
                if (players[i].checkCrash() || gameReset)
                {   
                    players[i].reset();
                    if(shoes.size() ==0)
                        shoes.push_back(shoe);
                    if(mushrooms.size() == 0)
                        mushrooms.push_back(mushroom);
                    gameReset = false;
                }

                if (players[i].checkInJumpPad())
                {
                    players[i].isJumping = true;
                    players[i].ghostMode = true;

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

                if ((temp->x * temp->dx) > 1.15)
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
                int rand = dist(mt)%3;

                if (rand == 1)
                {    
                    Car* newCar;

                    if ( (lastCar[i] == NULL) || ( (lane_dx[i] * lastCar[i]->x) > -0.8 ) )
                    {
                        newCar = new Car(-lane_dx[i] * 1.15, lane_pos[i], lane_speed[i], lane_dx[i], dist(mt) % 2);
                        cars.push_back(newCar);
                        lastCar[i] = newCar;
                    }
                }
            }
            glutPostRedisplay();
            glutTimerFunc(500, CarManager::createCars, 0.1);
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
        float tree_x = -0.4f;

        Tree* newTreeInField0;
        newTreeInField0 = new Tree(tree_x, 0);
        trees.push_back(newTreeInField0);

        tree_x = 0.4f;
        for (int i = 0; i < 3; i++) // field1의 나무들 생성
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
        float y = windowRatio / 8 * 5;
        float speed = 0.005;
        for (int i = 0; i < 4; i++)
        {
            Boat* boat;
            boat = new Boat(x, y, speed);
            boats.push_back(boat);
            x += 0.15f + 0.2 + i;
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

/************************** BACKGROUND **************************/
void drawLine(float y, GLfloat width) {
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(width);

    // 선 그리기
    glBegin(GL_LINES);
    glVertex2f(-1, y); // 시작점
    glVertex2f(1, y); // 끝점
    glEnd();

    //glFlush();
}

void drawStippleLine(float y, GLfloat width) {
    glColor3f(1, 1, 1);
    glLineWidth(width);

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x00FF);
    glBegin(GL_LINES);
    glVertex2f(-1, y);
    glVertex2f(1, y);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}

void drawBackGround() {

    glColor3f(0.4f, 0.9f, 0.2f);
    glRectf(-1, -1, 1, -windowRatio / 8 * 6); // 시작점 (맨 아래 잔디)

    glColor3f(0.3f, 0.3f, 0.3f);
    glRectf(-1, -windowRatio / 8 * 6, 1, -windowRatio / 8 * 2); // line0 & line1

    glColor3f(0.4f, 0.9f, 0.2f);
    glRectf(-1, -windowRatio / 8 * 2, 1, 0); // 중간 잔디

    glColor3f(0.3f, 0.3f, 0.3f);
    glRectf(-1, 0, 1, windowRatio / 8 * 4); // line2 & line3

    glColor3f(0.0f, 0.5f, 1.0f);
    glRectf(-1, windowRatio / 8 * 4, 1, windowRatio / 8 * 6); // 강

    glColor3f(0.4f, 0.9f, 0.2f);
    glRectf(-1, windowRatio / 8 * 6, 1, windowRatio); // 마지막  잔디

    drawLine(-windowRatio / 8 * 6, 2);
    drawLine(-windowRatio / 8 * 2, 2);
    drawLine(0, 2);
    drawLine(windowRatio / 8 * 4, 2);

    drawStippleLine(-windowRatio / 8 * 4, 2);
    drawStippleLine(windowRatio / 8 * 2, 2);

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

void keyboards(unsigned char key, int x, int y)
{
    switch (key) {
    case 'P':
    case 'p':
        debugMode ^= 1;
        break;
    case 'R':
    case 'r':
        gameReset = true;
    }
}

void updateTimerFunctions()
{   
    carManager.updateCar(1);
    boatManager.updateBoat(1);
    playerManager.updatePlayer(1);
}

void renderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
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


void main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
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
    glutKeyboardFunc(keyboards);
    gluOrtho2D(-1, 1, -windowRatio, windowRatio);
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glewInit();
    glutMainLoop();
    return;
}
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/vec3.hpp>
#include <vector>

using namespace std;

int windowWidth = 900;
int windowHeight = 600;
bool gamePause = false;

class Car;
class Player;
class Tree;
class Boat;
class CarManager;
class PlayerManager;
class TreeManager;
class BoatManager;

/************************** LANE **************************/
float lane0_y = -0.55f;
float lane0_speed = 0.001f;
int lane0_direction = 0;

float lane1_y = -0.25f;
float lane1_speed = 0.003f;
int lane1_direction = 1;

float lane2_y = 0.35f;
float lane2_speed = 0.004f;
int lane2_direction = 0;

float lane3_y = 0.65f;
float lane3_speed = 0.005f;
int lane3_direction = 1;

float field0_y = -0.85f;
float field1_y = 0.05f;

/************************** CAR **************************/
class Car {
public:
    Car(float x_input, int lane_input);
    float x, y, speed;
    int direction; // 0: left, 1: right
    int lane; // 아래서부터 0, 1, 2, 3
    float car_height = 0.2f;
    float car_width = 0.3f;
    void move()
    {
        if (direction == 0)
        {
            x -= speed;
            if (x < -1.15f) x = 1.15f;
        }
        else if (direction == 1)
        {
            x += speed;
            if (x > 1.15f) x = -1.15f;
        }
    }
    void draw()
    {
        glColor3f(1.0f, 0.7f, 0.2f);
        glRectf(x - 0.5 * car_width, y - 0.5 * car_height, x + 0.5 * car_width, y + 0.5 * car_height);
    }

};

Car::Car(float x_input, int lane_input)
{
    x = x_input;
    lane = lane_input;
    switch (lane)
    {
    case 0:
        y = lane0_y;
        speed = lane0_speed;
        direction = lane0_direction;
        break;
    case 1:
        y = lane1_y;
        speed = lane1_speed;
        direction = lane1_direction;
        break;
    case 2:
        y = lane2_y;
        speed = lane2_speed;
        direction = lane2_direction;
        break;
    case 3:
        y = lane3_y;
        speed = lane3_speed;
        direction = lane3_direction;
        break;
    default:
        break;
    }
    return;
}

vector<Car> cars;

/************************** BOAT **************************/
class Boat {
public:
    Boat(float _x, float _y, float _speed)
        :x(_x), y(_y), speed(_speed) {};

    float x, y, speed;
    int dx = -1;
    float height = 0.2f;
    float width = 0.25f;
    
    void move()
    {
        x += dx * speed;
        if (x < -1.3f)
            x = 1.3;
        else if (x > 1.3f)
            x = -1.3f;
    }

    void draw()
    {
        glColor3f(0.6f, 0.2f, 0.0f);
        glRectf(x - 0.5 * width, y - 0.5 * height, x + 0.5 * width, y + 0.5 * height);
    }

};


vector<Boat> boats;

/************************** TREE **************************/
class Tree {
public:
    Tree(float x_input, int field_input);
    float x, y;
    float width = 0.3f;
    float height = 0.2f;

    void draw()
    {
        glColor3f(0.2f, 0.7f, 0.0f);
        glRectf(x - 0.5 * width, y - 0.5 * height, x + 0.5 * width, y + 0.5 * height);
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

vector<Tree> trees;

/************************** PLAYER **************************/
class Player {
public:
    Player(float _x, float _y, float _width, float _height, float _speedX, float _speedY)
        : x(_x), y(_y), width(_width), height(_height), speedX(_speedX), speedY(_speedY) {}

    float x, y, width, height, speedX, speedY;
    char face_mode = 0;

    bool checkTreeOverlap(float newX, float newY)
    {
        for (int i = 0; i < trees.size(); i++)
        {
            float distance_x = abs(trees[i].x - newX);
            float distance_y = abs(trees[i].y - newY);
            bool overlap_x = ((trees[i].width + width) * 0.5 - distance_x) > 0.01;
            bool overlap_y = ((trees[i].height + height) * 0.5 - distance_y) > 0.01;

            if (overlap_x && overlap_y)
                return true;
        }
        return false;
    }

    bool checkCrash(void)
    {
        bool didCrash = false;

        float distance_x, distance_y;
        float difference_x, difference_y;

        face_mode = 0;

        for (int i = 0; i < cars.size(); i = i + 1)
        {
            Car* temp = &cars[i];
            distance_x = abs(temp->x - x);
            distance_y = abs(temp->y - y);
            difference_x = (temp->car_width + width) * 0.5f - distance_x;
            difference_y = (temp->car_height + height) * 0.5f - distance_y;

            if ((difference_x > 0) && (difference_y > 0))
            {
                didCrash = true;
                break;
            }
            else if ((difference_x > -0.15) && (difference_y > -0.01))
            {
                face_mode = 1;
            }
        }
        return didCrash;
    }

    bool checkInRiver(void)
    {   
        float river_y = 0.95;
        float river_height = 0.3;

        if ((y < river_y + 0.5 * river_height) && (y > river_y - 0.5 * river_height))
            return true;
        return false;
    }

    bool checkGameClear(void)
    {
        bool didClear = false;

        if (y >= 1.15f) didClear = true;

        return didClear;
    }

    void move(float dx, float dy)
    {

        if (!checkTreeOverlap(x + dx * speedX, y + dy * speedY))
        {
            x += dx * speedX;
            y += dy * speedY;

            if (x >= 1)
                x = -1 + speedX;
            else if (x <= -1)
                x = 1 - speedX;

            if (y < -1)
                y = -1 + speedY /2;
        }
    }

    float getTransportationMove()
    {

        float distance_x, distance_y;
        float difference_x, difference_y;

        for (int i = 0; i < boats.size(); i = i + 1)
        {
            Boat* temp = &boats[i];
            distance_x = abs(temp->x - x);
            distance_y = abs(temp->y - y);

            if ((distance_x < (0.5 * temp->width)) && (distance_y < (0.5 * temp->height)))
            {
                return temp->speed * temp->dx;
            }
        }
        return 0;
    }

    void draw()
    {

        //hand
        if (face_mode == 2)
        {
            glLineWidth(4);
            glBegin(GL_LINES);
            glColor3f(0.0f, 0.0f, 0.0f);
            glVertex2f(x - 0.49 * width, y);
            glVertex2f(x - 0.8 * width, y + 0.3 * height);
            glVertex2f(x + 0.49 * width, y);
            glVertex2f(x + 0.8 * width, y + 0.3 * height);
            glEnd();
        }


        //body base
        if (face_mode == 2)
            glColor3f(0.3f, 0.7f, 0.9f);
        else
            glColor3f(1.0f, 0.4f, 0.4f);

        glRectf(x - 0.5 * width, y - 0.5 * height, x + 0.5 * width, y + 0.5 * height);


        //eyes
        if (face_mode == 0)
        {
            glColor3f(0.0f, 0.0f, 0.0f);
            glRectf(x - 0.4 * width, y + 0.2 * height, x - 0.1 * width, y + 0.1 * height);
            glRectf(x + 0.4 * width, y + 0.2 * height, x + 0.1 * width, y + 0.1 * height);
        }
        else if (face_mode == 1)
        {
            glColor3f(0.0f, 0.0f, 0.0f);
            glRectf(x - 0.2 * width, y + 0.4 * height, x - 0.1 * width, y + 0.1 * height);
            glRectf(x + 0.2 * width, y + 0.4 * height, x + 0.1 * width, y + 0.1 * height);
        }

        else if (face_mode == 2)
        {
            glColor3f(0.0f, 0.0f, 0.0f);
            glLineWidth(4);
            glBegin(GL_LINE_STRIP);
            glVertex2f(x - 0.4 * width, y + 0.1 * height );
            glVertex2f(x - 0.25 * width, y + 0.3 * height);
            glVertex2f(x - 0.1 * width, y + 0.1 * height);
            glEnd();

            glBegin(GL_LINE_STRIP);
            glVertex2f(x + 0.4 * width, y + 0.1 * height);
            glVertex2f(x + 0.25 * width, y + 0.3 * height);
            glVertex2f(x + 0.1 * width, y + 0.1 * height);
            glEnd();
        }


        //mouth
        if (face_mode == 0)
        {
            glLineWidth(4);
            glBegin(GL_LINE_STRIP);
            glVertex2f(x - 0.15 * width, y - 0.3 * height);
            glVertex2f(x, y - 0.1 * height);
            glVertex2f(x + 0.15 * width, y - 0.3 * height);
            glEnd();
        }

        else if (face_mode == 1)
        {
            glLineWidth(4);
            glBegin(GL_LINE_STRIP);
            glVertex2f(x - 0.15 * width, y - 0.3 * height);
            glVertex2f(x - 0.15 * width, y);
            glVertex2f(x + 0.15 * width, y);
            glVertex2f(x + 0.15 * width, y - 0.3 * height);
            glVertex2f(x - 0.15 * width, y - 0.3 * height);
            glEnd();
        }

        else if (face_mode == 2)
        {
            glColor3f(0.0f, 0.0f, 0.0f);
            glLineWidth(4);
            glBegin(GL_LINE_STRIP);
            glVertex2f(x - 0.2 * width, y - 0.15 * height);
            glVertex2f(x, y - 0.3 * height);
            glVertex2f(x + 0.2 * width, y - 0.15 * height);
            glEnd();
        }


        //hair
        glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(x - 0.2 * width, y + 0.5 * height);
        glVertex2f(x - 0.4 * width, y + 0.8 * height);
        glVertex2f(x + 0.2 * width, y + 0.5 * height);
        glVertex2f(x + 0.4 * width, y + 0.8 * height);
        glVertex2f(x, y + 0.5 * height);
        glVertex2f(x, y + 0.8 * height);
        glEnd();

        //foot
        glLineWidth(8);
        glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(x - 0.2 * width, y - 0.6 * height);
        glVertex2f(x - 0.6 * width, y - 0.6 * height);
        glVertex2f(x + 0.2 * width, y - 0.6 * height);
        glVertex2f(x + 0.6 * width, y - 0.6 * height);
        glEnd();

        
    }
};

Player player(0.0f, -0.85f, 0.1f, 0.15f, 0.1f, 0.3f);
vector<Player> players = { player };

/************************** Managers **************************/
class PlayerManager {

public:
    PlayerManager() {}

    static void updatePlayer(int value)
    {   
        for (int i = 0; i < players.size(); i++)
        {   
            if (players[i].checkInRiver())
            {   
                float transportaionMove = players[i].getTransportationMove();
                if (transportaionMove == 0)
                {
                    players[i].x = 0.0f;
                    players[i].y = -1 + players[i].speedY / 2;
                }
                else
                    players[i].x += transportaionMove;
            }
            if (players[i].checkCrash())
            {
                players[i].x = 0.0f;
                players[i].y = -1 + players[i].speedY / 2;
            }

            else if (players[i].checkGameClear())
            {
                gamePause = true;
                players[i].face_mode = 2;
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
    void createCar()
    {
        float car_x = -1.0f;
        for (int i = 0; i < 3; i++) // lane0의 car들 생성
        {
            Car* newCarInLane0;
            newCarInLane0 = new Car(car_x, 0);
            cars.push_back(*newCarInLane0);
            car_x += 0.45f;
        }
        car_x = 1.0f;
        for (int i = 0; i < 3; i++) // lane1의 car들 생성
        {
            Car* newCarInLane1;
            newCarInLane1 = new Car(car_x, 1);
            cars.push_back(*newCarInLane1);
            car_x -= 0.6f;
        }
        for (int i = 0; i < 4; i++) // lane2의 car들 생성
        {
            Car* newCarInLane2;
            newCarInLane2 = new Car(car_x, 2);
            cars.push_back(*newCarInLane2);
            car_x += 0.75f;
        }
        for (int i = 0; i < 4; i++) // lane3의 car들 생성
        {
            Car* newCarInLane3;
            newCarInLane3 = new Car(car_x, 3);
            cars.push_back(*newCarInLane3);
            car_x -= 0.6f;
        }
    }

    static void updateCar(int value)
    {   
        if (!gamePause)
        {
            for (int i = 0; i < cars.size(); i = i + 1)
            {
                Car* temp = &cars[i];
                temp->move();
            }
            glutPostRedisplay();
            glutTimerFunc(10, CarManager::updateCar, 0.1);
        }
    }

};

CarManager carManager;

class TreeManager
{
public:
    void createTree()
    {
        float tree_x = -0.4f;

        Tree* newTreeInField0;
        newTreeInField0 = new Tree(tree_x, 0);
        trees.push_back(*newTreeInField0);

        tree_x = 0.6f;
        for (int i = 0; i < 3; i++) // field1의 나무들 생성
        {
            Tree* newTreeInField1;
            newTreeInField1 = new Tree(tree_x, 1);
            trees.push_back(*newTreeInField1);
            tree_x -= 0.4f + (0.3 * i);
        }
    }
};

TreeManager treeManager;

class BoatManager
{
public:
    void createBoat()
    {
        float x = -0.8f;
        float y = 0.95f;
        float speed = 0.005;
        for (int i = 0; i < 4; i++)
        {
            Boat* boat;
            boat = new Boat(x,y,speed);
            boats.push_back(*boat);
            x += 0.15f + 0.2+i;
        }
    }

    static void updateBoat(int value)
    {
        if (!gamePause)
        {
            for (int i = 0; i < boats.size(); i = i + 1)
            {
                Boat* temp = &boats[i];
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
    glRectf(-1, -1, 1, -0.7); // 시작점 (맨 아래 잔디)

    glColor3f(0.3f, 0.3f, 0.3f);
    glRectf(-1, -0.7, 1, -0.1); // line0 & line1

    glColor3f(0.4f, 0.9f, 0.2f);
    glRectf(-1, -0.1, 1, 0.2); // 중간 잔디

    glColor3f(0.3f, 0.3f, 0.3f);
    glRectf(-1, 0.2, 1, 0.8); // line2 & line3

    glColor3f(0.0f, 0.5f, 1.0f);
    glRectf(-1, 0.8, 1, 1.1); // 강

    glColor3f(0.4f, 0.9f, 0.2f);
    glRectf(-1, 1.1, 1, 1.4); // 마지막  잔디

    drawLine(-0.7, 2);
    drawLine(-0.1, 2);
    drawLine(0.2, 2);
    drawLine(0.8, 2);

    drawStippleLine(0.5, 2);
    drawStippleLine(-0.4, 2);

    for (int i = 0; i < trees.size(); i = i + 1)
    {
        Tree* temp = &trees[i];
        temp->draw();
    }
}


/************************** CALLBACK FUNCTIONS **************************/
void inputKeys(int key, int x, int y) {
    if (!gamePause)
    {
        switch (key) {
        case GLUT_KEY_UP:
            players[0].move(0, 1);
            break;
        case GLUT_KEY_LEFT:
            players[0].move(-1, 0);
            break;
        case GLUT_KEY_RIGHT:
            players[0].move(1, 0);
            break;
        }
        glutPostRedisplay();
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
        Boat* temp = &boats[i];
        temp->draw();
    }

    for (int i = 0; i < cars.size(); i = i + 1)
    {
        Car* temp = &cars[i];
        temp->draw();
    }

    for (int i = 0; i < players.size(); i = i + 1)
    {
        Player* temp = &players[i];
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
    carManager.createCar();
    treeManager.createTree();
    boatManager.createBoat();
    glutDisplayFunc(renderScene);
    updateTimerFunctions();
    glutSpecialFunc(inputKeys);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.4);
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glewInit();

    glutMainLoop();
    return;
}
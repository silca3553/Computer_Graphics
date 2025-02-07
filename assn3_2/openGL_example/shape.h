#pragma once
#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <math.h>

using namespace std;
using namespace glm;

class Shape
{
public:
    GLuint vao;
    vec3 color;
    int vertexCount;
    GLenum shapeType;

    void setVertexArray(const vec3* positions, int size);
    void draw(GLint colorLoc, GLint transformLoc, mat4 transformMatrix, bool drawEdge);
};

class Cuboid : public Shape
{
public:
    Cuboid(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, float _r, float _g, float _b, float _tag);

    float x1, y1, z1, x2, y2, z2, r, g, b;
    float width, length, height, cx, cy, cz;
    int tag;

    void checkTrigger(bool* triggerCoords, float cx2, float cy2, float cz2, float width2, float length2, float height2);
};

class Rect : public Shape
{
public:
    Rect(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, float _r, float _g, float _b, bool _vertical, float _tag);
    float x1, y1, z1, x2, y2, z2, r, g, b;
    float width, length, height, cx, cy, cz;
    int tag;
    bool vertical;
};

class StripLine : public Shape
{
public:
    StripLine(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, float _r, float _g, float _b, float interval);
};


class Cylinder : public Shape
{
public:
    Cylinder(float _cx, float _cy, float _cz, float _radX, float radY, float _h, float _r, float _g, float _b, float _tag);
    float cx, cy, cz, radX, radY, h, r, g, b;
    int tag;
};

class Triangle : public Shape
{
public:
    Triangle(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, float _x3, float _y3, float _z3, float _r, float _g, float _b, float _tag);
    float x1, y1, z1, x2, y2, z2, x3, y3, z3, r, g, b;
    int tag;
};
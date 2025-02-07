#pragma once
#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <math.h>
#include "texture.h"

using namespace std;
using namespace glm;

class Shape
{
public:
    GLuint vao;
    vec3 color;
    int vertexCount;
    GLenum shapeType;
    GLuint diffuseTextureID;
    GLuint normalTextureID;
    bool shadingType = 0;
    bool isDiffuse = 1;
    bool isNormal = 0;
    int tag =0;

    void setMode(bool _isDiffuse, bool _isNormal);
    void setVertexArray(const vec3* positions, const vec3* normals, const vec2* texCoords, int positionSize, int texCoordSize);
    void setTexture(const Texture& texture, bool type);
    void draw(GLint diffuseTextureLoc, GLint normalTextureLoc, GLint colorLoc, GLint modelLoc, GLint isTextureLoc, mat4 modelView, bool drawEdge);
};

class Cuboid : public Shape
{
public:
    Cuboid(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, vec3* _color, int _tag);

    float x1, y1, z1, x2, y2, z2;
    float width, length, height, cx, cy, cz;


    void checkTrigger(bool* triggerCoords, float cx2, float cy2, float cz2, float width2, float length2, float height2);
};

class Rect : public Shape
{
public:
    Rect(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, vec3* _color, bool _vertical, int _tag);
    float x1, y1, z1, x2, y2, z2;
    float width, length, height, cx, cy, cz;
    int tag;
    bool vertical;
};

class StripLine : public Shape
{
public:
    StripLine(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, vec3* _color, float interval);
};


class Cylinder : public Shape
{
public:
    Cylinder(float _cx, float _cy, float _cz, float _radX, float radY, float _h, vec3* _color, int _tag);
    float cx, cy, cz, radX, radY, h;
    int tag;
};

class Triangle : public Shape
{
public:
    Triangle(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, float _x3, float _y3, float _z3, vec3* _color, int _tag);
    float x1, y1, z1, x2, y2, z2, x3, y3, z3;
    int tag;
};
#include "shape.h"

void Shape::setVertexArray(const vec3* positions, int size) {
    if (size)
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Position
        glEnableVertexAttribArray(0);
    }
}


void Shape::draw(GLint colorLoc, GLint modelLoc, mat4 modelView, bool drawEdge)
{
    glBindVertexArray(vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glUniform3f(colorLoc, color.r, color.g, color.b);     //set Uniform color of fragMent Shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(modelView));
    glDrawArrays(shapeType, 0, vertexCount);

    if (drawEdge)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(2);
        glUniform3f(colorLoc, 0, 0, 0);
        glDrawArrays(shapeType, 0, vertexCount);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}


Cuboid::Cuboid(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, float _r, float _g, float _b, float _tag)
    :x1(_x1), y1(_y1), z1(_z1), x2(_x2), y2(_y2), z2(_z2), r(_r), g(_g), b(_b), tag(_tag)
{
    //set variables
    shapeType = GL_QUADS;
    width = x2 - x1;
    length = y2 - y1;
    height = z2 - z1;
    cx = (x1 + x2) / 2;
    cy = (y1 + y2) / 2;
    cz = (z1 + z2) / 2;

    //set vertex position list
    vec3 positions[24];
    vertexCount = 24;
    int i = 0;

    positions[i++] = vec3(x1, y1, z1); //down
    positions[i++] = vec3(x1, y2, z1);
    positions[i++] = vec3(x2, y2, z1);
    positions[i++] = vec3(x2, y1, z1);

    positions[i++] = vec3(x1, y1, z2); // up
    positions[i++] = vec3(x2, y1, z2);
    positions[i++] = vec3(x2, y2, z2);
    positions[i++] = vec3(x1, y2, z2);

    positions[i++] = vec3(x1, y1, z1); // front
    positions[i++] = vec3(x2, y1, z1);
    positions[i++] = vec3(x2, y1, z2);
    positions[i++] = vec3(x1, y1, z2);

    positions[i++] = vec3(x1, y2, z1); // back
    positions[i++] = vec3(x1, y2, z2);
    positions[i++] = vec3(x2, y2, z2);
    positions[i++] = vec3(x2, y2, z1);

    positions[i++] = vec3(x1, y1, z1); // left
    positions[i++] = vec3(x1, y1, z2);
    positions[i++] = vec3(x1, y2, z2);
    positions[i++] = vec3(x1, y2, z1);

    positions[i++] = vec3(x2, y1, z1); // right
    positions[i++] = vec3(x2, y2, z1);
    positions[i++] = vec3(x2, y2, z2);
    positions[i++] = vec3(x2, y1, z2);

    color = vec3(r, g, b); //set color

    setVertexArray(positions, sizeof(positions));
};

void Cuboid::checkTrigger(bool* triggerCoords, float cx2, float cy2, float cz2, float width2, float length2, float height2)
{
    float diffX = abs(cx - cx2);
    float diffY = abs(cy - cy2);
    float diffZ = abs(cz - cz2);

    triggerCoords[0] = diffX < 0.5 * (width + width2);
    triggerCoords[1] = diffY < 0.5 * (length + length2);
    triggerCoords[2] = diffZ < 0.5 * (height + height2);
}

Rect::Rect(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, float _r, float _g, float _b, bool _vertical, float _tag)
    :x1(_x1), y1(_y1), z1(_z1), x2(_x2), y2(_y2), z2(_z2), r(_r), g(_g), b(_b), vertical(_vertical), tag(_tag)
{
    //set variables
    shapeType = GL_QUADS;
    width = x2 - x1;
    length = y2 - y1;
    height = z2 - z1;
    cx = (x1 + x2) / 2;
    cy = (y1 + y2) / 2;
    cz = (z1 + z2) / 2;

    //set vertex position list
    vec3 positions[4];
    vertexCount = 4;
    int i = 0;

    if (vertical)
    {
        positions[i++] = vec3(x1, y1, z1);
        positions[i++] = vec3(x2, y2, z1);
        positions[i++] = vec3(x2, y2, z2);
        positions[i++] = vec3(x1, y1, z2);
    }

    else // orthogonal to xz-plane
    {
        positions[i++] = vec3(x1, y1, z1);
        positions[i++] = vec3(x1, y2, z1);
        positions[i++] = vec3(x2, y2, z2);
        positions[i++] = vec3(x2, y1, z2);
    }

    color = vec3(r, g, b); //set color

    setVertexArray(positions, sizeof(positions));
};

StripLine::StripLine(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, float _r, float _g, float _b, float interval)
{
    shapeType = GL_LINES;
    int n = (int)(sqrt(pow(_x1-_x2,2)+ pow(_y1 - _y2, 2)+ pow(_z1 - _z2, 2))/ interval);
    vec3 unitVec = normalize(vec3(_x2 - _x1, _y2 - _y1, _z2 - _z1));

    vector<vec3> positions;

    for (int i = 0; i < n; i+=2)
    {
        positions.push_back(vec3(_x1, _y1, _z1) + unitVec * (interval * i));
        positions.push_back(vec3(_x1, _y1, _z1) + unitVec * (interval * (i+1)));
    }

    color = vec3(_r, _g, _b);
    vertexCount = positions.size();

    setVertexArray(positions.data(), sizeof(vec3)*vertexCount);
}


Cylinder::Cylinder(float _cx, float _cy, float _cz, float _radX, float _radY, float _h, float _r, float _g, float _b, float _tag)
    :cx(_cx), cy(_cy), cz(_cz), radX(_radX), radY(_radY), h(_h), r(_r), g(_g), b(_b), tag(_tag)
{
    //set variables
    shapeType = GL_TRIANGLE_STRIP;
    float upperZ = cz + 0.5 * h;
    float lowerZ = cz - 0.5 * h;

    //set vertex position list
    vec3 positions[120];
    vertexCount = 120; // 6 * 20
    int i = 0;

    for (int j = 0; j < 20; j++)
    {
        float theta = 2 * M_PI * j / 20;
        float nTheta = 2 * M_PI * (j + 1) / 20;

        positions[i++] = vec3(cx, cy, upperZ);
        positions[i++] = vec3(cx + radX * cos(theta), cy + radY * sin(theta), upperZ);
        positions[i++] = vec3(cx + radX * cos(nTheta), cy + radY * sin(nTheta), upperZ);
        positions[i++] = vec3(cx + radX * cos(theta), cy + radY * sin(theta), lowerZ);
        positions[i++] = vec3(cx + radX * cos(nTheta), cy + radY * sin(nTheta), lowerZ);
        positions[i++] = vec3(cx, cy, lowerZ);
    }

    color = vec3(r, g, b); //set color

    setVertexArray(positions, sizeof(positions));
}

Triangle::Triangle(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, float _x3, float _y3, float _z3, float _r, float _g, float _b, float _tag)
    :x1(_x1), y1(_y1), z1(_z1), x2(_x2), y2(_y2), z2(_z2), x3(_x3), y3(_y3), z3(_z3), r(_r), g(_g), b(_b), tag(_tag)
{
    //set variables
    shapeType = GL_POLYGON;

    //set vertex position list
    vec3 positions[3];
    vertexCount = 3;
    int i = 0;

    positions[i++] = vec3(x1, y1, z1);
    positions[i++] = vec3(x2, y2, z2);
    positions[i++] = vec3(x3, y3, z3);

    color = vec3(r, g, b);

    setVertexArray(positions, sizeof(positions));
}
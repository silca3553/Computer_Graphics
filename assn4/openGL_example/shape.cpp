#include "shape.h"

void Shape::setVertexArray(const vec3* positions, const vec3* normals, const vec2* texCoords, int positionSize, int texCoordSize) {
    if (positionSize)
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, (int)(positionSize * 2 + texCoordSize), NULL, GL_STATIC_DRAW);

        glBufferSubData(GL_ARRAY_BUFFER, 0, positionSize, positions);
        glBufferSubData(GL_ARRAY_BUFFER, positionSize, positionSize, normals);
        glBufferSubData(GL_ARRAY_BUFFER, 2 * positionSize, texCoordSize, texCoords);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Position
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(positionSize)); // Normal
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(texCoordSize)); // texCoord
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
    }
}

void Shape::setMode(bool _isDiffuse, bool _isNormal)
{
    isDiffuse = _isDiffuse;
    isNormal = _isNormal;
}

void Shape::setTexture(const Texture& texture, bool type)
{
    if (type == 0) diffuseTextureID = texture.textureID;
    else if (type == 1) normalTextureID = texture.textureID;
}


void Shape::draw(GLint diffuseTextureLoc, GLint normalTextureLoc, GLint colorLoc, GLint modelLoc, GLint isTextureLoc,mat4 modelView, bool drawEdge)
{
    glBindVertexArray(vao);

    if (isDiffuse)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTextureID);
        glUniform1i(diffuseTextureLoc, 0);
        glUniform3f(colorLoc, 1.0, 1.0, 1.0);
    }
    else glUniform3f(colorLoc, color.r, color.g, color.b);

    if (isNormal)
    {   
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTextureID);
        glUniform1i(normalTextureLoc, 1);
        glUniform1i(isTextureLoc, tag);
    }

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(modelView));
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(shapeType, 0, vertexCount);
    
    if (drawEdge)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(2);
        glUniform3f(colorLoc, 0, 0, 0);
        glDrawArrays(shapeType, 0, vertexCount);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glBindVertexArray(0);
}


Cuboid::Cuboid(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, vec3* _color, int _tag)
    :x1(_x1), y1(_y1), z1(_z1), x2(_x2), y2(_y2), z2(_z2)
{
    tag = _tag;
    //set variables
    shapeType = GL_QUADS;
    width = x2 - x1;
    length = y2 - y1;
    height = z2 - z1;
    cx = (x1 + x2) / 2;
    cy = (y1 + y2) / 2;
    cz = (z1 + z2) / 2;

    //set vertex position list
    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec2> texCoords;

    int i = 0;

    vec3 normVecs[8];
    vec3 ratio = vec3(width, length, height);
    normVecs[0] = vec3(0, 0, 1); // for upper plane
    normVecs[1] = vec3(0, 0, -1); // for lower plane
    normVecs[2] = vec3(0, -1, 0); // for front plane
    normVecs[3] = vec3(0, 1, 0); // for back plane
    normVecs[4] = vec3(-1, 0, 0); // for left plane
    normVecs[5] = vec3(1, 0, 0); // for right plane

    float nx, ny, nz;
    float unit = 0.1;
    for (float x = x1; x < x2; x += unit)
    {
        nx = x + unit;
        if (nx > x2) nx = x2;
        for (float y = y1; y < y2; y += unit)
        {
            ny = y + unit;
            if (ny > y2) ny = y2;
            positions.push_back(vec3(x, y, z1)); //down
            texCoords.push_back(vec2(0, 1));
            normals.push_back(normVecs[1]);
            positions.push_back(vec3(x, ny, z1));
            texCoords.push_back(vec2(0, 0));
            normals.push_back(normVecs[1]);
            positions.push_back(vec3(nx, ny, z1));
            texCoords.push_back(vec2(1, 0));
            normals.push_back(normVecs[1]);
            positions.push_back(vec3(nx, y, z1));
            texCoords.push_back(vec2(1, 1));
            normals.push_back(normVecs[1]);

            positions.push_back(vec3(x, y, z2)); //up
            texCoords.push_back(vec2(0, 0));
            normals.push_back(normVecs[0]);
            positions.push_back(vec3(nx, y, z2));
            texCoords.push_back(vec2(1, 0));
            normals.push_back(normVecs[0]);
            positions.push_back(vec3(nx, ny, z2));
            texCoords.push_back(vec2(1, 1));
            normals.push_back(normVecs[0]);
            positions.push_back(vec3(x, ny, z2));
            texCoords.push_back(vec2(0, 1));
            normals.push_back(normVecs[0]);
        }
    }


    for (float x = x1; x < x2; x += unit)
    {
        nx = x + unit;
        if (nx > x2) nx = x2;
        for (float z = z1; z < z2; z += unit)
        {
            nz = z + unit;
            if (nz > z2) nz = z2;
            positions.push_back(vec3(x, y1, z)); //front
            texCoords.push_back(vec2(0, 0));
            normals.push_back(normVecs[2]);
            positions.push_back(vec3(nx, y1, z));
            texCoords.push_back(vec2(1, 0));
            normals.push_back(normVecs[2]);
            positions.push_back(vec3(nx, y1, nz));
            texCoords.push_back(vec2(1, 1));
            normals.push_back(normVecs[2]);
            positions.push_back(vec3(x, y1, nz));
            texCoords.push_back(vec2(0, 1));
            normals.push_back(normVecs[2]);

            positions.push_back(vec3(x, y2, z)); //back
            texCoords.push_back(vec2(0, 0));
            normals.push_back(normVecs[3]);
            positions.push_back(vec3(x, y2, nz));
            texCoords.push_back(vec2(0, 1));
            normals.push_back(normVecs[3]);
            positions.push_back(vec3(nx, y2, nz));
            texCoords.push_back(vec2(1, 1));
            normals.push_back(normVecs[3]);
            positions.push_back(vec3(nx, y2, z));
            texCoords.push_back(vec2(1, 0));
            normals.push_back(normVecs[3]);
        }
    }

    for (float y = y1; y < y2; y += unit)
    {
        ny = y + unit;
        if (ny > y2) ny = y2;
        for (float z = z1; z < z2; z += unit)
        {
            nz = z + unit;
            if (nz > z2) nz = z2;
            positions.push_back(vec3(x1, y, z)); //left
            texCoords.push_back(vec2(1, 0));
            normals.push_back(normVecs[4]);
            positions.push_back(vec3(x1, y, nz));
            texCoords.push_back(vec2(1, 1));
            normals.push_back(normVecs[4]);
            positions.push_back(vec3(x1, ny, nz));
            texCoords.push_back(vec2(0, 1));
            normals.push_back(normVecs[4]);
            positions.push_back(vec3(x1, ny, z));
            texCoords.push_back(vec2(0, 0));
            normals.push_back(normVecs[4]);

            positions.push_back(vec3(x2, y, z)); //right
            texCoords.push_back(vec2(0, 0));
            normals.push_back(normVecs[5]);
            positions.push_back(vec3(x2, ny, z));
            texCoords.push_back(vec2(1, 0));
            normals.push_back(normVecs[5]);
            positions.push_back(vec3(x2, ny, nz));
            texCoords.push_back(vec2(1, 1));
            normals.push_back(normVecs[5]);
            positions.push_back(vec3(x2, y, nz));
            texCoords.push_back(vec2(0, 1));
            normals.push_back(normVecs[5]);
        }
    }

    vertexCount = positions.size();
    color = *_color;
    
    setVertexArray(positions.data(), normals.data(), texCoords.data(), sizeof(vec3) * vertexCount, sizeof(vec2) * vertexCount);
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

Rect::Rect(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, vec3* _color, bool _vertical, int _tag)
    :x1(_x1), y1(_y1), z1(_z1), x2(_x2), y2(_y2), z2(_z2), vertical(_vertical), tag(_tag)
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
    vec3 normals[4];
    vec2 texCoords[4];
    vertexCount = 4;
    int i = 0;

    if (vertical)
    {
        vec3 vertNormal = vec3(length * height, -width * height, 0);
        positions[i] = vec3(x1, y1, z1);
        texCoords[i] = vec2(0, 0);
        normals[i++] = vertNormal;
        positions[i] = vec3(x2, y2, z1);
        texCoords[i] = vec2(1, 0);
        normals[i++] = vertNormal;
        positions[i] = vec3(x2, y2, z2);
        texCoords[i] = vec2(1, 1);
        normals[i++] = vertNormal;
        positions[i] = vec3(x1, y1, z2);
        texCoords[i] = vec2(0, 1);
        normals[i++] = vertNormal;
    }

    else // orthogonal to xz-plane
    {
        vec3 horiNormal = vec3(-length * height, 0, width * length);
        positions[i] = vec3(x1, y1, z1);
        texCoords[i] = vec2(0, 0);
        normals[i++] = horiNormal;
        positions[i] = vec3(x2, y1, z2);
        texCoords[i] = vec2(0, 1);
        normals[i++] = horiNormal;
        positions[i] = vec3(x2, y2, z2);
        texCoords[i] = vec2(1, 1);
        normals[i++] = horiNormal;
        positions[i] = vec3(x1, y2, z1);
        texCoords[i] = vec2(1, 0);
        normals[i++] = horiNormal;
    }
    color = *_color;
    setVertexArray(positions, normals, texCoords, sizeof(positions), sizeof(texCoords));
};

StripLine::StripLine(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, vec3* _color, float interval)
{
    shapeType = GL_LINES;
    int n = (int)(sqrt(pow(_x1 - _x2, 2) + pow(_y1 - _y2, 2) + pow(_z1 - _z2, 2)) / interval);
    vec3 unitVec = normalize(vec3(_x2 - _x1, _y2 - _y1, _z2 - _z1));

    vector<vec3> positions;
    vector<vec3> normals;
    vector<vec2> texCoords;

    for (int i = 0; i < n; i += 2)
    {
        positions.push_back(vec3(_x1, _y1, _z1) + unitVec * (interval * i));
        positions.push_back(vec3(_x1, _y1, _z1) + unitVec * (interval * (i + 1)));
        normals.push_back(vec3(0, 0, 1));
        normals.push_back(vec3(0, 0, 1));
        texCoords.push_back(vec2(0, 0));
        texCoords.push_back(vec2(1, 0));
    }

    color = *_color;
    vertexCount = positions.size();

    setVertexArray(positions.data(), normals.data(), texCoords.data(), sizeof(vec3) * vertexCount, sizeof(vec2) * vertexCount);
}


Cylinder::Cylinder(float _cx, float _cy, float _cz, float _radX, float _radY, float _h, vec3* _color, int _tag)
    :cx(_cx), cy(_cy), cz(_cz), radX(_radX), radY(_radY), h(_h), tag(_tag)
{
    //set variables
    shapeType = GL_TRIANGLE_STRIP;
    float upperZ = cz + 0.5 * h;
    float lowerZ = cz - 0.5 * h;

    //set vertex position list
    vec3 positions[120];
    vec3 normals[120];
    vec2 texCoords[120];
    vertexCount = 120; // 6 * 20
    int i = 0;

    for (int j = 0; j < 20; j++)
    {
        float theta = 2 * M_PI * j / 20;
        float nTheta = 2 * M_PI * (j + 1) / 20;

        positions[i] = vec3(cx, cy, upperZ);
        texCoords[i] = vec2(0, 0);
        normals[i++] = vec3(0, 0, 1);
        positions[i] = vec3(cx + radX * cos(theta), cy + radY * sin(theta), upperZ);
        texCoords[i] = vec2(1, 0);
        normals[i++] = vec3(cos(theta), sin(theta), sqrt(2) / 2);
        positions[i] = vec3(cx + radX * cos(nTheta), cy + radY * sin(nTheta), upperZ);
        texCoords[i] = vec2(0, 1);
        normals[i++] = vec3(cos(nTheta), sin(nTheta), sqrt(2) / 2);
        positions[i] = vec3(cx + radX * cos(theta), cy + radY * sin(theta), lowerZ);
        texCoords[i] = vec2(0, 0);
        normals[i++] = vec3(cos(theta), sin(theta), -sqrt(2) / 2);
        positions[i] = vec3(cx + radX * cos(nTheta), cy + radY * sin(nTheta), lowerZ);
        texCoords[i] = vec2(1, 0);
        normals[i++] = vec3(cos(nTheta), sin(nTheta), -sqrt(2) / 2);
        positions[i] = vec3(cx, cy, lowerZ);
        texCoords[i] = vec2(0, 1);
        normals[i++] = vec3(0, 0, -1);
    }
    color = *_color;
    setVertexArray(positions, normals, texCoords, sizeof(positions), sizeof(texCoords));
}

Triangle::Triangle(float _x1, float _y1, float _z1, float _x2, float _y2, float _z2, float _x3, float _y3, float _z3, vec3* _color, int _tag)
    :x1(_x1), y1(_y1), z1(_z1), x2(_x2), y2(_y2), z2(_z2), x3(_x3), y3(_y3), z3(_z3), tag(_tag)
{
    //set variables
    shapeType = GL_POLYGON;

    //set vertex position list
    vec3 positions[3];
    vec3 normals[3];
    vec2 texCoords[3];
    vertexCount = 3;
    int i = 0;
    vec3 v1 = vec3(x2 - x1, y2 - y1, z2 - z1);
    vec3 v2 = vec3(x3 - x2, y3 - y2, z3 - z2);
    vec3 normal = cross(v1, v2);

    positions[i] = vec3(x1, y1, z1);
    texCoords[i] = vec2(0, 0);
    normals[i++] = normal;
    positions[i] = vec3(x2, y2, z2);
    texCoords[i] = vec2(1, 0);
    normals[i++] = normal;
    positions[i] = vec3(x3, y3, z3);
    texCoords[i] = vec2(1, 0.5);
    normals[i++] = normal;

    color = *_color;
    setVertexArray(positions, normals, texCoords, sizeof(positions), sizeof(texCoords));
}
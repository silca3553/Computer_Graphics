#pragma once

#include <GL/glew.h>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "matrixStack.h"
#include "shape.h"

using namespace std;
using namespace glm;

class Light
{
public:
	vec3 diffuse, specular;
	float shininess;
	vec4 lightPosition;
	float x, y, z;
	Light();
	Light(vec3 _diffuse, vec3 _specular, float _shininess);
};

class DirLight : public Light {
public:
	float radius;
	float theta;
	vec3 direction;
	bool movingDirection = false; // false: going left, true: going right
	float dtheta = 0.003;
	Cylinder* cylinder;
	bool isDiffuse, isNormal;
	DirLight(vec3 _diffuse, vec3 _specular, float _shininess, Texture* texture, bool _isDiffuse, bool _isNormal);
	void move();
	void draw(GLint diffuseTextureLoc, GLint normalTextureLoc, GLint dirDLoc, GLint dirSLoc, GLint dirLightLoc, GLint dirShineLoc, GLint colorLoc, GLint modelLoc, GLint isTextureLoc, MatrixStack* matrixStack, bool viewingEdge, bool _isDiffuse, bool _isNormal);
};

class PtLight : public Light {
public:
	bool isDiffuse, isNormal;
	PtLight(vec3 _diffuse, vec3 _specular, float _shininess, Texture* texture, bool _isDiffuse, bool _isNormal);
	Cylinder* cylinder;
	void move(float x, float y, float z);
	void draw(GLint diffuseTextureLoc, GLint normalTextureLoc, GLint ptDLoc, GLint ptSLoc, GLint ptLightLoc, GLint ptShineLoc, GLint colorLoc, GLint modelLoc, GLint isTextureLoc, MatrixStack* matrixStack, bool viewingEdge, bool _isDiffuse, bool _isNormal);
};
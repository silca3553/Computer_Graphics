#pragma once

#include <GL/glew.h>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using namespace std;
using namespace glm;

class MatrixStack
{
public:
	vector<mat4> stack;
	int size = 0;

	void loadIdentity();
	void translatef(float x, float y, float z);
	void rotatef(float angle, float x, float y, float z);
	void pushMatrix();
	void popMatrix();
	void dot(mat4 matrix);
	mat4 get();
};
#include "matrixStack.h"

void MatrixStack::loadIdentity()
{
	stack.clear();
	stack.push_back(identity<mat4>());
	size = 1;
}

void MatrixStack::translatef(float x, float y, float z)
{
	vec3 translateVec(x, y, z);
	stack[size - 1] = translate(stack[size - 1], translateVec);
}

void MatrixStack::rotatef(float angle, float x, float y, float z)
{
	vec3 rotationAxis(x, y, z);
	stack[size - 1] = rotate(stack[size - 1], radians(angle), rotationAxis);
}

void MatrixStack::pushMatrix()
{
	stack.push_back(stack[size - 1]);
	size += 1;
}

void MatrixStack::popMatrix()
{
	stack.pop_back();
	size -= 1;
}

void MatrixStack::dot(mat4 matrix)
{
	stack[size - 1] *= matrix;
}

mat4 MatrixStack::get()
{
	return stack[size - 1];
}
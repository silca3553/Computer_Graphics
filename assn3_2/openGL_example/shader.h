#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>


using namespace std;

class Shader
{
public:
	GLint vShader, fShader, program;

	Shader(const char* vShaderPath, const char* fShaderPath);
	void run();
	void clean();
	GLint loadShader(const char* fileName, GLenum shaderType);
};
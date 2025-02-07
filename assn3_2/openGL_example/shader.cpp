#include "shader.h"
#include <sstream>


Shader::Shader(const char* vShaderPath, const char* fShaderPath)
{
	program = glCreateProgram();
	vShader = loadShader(vShaderPath, GL_VERTEX_SHADER);
	fShader = loadShader(fShaderPath, GL_FRAGMENT_SHADER);
	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
}

GLint Shader::loadShader(const char* filePath, GLenum shaderType)
{
	ifstream file(filePath);

	stringstream stringBuff;
	stringBuff << file.rdbuf();
	string shaderCodeSring = stringBuff.str();
	const char* shaderCode = shaderCodeSring.c_str();

	GLuint shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &shaderCode, NULL);
	glCompileShader(shaderId);

	GLint success;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		std::cerr << filePath << " failed: " << infoLog << std::endl;
	}

	return shaderId;
}

void Shader::run()
{
	glLinkProgram(program);
	glUseProgram(program);
}

void Shader::clean()
{
	glDeleteShader(vShader);
	glDeleteShader(fShader);
}
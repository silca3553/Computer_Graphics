#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <fstream>

#include "stb_image.h"

class Texture {
public:
    GLuint textureID;
    Texture(const char* path);
    GLuint getTextureID();
};
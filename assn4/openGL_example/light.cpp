#include "light.h"
#include "shader.h"

Light::Light()
{

}

Light::Light(vec3 _diffuse, vec3 _specular, float _shininess)
{
    diffuse = _diffuse;
    specular = _specular;
    shininess = _shininess;
}

DirLight::DirLight(vec3 _diffuse, vec3 _specular, float _shininess, Texture* texture, bool _isDiffuse, bool _isNormal)
{
    radius = 1.4f;
    theta = 0.0;
    x = radius * cos(theta);
    y = -0.1;
    z = radius * sin(theta);
    lightPosition = vec4(x, y, z, 1.0);

    diffuse = _diffuse;
    specular = _specular;
    shininess = _shininess;

    isDiffuse = _isDiffuse;
    isNormal = _isNormal;

    vec3 yellow = vec3(1.0f, 1.0f, 0.0f);
    cylinder = new Cylinder(0, 0, -0.1, 0.07, 0.07, 0.01, &yellow, 0);
    cylinder->setTexture(*texture, 0);
}

void DirLight::move()
{

    if (theta >= (1.1*M_PI))
    {
        movingDirection = true;
        theta = 1.1 * M_PI;
    }
    else if (theta <= -0.1* M_PI)
    {
        movingDirection = false;
        theta = -0.1 * M_PI;
    }

    if (movingDirection == false) theta += dtheta;
    else theta -= dtheta;

    x = radius * cos(theta);
    y = -0.001f;
    z = radius * sin(theta);

    lightPosition = vec4(x, y, z, 1.0);
}

void DirLight::draw(GLint diffuseTextureLoc, GLint normalTextureLoc, GLint dirDLoc, GLint dirSLoc, GLint dirLightLoc, GLint dirShineLoc, GLint colorLoc, GLint modelLoc, GLint isTextureLoc, MatrixStack* matrixStack, bool viewingEdge, bool _isDiffuse, bool _isNormal)
{
    // directional light
    //vec4 lightPos = (look * lightPosition) - (look * vec4(0, 0, 0, 1));
    //lightPos.w = 0;
    glUniform3f(dirDLoc, diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(dirSLoc, specular.x, specular.y, specular.z);
    glUniform4f(dirLightLoc, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
    glUniform1f(dirShineLoc, shininess);

    isDiffuse = _isDiffuse;
    isNormal = _isNormal;

    matrixStack->pushMatrix();
    matrixStack->translatef(lightPosition.x, lightPosition.y, lightPosition.z);
    matrixStack->rotatef(90, 1, 0, 0);
    cylinder->setMode(isDiffuse, isNormal);
    cylinder->draw(diffuseTextureLoc, normalTextureLoc, colorLoc, modelLoc, isTextureLoc, matrixStack->get(), viewingEdge);
    matrixStack->popMatrix();
}

PtLight::PtLight(vec3 _diffuse, vec3 _specular, float _shininess, Texture* texture, bool _isDiffuse, bool _isNormal)
{
    x = 0.0f;
    y = -7.0 / 8 - 0.1f;
    z = 0.5;
    lightPosition = vec4(x, y, z, 1.0);

    diffuse = _diffuse;
    specular = _specular;
    shininess = _shininess;

    isDiffuse = _isDiffuse;
    isNormal = _isNormal;

    vec3 yellow = vec3(1.0f, 1.0f, 0.0f);
    cylinder = new Cylinder(0, 0, 0, 0.01, 0.01, 0.03, &yellow, 0);
    cylinder->setTexture(*texture, 0);
}

void PtLight::move(float x, float y, float z)
{
    lightPosition = vec4(x, y, z, 1.0);
}

void PtLight::draw(GLint diffuseTextureLoc, GLint normalTextureLoc, GLint ptDLoc, GLint ptSLoc, GLint ptLightLoc, GLint ptShineLoc, GLint colorLoc, GLint modelLoc, GLint isTextureLoc, MatrixStack* matrixStack, bool viewingEdge, bool _isDiffuse, bool _isNormal)
{
    // point light
    //vec4 lightPos = look * lightPosition;
    glUniform3f(ptDLoc, diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(ptSLoc, specular.x, specular.y, specular.z);
    glUniform4f(ptLightLoc, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
    glUniform1f(ptShineLoc, shininess);

    isDiffuse = _isDiffuse;
    isNormal = _isNormal;

    matrixStack->pushMatrix();
    matrixStack->translatef(lightPosition.x, lightPosition.y, lightPosition.z);
    cylinder->setMode(isDiffuse, isNormal);
    cylinder->draw(diffuseTextureLoc, normalTextureLoc, colorLoc, modelLoc, isTextureLoc, matrixStack->get(), viewingEdge);
    matrixStack->popMatrix();
}
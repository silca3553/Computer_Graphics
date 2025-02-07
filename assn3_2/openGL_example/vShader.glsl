#version 460

layout(location = 0) in vec3 vPosition;

uniform mat4 modelView;
uniform float time;
uniform int effectMode;
uniform mat4 projection;

void main()
{
    if(effectMode == 4)
    {
        vec3 effPosition;
        effPosition.x =  vPosition.x * ( 1 +  0.1*sin(time*10+vPosition.z*10));
        effPosition.z =  vPosition.z * ( 1 +  0.1*sin(time*10+vPosition.y*10));
        effPosition.y =  vPosition.y;
        gl_Position = projection* modelView * vec4( effPosition, 1.0);
    }

    else
    {
        gl_Position = projection* modelView * vec4(vPosition, 1.0);
    }
}
#version 460

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

uniform mat4 projection;
uniform mat4 lookAt;
uniform mat4 modelView;

uniform vec4 dirLightPosition;
uniform vec4 ptLightPosition;

uniform float time;
uniform int effectMode;

out vec3 dirL;
out vec3 dirN;
out vec3 dirE;

out vec3 ptL;
out vec3 ptN;
out vec3 ptE;

out vec2 texCoord;

void setVectors(vec4 lightPosition, inout vec3 L, inout vec3 N, inout vec3 E)
{
	vec3 pos = (modelView * vec4(vPosition, 1.0)).xyz;
	N = normalize((modelView * vec4(vNormal,1.0)).xyz - (modelView * vec4(0,0,0,1)).xyz);
	L = normalize(lightPosition.xyz);
	E = normalize( -pos);

	if(lightPosition.w != 0.0)
	{
		L = normalize(lightPosition.xyz - pos);
	}
}

void main()
{

	vec4 dirLightPos = (lookAt * dirLightPosition) - (lookAt * vec4(0, 0, 0, 1));
	dirLightPos.w =0;
    setVectors(dirLightPos,dirL,dirN,dirE);

	vec4 ptLightPos = lookAt * ptLightPosition;
    setVectors(ptLightPos,ptL,ptN,ptE);

	texCoord = vTexCoord;
	
	if(effectMode == 1)
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
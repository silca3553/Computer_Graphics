#version 460

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
out vec4 vColor;
out vec2 texCoord;

uniform vec3 ambientCoff;
uniform vec3 dirDcoff, dirScoff;
uniform vec3 ptDcoff, ptScoff;

uniform mat4 modelView;
uniform mat4 lookAt;
uniform mat4 projection;

uniform vec4 dirLightPosition;
uniform float dirShininess;

uniform vec4 ptLightPosition;
uniform float ptShininess;

uniform vec3 color;
uniform float time;
uniform int effectMode;


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

vec3 getColor(vec3 diffuseCoff, vec3 specularCoff ,vec3 L, vec3 N, vec3 E, float shininess)
{
	vec3 H = normalize(L+E);
	
	vec3 diffuse = diffuseCoff * max(dot(L,N),0.0);
	vec3 specular = specularCoff * pow(max( dot(N,H) ,0.0),shininess);

	if(dot(L,N) < 0.0)
	{	
		specular = vec3(0.0,0.0,0.0);
	}

	return diffuse * color +  specular * vec3(1,1,1);
}

void main()
{
    vec3 dirL,dirN,dirE;
    vec3 ptL, ptN, ptE;

	vec4 dirLightPos = (lookAt * dirLightPosition) - (lookAt * vec4(0, 0, 0, 1));
	dirLightPos.w =0;
    setVectors(dirLightPos,dirL,dirN,dirE);

	vec4 ptLightPos = lookAt * ptLightPosition;
    setVectors(ptLightPos,ptL,ptN,ptE);

	vec3 dirColor = getColor(dirDcoff, dirScoff, dirL, dirN, dirE, dirShininess);
	vec3 ptColor =  getColor(ptDcoff, ptScoff, ptL, ptN, ptE, ptShininess);

	vColor.rgb = ambientCoff * color + dirColor + ptColor;
	vColor.a = 1.0;

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
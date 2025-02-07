#version 460

in vec3 dirL;
in vec3 dirN;
in vec3 dirE;
in vec3 ptL;
in vec3 ptN;
in vec3 ptE;
in vec2 texCoord;

out vec4 outColor;

uniform vec3 color;
uniform float dirShininess;
uniform float ptShininess;
uniform vec3 ambientCoff;
uniform vec3 dirDcoff, dirScoff;
uniform vec3 ptDcoff, ptScoff;

uniform sampler2D DiffuseTexture;
uniform sampler2D NormalTexture;
uniform bool IsDiffuse;
uniform bool IsNormal;
uniform bool IsTexture;
uniform mat4 modelView;

vec3 getColor(vec3 diffuseCoff, vec3 specularCoff ,vec3 L, vec3 N, vec3 E, float shininess)
{
	vec3 H = normalize(L+E);
	
	vec3 diffuse = diffuseCoff * max(dot(L,N),0.0);
	vec3 specular = specularCoff * pow(max( dot(N,H) ,0.0),shininess);

	if(dot(L,N) < 0.0)
	{	
		specular = vec3(0.0,0.0,0.0);
	}

	return diffuse * color +  specular*vec3(1,1,1);
}

void main()
{
	vec3 dir_N = dirN;
	vec3 pt_N = ptN;
	if (IsNormal && IsTexture)
	{
		dir_N = texture2D(NormalTexture, texCoord).xyz;
		dir_N = normalize(dir_N * 2.0 -1.0);
		dir_N = normalize((modelView * vec4(dir_N,1) ).xyz - (modelView * vec4(0,0,0,1) ).xyz);
		pt_N = dir_N;
	}

	vec3 dirColor = getColor(dirDcoff, dirScoff, dirL, dir_N, dirE, dirShininess);
	vec3 ptColor =  getColor(ptDcoff, ptScoff, ptL, pt_N, ptE, ptShininess);

	vec4 finalColor;
	finalColor.rgb = ambientCoff * color + dirColor + ptColor;
	finalColor.a = 1.0;

	if (IsDiffuse)
	{
		outColor = vec4(texture2D(DiffuseTexture, texCoord)) * finalColor;
		outColor.a = 1.0;
	}
	else
	{
		outColor = finalColor;
		outColor.a = 1.0;
	}
	
}
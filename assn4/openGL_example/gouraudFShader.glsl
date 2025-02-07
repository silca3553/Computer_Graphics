#version 460
in vec4 vColor;
in vec2 texCoord;

out vec4 outColor; // Output color

uniform sampler2D DiffuseTexture;
uniform bool IsDiffuse;

void main() {
	if (IsDiffuse) outColor = vec4(texture2D(DiffuseTexture, texCoord)) * vColor;
	else outColor = vColor;
}

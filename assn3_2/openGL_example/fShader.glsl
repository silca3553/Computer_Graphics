#version 460

uniform vec3 color;
uniform float time;
uniform int effectMode;
out vec4 outColor; // Output color


void main() {
    if(effectMode == 1)
    {
        vec3 mixedColor = mix(vec3(1.0), color, 0.5 + 0.5* sin(time));
        outColor = vec4(mixedColor, 1.0);
    }
    
    else if(effectMode == 2)
    {
        float weight = dot(color,color);
        outColor = vec4(vec3(1,1,1)*weight, 1.0);
    }

    else if(effectMode == 3)
    {
        vec3 newColor;
        newColor.x = color.x * abs(sin(time)) * 2;
        newColor.y = color.y * abs(cos(2*time)) * 2;
        newColor.z = color.z * abs(sin(3*time)) * 2;
        outColor = vec4(newColor, 1.0);
    }

    else
    {
        outColor = vec4(color, 1.0);
    }

}

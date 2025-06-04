#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
#define MAX_LIGHTS 4

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace[MAX_LIGHTS];
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightSpaceMatrices[MAX_LIGHTS];


void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);
    vs_out.FragPos = worldPosition.xyz;
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;

    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        vs_out.FragPosLightSpace[i] = lightSpaceMatrices[i] * worldPosition;
    }

    gl_Position = projection * view * worldPosition;
}
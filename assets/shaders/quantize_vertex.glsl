#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;

out vec2 uUV;

void main()
{
    vec4 worldPosition = vec4(aPos, 1.0);
    gl_Position = worldPosition;
    uUV = aUV;
}
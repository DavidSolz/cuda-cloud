#version 410 core

layout(location = 0) in vec3 iVertex; 
layout(location = 1) in vec2 iUV; 

out vec2 vUV;

void main()
{
    gl_Position = vec4(iVertex, 1.0);
    vUV = iUV;
}
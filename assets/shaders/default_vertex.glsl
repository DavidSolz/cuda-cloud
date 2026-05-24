#version 410 core

layout(location = 0) in vec3 iVertex; 
layout(location = 1) in vec2 iUV; 

out vec2 vUV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(iVertex, 1.0);
    vUV = iUV;
}
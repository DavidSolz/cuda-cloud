#version 410 core

layout(location = 0) in vec3 iVertex; 
layout(location = 1) in vec3 iColor; 

out vec3 vColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(iVertex, 1.0);
    vColor = iColor;
}
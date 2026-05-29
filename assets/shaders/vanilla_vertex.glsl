#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

out vec2 vUV;
out vec3 vNormal;
out vec3 vPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    vec4 worldPosition = projectionMatrix * viewMatrix * modelMatrix * vec4(aPosition, 1.0);
    vUV = aUV;
    vNormal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    vPosition = vec3(modelMatrix * vec4(aPosition, 1.0));
    gl_Position = worldPosition;
}
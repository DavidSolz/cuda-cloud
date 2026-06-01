#version 410 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

layout(std140) uniform FramebufferDataBuffer {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    vec4 eyePosition;
    vec2 resolution;
    vec2 padding1;
    float time;
    vec3 padding2;
} framebufferData;

out VertexData {
    vec3 worldPosition;
    vec3 normal;
    vec2 uv;
} vertexData;

uniform mat4 modelMatrix;

void main()
{
    vec4 worldPosition = modelMatrix * vec4(vPosition, 1.0);
    gl_Position = framebufferData.projectionMatrix * framebufferData.viewMatrix * worldPosition;

    vertexData.worldPosition = worldPosition.xyz;
    vertexData.normal = mat3(transpose(inverse(modelMatrix))) * vNormal;
    vertexData.uv = vUV;
}
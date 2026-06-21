#version 410 core

layout(std140) uniform FramebufferDataBuffer {
    mat4 projectionMatrix;
    mat4 viewMatrix;
    vec4 eyePosition;
    vec2 resolution;
    vec2 padding1;
    float time;
    vec3 padding2;
} framebufferData;

in VertexData {
    vec3 worldPosition;
    vec3 normal;
    vec2 uv;
} vertexData;

uniform vec3 mAlbedo;
uniform float mRoughness;
uniform float mMetallic;
uniform float mClearcoat;

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gDiffuse;
layout(location = 3) out vec4 gTextureCoord;

void main()
{
    vec3 normal = normalize(vertexData.normal);

    gPosition = vec4(vertexData.worldPosition, mClearcoat);
    gNormal = vec4(normal, mRoughness);
    gDiffuse = vec4(mAlbedo, mMetallic);
    gTextureCoord = vec4(vertexData.uv, 0.0, 1.0);
}
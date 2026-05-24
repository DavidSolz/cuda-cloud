#version 410 core

out vec4 glColor;

in vec2 vUV;


uniform sampler2D colorMap;

void main()
{
    glColor = texture(colorMap, vUV);
}
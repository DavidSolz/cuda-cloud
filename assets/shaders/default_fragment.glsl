#version 410 core

out vec4 glColor;

in vec3 vColor;

void main()
{
    glColor = vec4(vColor, 1.0);
}
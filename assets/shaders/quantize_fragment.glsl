#version 410 core

out vec4 glColor;

in vec2 uUV;

uniform vec2 resolution;
uniform float cellWidth;

uniform sampler2D colorTexture;

void main()
{
    vec2 normalizedCoord = gl_FragCoord.xy / resolution.xy;

    vec2 cellUV = floor(normalizedCoord * resolution.xy / cellWidth) * cellWidth / resolution.xy + (cellWidth / 2.0) / resolution.xy;

    glColor = texture(colorTexture, cellUV);
}
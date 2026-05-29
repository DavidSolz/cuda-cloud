#version 410 core

out vec4 glColor;

in vec3 vPosition;
in vec3 vNormal;
in vec2 vUV;

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec3 objectColor;

void main()
{
    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPos - vPosition);
    vec3 V = normalize(cameraPos - vPosition);

    float dotNL = max(dot(N, L), 0.0);
    float dotNV = max(dot(N, V), 0.0);

    const float shininess = 32.0f;
    vec3 R = reflect(-L, N);
    float dotRV = max(dot(R, V), 0.0);
    vec3 specular = pow(dotRV, shininess) * vec3(1.0f);
    vec3 diffuse = dotNL * objectColor + specular;

    glColor = vec4(diffuse, 1.0f);
}
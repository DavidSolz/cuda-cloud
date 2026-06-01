#version 410 core

#define PI 3.14159265359

layout(std140) uniform LightDataBuffer
{
    vec3 position[10];
    vec3 color[10];
} lightDataBuffer;

in vec2 uv;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

layout(location = 0) out vec4 fragColor;

uniform vec3 mAlbedo;
uniform float mMetallic;
uniform float mRoughness;

uniform vec3 lightPos;
uniform vec3 cameraPos;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

void main()
{

    vec3 position = texture(gPosition, uv).xyz;
    vec3 normal = normalize(texture(gNormal, uv).xyz * 2.0 - 1.0);
    vec3 albedo = texture(gAlbedo, uv).rgb;

    vec3 view = normalize(cameraPos - position);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, mAlbedo, mMetallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < 10; i++)
    {
        vec3 light = normalize(lightDataBuffer.position[i].xyz - position);
        vec3 halfway = normalize(light + view);

        float distance = length(lightDataBuffer.position[i].xyz - position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightDataBuffer.color[i].rgb * attenuation;

        vec3 F = fresnelSchlick(max(dot(halfway, view), 0.0), F0);
        float NDF = DistributionGGX(normal, halfway, mRoughness);       
        float G = GeometrySmith(normal, view, light, mRoughness);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - mMetallic;	

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, light), 0.0)  + 0.0001;
        vec3 specular = numerator / denominator;  

        float NdotL = max(dot(normal, light), 0.0);        
        Lo += (kD * mAlbedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * mAlbedo;
    vec3 color = ambient + Lo; 

    fragColor = vec4(color, 1.0);
}
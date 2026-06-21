#version 410 core

#define PI 3.14159265359

layout(std140) uniform LightDataBuffer
{
    vec4 position[10];
    vec4 color[10];
} lightDataBuffer;

in vec2 uv;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gTextureCoord;
uniform float mClearCoatRoughness = 0.05f;

layout(location = 0) out vec4 fragColor;

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

float GeometrySchlickGGX_Base(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySchlickGGX_Coat(float NdotV, float roughness)
{
    float roughnessSquared = roughness * roughness;
    float k = (roughnessSquared * roughnessSquared) / 2.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness, bool isCoat)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = isCoat ? GeometrySchlickGGX_Coat(NdotV, roughness) : GeometrySchlickGGX_Base(NdotV, roughness);
    float ggx1  = isCoat ? GeometrySchlickGGX_Coat(NdotL, roughness) : GeometrySchlickGGX_Base(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

void main()
{

    vec4 positionData = texture(gPosition, uv);
    vec3 position = positionData.xyz;
    float clearcoat = positionData.w;

    vec4 normalData = texture(gNormal, uv);
    vec3 normal = normalize(normalData.xyz); 
    float roughness = normalData.w;

    vec4 diffuseData = texture(gDiffuse, uv);
    vec3 albedo = diffuseData.rgb;
    float metallic = diffuseData.w;

    vec4 textureCoordData = texture(gTextureCoord, uv);
    vec2 texCoord = textureCoordData.xy;

    vec3 view = normalize(cameraPos - position);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 F0_clearCoat = vec3(0.04);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < 10; i++)
    {
        vec3 light = normalize(lightDataBuffer.position[i].xyz - position);
        vec3 halfway = normalize(light + view);

        float distance = length(lightDataBuffer.position[i].xyz - position);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightDataBuffer.color[i].rgb * attenuation;

        float NdotL = max(dot(normal, light), 0.0);
        float NdotV = max(dot(normal, view), 0.0);

        vec3 F_base = fresnelSchlick(max(dot(halfway, view), 0.0), F0);
        float NDF_base = DistributionGGX(normal, halfway, roughness);       
        float G_base = GeometrySmith(normal, view, light, roughness, false);

        vec3 specular_base = (NDF_base * G_base * F_base) / (4.0 * NdotV * NdotL + 0.0001);
        vec3 kS_base = F_base;
        vec3 kD_base = (vec3(1.0) - kS_base) * (1.0 - metallic);
        vec3 diffuse_base = kD_base * albedo / PI;

        vec3 baseLayer = (diffuse_base + specular_base) * radiance * NdotL; 

        vec3 F_coat = fresnelSchlick(max(dot(halfway, view), 0.0), F0_clearCoat);
        float NDF_coat = DistributionGGX(normal, halfway, mClearCoatRoughness);
        float G_coat   = GeometrySmith(normal, view, light, mClearCoatRoughness, true);
        
        vec3 specular_coat = (NDF_coat * G_coat * F_coat) / (4.0 * NdotV * NdotL + 0.0001);

        float attenuation_coat = 1.0 - (F_coat.r * clearcoat);

        Lo += (baseLayer * attenuation_coat) + (specular_coat * clearcoat * radiance * NdotL);
    }

    vec3 ambient = vec3(0.03) * albedo * (1.0 - clearcoat * 0.04);
    vec3 color = ambient + Lo; 

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    fragColor = vec4(color, 1.0);
}
#version 410 core

out vec4 glColor;

in vec2 uUV;

uniform int numStepsLight;
uniform int numStepsView;

uniform float darknessThreshold;
uniform float densityBias;

uniform vec3 boundingBoxMin;
uniform vec3 boundingBoxMax;

uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec2 resolution;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;

uniform sampler3D densityTexture;

uniform mat4 invViewMatrix;
uniform mat4 invProjectionMatrix;

uniform float absorption;

float hash(vec2 p)  
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float sampleDensity(vec3 rayPosition)
{
    vec3 size = boundingBoxMax - boundingBoxMin;
    vec3 uvw = size * 0.5 + rayPosition;

    float density = texture(densityTexture, uvw / size).r;
    float exponent = (densityBias - 1.0) / (-densityBias - 1.0);

    float distanceToAABB = length(min(boundingBoxMin - rayPosition, rayPosition - boundingBoxMax));
    float maxDistance = length(size);
    float distanceFactor = 1.0 - clamp(distanceToAABB / maxDistance, 0.0, 1.0);

    return pow(density * distanceFactor, exponent);
}

vec2 rayBoxIntersection(vec3 rayOrigin, vec3 rayDirection, vec3 boxMin, vec3 boxMax)
{
    vec3 tMin = (boxMin - rayOrigin) / rayDirection;
    vec3 tMax = (boxMax - rayOrigin) / rayDirection;

    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);

    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);

    if (tNear > tFar || tFar < 0.0f)
    {
        return vec2(-1.0f); 
    }

    return vec2(max(0.0, tNear), tFar);
}

float lightMarch(vec3 position)
{
    vec3 lightDir = normalize(lightPos - position);
    vec2 dstInsideBox = rayBoxIntersection(position, lightDir, boundingBoxMin, boundingBoxMax);

    float dstInside = dstInsideBox.y - dstInsideBox.x;

    float stepSize = dstInside / numStepsLight;
    float totalDensity = 0.0;

    for (int i = 0; i < numStepsLight; i++)
    {
        position += lightDir * stepSize;
        totalDensity += max(0.0, sampleDensity(position) * stepSize);
    }

    float transmittance = exp(-totalDensity * absorption);
    return darknessThreshold + transmittance * (1.0 - darknessThreshold);
}

float getLinearDepth(vec2 uv)
{
    float depth = texture(depthTexture, uv).r;
    
    vec4 clipSpacePos = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 viewSpacePos = invProjectionMatrix * clipSpacePos;
    viewSpacePos /= viewSpacePos.w; 
    
    return -viewSpacePos.z; 
}
void main()
{
    vec2 texCoord = gl_FragCoord.xy / resolution.xy;
    vec4 clipPos = vec4(texCoord * 2.0 - 1.0, -1.0, 1.0);

    vec4 target = invProjectionMatrix * clipPos;
    vec3 rayDirection = normalize((invViewMatrix * vec4(normalize(target.xyz / target.w), 0.0)).xyz);
    vec3 rayOrigin = cameraPos;

    vec4 opaqueColor = texture(colorTexture, texCoord);
    float opaqueDepth = getLinearDepth(texCoord);

    vec3 viewForward = normalize((invViewMatrix * vec4(0.0, 0.0, -1.0, 0.0)).xyz);
    float geometryDistance = opaqueDepth / dot(rayDirection, viewForward);

    vec2 intersection = rayBoxIntersection(rayOrigin, rayDirection, boundingBoxMin, boundingBoxMax);
    float tNear = intersection.x;
    float tFar = intersection.y;

    if (tNear == -1.0f || tNear >= tFar || tNear >= geometryDistance)
    {
        glColor = opaqueColor;
        return;
    }
    tFar = min(tFar, geometryDistance);

    float stepSize = length(boundingBoxMax - boundingBoxMin) / float(numStepsView); 
    
    float distanceInsideBox = tFar - tNear;
    int numSteps = clamp(int(distanceInsideBox / stepSize), 0, numStepsView);

    float jitter = hash(texCoord * resolution) * stepSize;
    float currentT = tNear + jitter;

    vec3 lightEnergy = vec3(0.0f);
    float transmittance = 1.0f;

    while (currentT < tFar)
    {
        vec3 currentPosition = rayOrigin + rayDirection * currentT;
        float densitySample = sampleDensity(currentPosition);
        
        if (densitySample > 0.01)
        {
            float lightTransmittance = lightMarch(currentPosition);            
            lightEnergy += densitySample * stepSize * transmittance * lightTransmittance;
            transmittance *= exp(-densitySample * absorption * stepSize);

            if (transmittance < 0.01)
            {
                break;
            }
        }
        
        currentT += stepSize;
    }

    vec3 finalColor = opaqueColor.rgb * transmittance + lightEnergy;
    glColor = vec4(finalColor, 1.0);
}
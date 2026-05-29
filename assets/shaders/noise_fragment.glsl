#version 410 core

out vec4 glColor;

in vec2 vUV;

uniform float uTime;
uniform float uSeed;
uniform int uSplitsPerAxis;
uniform int uInverted;
uniform vec2 uNoiseResolution;

uniform sampler2D colorMap;

vec2 random2( vec2 p ) {
    vec2 anchorA = vec2(127.1, 311.7);
    vec2 anchorB = vec2(269.5, 183.3);
    float seed = uSeed;

    return fract(sin(vec2(dot(p, anchorA),dot(p,anchorB)))*seed);
}

void main()
{
    vec2 st = gl_FragCoord.xy / uNoiseResolution.xy;
    st.x *= uNoiseResolution.x / uNoiseResolution.y;

    float cellSize = 1.0 / float(uSplitsPerAxis);
    st /= cellSize;

    vec2 i_st = floor(st);
    vec2 f_st = fract(st);

    float dist = 1.0;
    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            vec2 neighbor = vec2(float(x),float(y));

            vec2 point = random2(i_st + neighbor);

            point = 0.5 + 0.5*sin(uTime + 6.2831*point);

            vec2 diff = neighbor + point - f_st;

            float currentDist = length(diff);

            dist = min(dist, currentDist);
        }
    }

    vec3 color = vec3(0.0);
    color += dist * 2.0;

    if (uInverted > 0) {
        color = vec3(1.0) - color;
    }

    color.r += step(0.92, f_st.x) + step(0.98, f_st.y);

    glColor = vec4(color, 1.0);
}
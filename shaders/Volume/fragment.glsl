#version 410
layout(location = 0) in highp vec4 fragPos;
layout(location = 1) in highp vec4 vertPos;

uniform highp sampler3D volume;

uniform highp vec4 eye;

uniform highp vec2 huRange;
uniform highp vec2 valueRange;

uniform highp float slope;
uniform highp float intercept;

uniform highp int windowCenter;
uniform highp int windowWidth;

layout(location = 0) out highp vec4 fragColor;

vec4 calcFragColor(const vec4 position, const vec4 normal, const vec4 color,
                   const vec3 positionModel);

bool needToRender(const vec3 point,
                  const vec2 xab, const vec2 yab, const vec2 zab);


void main(void) {
    if (!needToRender(fragPos.xyz, vec2(0.5f, 0.5f), vec2(0.5f, 0.5f), vec2(0.5f, 0.5f))) {
        discard;
    }

    float valueRangeSpan = valueRange.y - valueRange.x;

    float volumeColor = texture(volume, fragPos.stp).r * valueRangeSpan;

    float volColorH = volumeColor * slope + intercept;

    volColorH *= (step(huRange.x, volColorH) * (1 - step(huRange.y, volColorH)));

    if (volColorH == 0.0f) {
        discard;
    }

    float minEdge = windowCenter - 0.5f - (windowWidth - 1) / 2.0f;
    float maxEdge = windowCenter - 0.5f + (windowWidth - 1) / 2.0f;

    float minStep = (1 - step(minEdge, volColorH));
    float maxStep = step(maxEdge, volColorH);

    uint volColorHNorm = uint(
        minStep * valueRange.x +
        maxStep * valueRange.y +
        (1 - minStep) * (1 - maxStep) * ((volColorH - windowCenter + 0.5f) / (windowWidth - 1) + 0.5f) *
                         valueRangeSpan
                         );

    vec4 volumeColorf = vec4(volColorH  / valueRangeSpan);

    fragColor = calcFragColor(vertPos, fragPos, volumeColorf, fragPos.xyz);
}

#version 410
layout(location = 0) in highp vec4 pos;
layout(location = 1) in highp vec3 vertexTest;
layout(location = 2) in highp vec4 N;

uniform highp vec4 colorU;

uniform highp sampler2D points;

uniform lowp uint pointCount;

struct Ranges {
   vec2 xRange;
   vec2 yRange;
   vec2 zRange;
};

uniform highp Ranges ranges;

struct Material {
    vec4 emissive;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform highp Material stlMaterial;

struct LightSource {
    vec4 position;
    vec4 color;
    float ambientIntensity;
};

uniform highp LightSource lightSource;

layout(location = 0) out highp vec4 fragColor;

bool needToRender(vec3 point, vec2 xRange, vec2 yRange, vec2 zRange, vec2 xab, vec2 yab, vec2 zab);

void main(void) {
    if (needToRender(vertexTest, ranges.xRange, ranges.yRange, ranges.zRange,
                     vec2(1.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 0.0))) {

        vec4 L = normalize(lightSource.position - pos);

        float NdotL = max(dot(N, L), 0);
        vec4 diffuse =  NdotL * lightSource.color * stlMaterial.diffuse;

        vec4 V = normalize(N - pos);
        vec4 H = normalize(L + V);
        vec4 R = reflect(-L, N);

        float RdotV = max(dot(R, V), 0);
        float NdotH = max(dot(N, H), 0);

        vec4 specular = pow(RdotV, stlMaterial.shininess) * lightSource.color * stlMaterial.specular;

        fragColor = (stlMaterial.emissive + lightSource.ambientIntensity + diffuse + specular) * colorU;
/*
        if (fragColor.r > 0.75 && fragColor.r < 0.85) {
        //if (headColor.r > 0.82) {//&& fragPos.p < 0.8 && fragPos.s > 0.33 && fragPos.s < 0.67) {
            fragColor.a *= (0.08 * fragColor.r);
        }
        else {
            fragColor.a *= 0.2;
        }*/
    }
    else {
        discard;
    }
}

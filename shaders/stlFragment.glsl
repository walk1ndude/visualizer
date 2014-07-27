#version 410
layout(location = 0) in highp vec4 pos;
layout(location = 1) in highp vec3 vertexTest;
layout(location = 2) in highp vec4 N;

uniform highp vec4 colorU;

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

void main(void) {
    if (vertexTest.x >= ranges.xRange.x && vertexTest.x <= ranges.xRange.y &&
        vertexTest.y >= ranges.yRange.x && vertexTest.y <= ranges.yRange.y &&
        vertexTest.z >= ranges.zRange.x && vertexTest.z <= ranges.zRange.y) {

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
    }
    else {
        discard;
    }
}

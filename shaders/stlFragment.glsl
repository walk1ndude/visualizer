#version 410 core

layout(location = 0) out highp vec4 vertex;
layout(location = 1) out highp vec3 normal;

uniform highp vec4 colorU;

uniform highp mat3 normalMatrix;

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

out highp vec4 fragColor;

void main(void) {
    vec4 N = vec4(normalize(normalMatrix * normal), 0.0);
    vec4 L = normalize(lightSource.position - vertex);

    float NdotL = max(dot(N, L), 0);
    vec4 diffuse =  NdotL * lightSource.color * stlMaterial.diffuse;

    vec4 V = normalize(vec4(normal, 1.0) - vertex);
    vec4 H = normalize(L + V);
    vec4 R = reflect(-L, N);

    float RdotV = max(dot(R, V), 0);
    float NdotH = max(dot(N, H), 0);

    vec4 specular = pow(RdotV, stlMaterial.shininess) * lightSource.color * stlMaterial.specular;

    fragColor = (stlMaterial.emissive + lightSource.ambientIntensity + diffuse + specular) * colorU;
}

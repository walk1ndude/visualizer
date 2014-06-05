#version 410 core

layout(location = 0) out highp vec4 vertex;
layout(location = 1) out highp vec3 normal;

uniform highp vec4 color;

uniform highp mat3 normalMatrix;

struct Material {
    vec4 emissive;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform highp Material headMaterial;

struct LightSource {
    vec4 direction;
    vec4 color;
    float ambientIntensity;
};

uniform highp LightSource light;

out highp vec4 fragColor;

void main(void) {
    vec4 N = vec4(normalize(normalMatrix * normal), 0.0);
    vec4 L = normalize(light.direction - vertex);

    float NdotL = max(dot(N, L), 0);
    vec4 diffuse =  NdotL * light.color * headMaterial.diffuse;

    vec4 V = normalize(normal - vertex);
    vec4 H = normalize(L + V);
    vec4 R = reflect(-L, N);

    float RdotV = max(dot(R, V), 0);
    float NdotH = max(dot(N, H), 0);

    vec4 specular = pow(RdotV, headMaterial.shininess) * light.color * headMaterial.specular;

    fragColor = (headMaterial.emissive + light.ambientIntensity + diffuse + specular) * color;
}

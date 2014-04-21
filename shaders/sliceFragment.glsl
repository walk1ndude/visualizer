#version 330 core
in highp vec4 fragPos;
in highp vec4 fragNormal;
in highp vec4 vertexPos;

uniform highp sampler3D texSample;

uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 scale;

uniform highp vec4 materialEmissive;
uniform highp vec4 materialDiffuse;
uniform highp vec4 materialSpecular;
uniform highp float materialShininess;

uniform highp vec4 lightDirection;
uniform highp vec4 lightColor;

uniform highp float ambientIntensity;

out highp vec4 fragColor;

void main(void) {
    vec4 emissive = materialEmissive;

    vec4 N = normalize(fragNormal);
    vec4 L = normalize(lightDirection - fragPos);

    float NdotL = max(dot(N, L), 0);
    vec4 diffuse =  NdotL * lightColor * materialDiffuse;

    vec4 V = normalize(vec4(0.0, 0.0, 0.0, 1.0) - fragPos);
    vec4 H = normalize(L + V);
    vec4 R = reflect(-L, N);

    float RdotV = max(dot(R, V), 0);
    float NdotH = max(dot(N, H), 0);

    vec4 specular = pow(RdotV, materialShininess) * lightColor * materialSpecular;

    fragColor = lightColor * (emissive + ambientIntensity + diffuse + specular) * texture(texSample, fragPos.stp);
}

#version 330 core
smooth in highp vec4 fragPos;
smooth in highp vec3 fragNormal;

uniform highp float rBottom;
uniform highp float rTop;

uniform highp sampler3D texSample;

uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 scale;

uniform highp float ambientIntensity;

out highp vec4 fragColor;

struct SimpleDirectionalLight {
   vec3 vColor;
   vec3 vDirection;
   float fAmbientIntensity;
};

SimpleDirectionalLight sunLight;

void main(void) {
    sunLight.vColor = vec3(1.0, 1.0, 1.0);
    sunLight.vDirection = vec3(0.0, 0.0, 1.0);
    sunLight.fAmbientIntensity = ambientIntensity;

    //vec3 lightVec = vec3(0.5, 0.5, 0.5);
    //sunLight.vDirection = mat3(scale * model) * (sunLight.vDirection - lightVec) + lightVec;

    fragColor = texture(texSample, fragPos.stp);

    float fDiffuseIntensity = max(0.0, dot(normalize(fragNormal), -sunLight.vDirection));

    fragColor = fragColor * vec4(sunLight.vColor * (sunLight.fAmbientIntensity + fDiffuseIntensity), 1.0);

    if (fragColor.r < rBottom || fragColor.r > rTop) {
        discard;
    }
}

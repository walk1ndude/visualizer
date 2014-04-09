#version 330 core
smooth in highp vec4 fragPos;
smooth in highp vec3 fragNormal;

uniform float rBottom;
uniform float rTop;
uniform sampler3D texSample;

out vec4 fragColor;

const vec4 vColor = vec4(1.0, 1.0, 1.0, 1.0);

struct SimpleDirectionalLight {
   vec3 vColor;
   vec3 vDirection;
   float fAmbientIntensity;
};

SimpleDirectionalLight sunLight = SimpleDirectionalLight(
            vec3(1.0, 1.0, 1.0),
            vec3(0.0, 0.0, 1.0),
            0.5
);


void main(void) {
    fragColor = texture(texSample, fragPos.stp);
    //float fDiffuseIntensity = max(0.0, dot(normalize(fragNormal), -sunLight.vDirection));

    //fragColor = fragColor * vColor * vec4(sunLight.vColor * (sunLight.fAmbientIntensity + fDiffuseIntensity), 1.0);

    if (fragColor.r < rBottom || fragColor.r > rTop) {
        discard;
    }
}

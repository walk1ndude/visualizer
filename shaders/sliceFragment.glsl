#version 330 core
smooth in highp vec4 fragPos;

struct Ranges {
   vec2 sRange;
   vec2 tRange;
   vec2 pRange;
};

uniform highp Ranges ranges;

uniform highp sampler3D texHead;
uniform highp sampler3D texGradient;

uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 scale;

uniform highp mat4 normalMatrix;

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
    if (fragPos.s >= ranges.sRange[0] && fragPos.s <= ranges.sRange[1]
            && fragPos.t >= ranges.tRange[0] && fragPos.t <= ranges.tRange[1]
            && fragPos.p >= ranges.pRange[0] && fragPos.p <= ranges.pRange[1]) {
        vec4 headColor = texture(texHead, fragPos.stp);

        if (headColor.r > 0.01) {

            vec4 normal = texture(texGradient, fragPos.stp);

            vec4 N = normalize(normalMatrix * normal);
            vec4 L = normalize(light.direction - fragPos);

            float NdotL = max(dot(N, L), 0);
            vec4 diffuse =  NdotL * light.color * headMaterial.diffuse;

            vec4 V = normalize(normal - fragPos);
            vec4 H = normalize(L + V);
            vec4 R = reflect(-L, N);

            float RdotV = max(dot(R, V), 0);
            float NdotH = max(dot(N, H), 0);

            vec4 specular = pow(RdotV, headMaterial.shininess) * light.color * headMaterial.specular;

            fragColor = light.color * (headMaterial.emissive + light.ambientIntensity + diffuse + specular) * headColor;

           /* if (fragColor.r < 0.15) {
                fragColor.a = 1.0;
            }
*/
        }
        else {
            discard;
        }
    }
    else {
        discard;
    }
}

#version 410 core
smooth in highp vec4 fragPos;

struct Ranges {
   vec2 sRange;
   vec2 tRange;
   vec2 pRange;
};

uniform highp Ranges ranges;

uniform highp sampler3D texHead;

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

uniform highp vec3 stepSlices;

out highp vec4 fragColor;

vec4 sobel3(vec3 position) {
    vec3 p = vec3(0);
    vec3 n = vec3(0);

    float fragElem[27];

    fragElem[0] = -1 * texture(texHead, position + vec3(-stepSlices.x, -stepSlices.y, -stepSlices.z)).r;
    fragElem[1] = -3 * texture(texHead, position + vec3(-stepSlices.x, 0, -stepSlices.z)).r;
    fragElem[2] = -1 * texture(texHead, position + vec3(-stepSlices.x, stepSlices.y, -stepSlices.z)).r;
    fragElem[3] = -3 * texture(texHead, position + vec3(-stepSlices.x, -stepSlices.y, 0)).r;
    fragElem[4] = -6 * texture(texHead, position + vec3(-stepSlices.x, 0, 0)).r;
    fragElem[5] = -3 * texture(texHead, position + vec3(-stepSlices.x, stepSlices.y, 0)).r;
    fragElem[6] = -1 * texture(texHead, position + vec3(-stepSlices.x, -stepSlices.y, stepSlices.z)).r;
    fragElem[7] = -3 * texture(texHead, position + vec3(-stepSlices.x, 0, stepSlices.z)).r;
    fragElem[8] = -1 * texture(texHead, position + vec3(-stepSlices.x, stepSlices.y, stepSlices.z)).r;

    fragElem[9] = 0.0;
    fragElem[10] = 0.0;
    fragElem[11] = 0.0;
    fragElem[12] = 0.0;
    fragElem[13] = 0.0;
    fragElem[14] = 0.0;
    fragElem[15] = 0.0;
    fragElem[16] = 0.0;
    fragElem[17] = 0.0;

    fragElem[18] = 1 * texture(texHead, position + vec3(stepSlices.x, -stepSlices.y, -stepSlices.z)).r;
    fragElem[19] = 3 * texture(texHead, position + vec3(stepSlices.x, 0, -stepSlices.z)).r;
    fragElem[20] = 1 * texture(texHead, position + vec3(stepSlices.x, stepSlices.y, -stepSlices.z)).r;
    fragElem[21] = 3 * texture(texHead, position + vec3(stepSlices.x, -stepSlices.y, 0)).r;
    fragElem[22] = 6 * texture(texHead, position + vec3(stepSlices.x, 0, 0)).r;
    fragElem[23] = 3 * texture(texHead, position + vec3(stepSlices.x, stepSlices.y, 0)).r;
    fragElem[24] = 1 * texture(texHead, position + vec3(stepSlices.x, -stepSlices.y, stepSlices.z)).r;
    fragElem[25] = 3 * texture(texHead, position + vec3(stepSlices.x, 0, stepSlices.z)).r;
    fragElem[26] = 1 * texture(texHead, position + vec3(stepSlices.x, stepSlices.y, stepSlices.z)).r;

    for (int i = 0; i != 27; ++ i) {
        if (i / 9 == 0) {
            p.x += fragElem[i];
        }

        if (i / 9 == 2) {
            n.x += fragElem[i];
        }

        if (i % 3 == 0) {
            p.y += fragElem[i];
        }

        if (i % 3 == 2) {
            n.y += fragElem[i];
        }

        if (i % 9 / 3 == 0) {
            p.z += fragElem[i];
        }

        if (i % 9 / 3 == 2) {
            n.z += fragElem[i];
        }
    }

    return vec4(normalize(n - p), 0.0);
}

void main(void) {
    if (fragPos.s >= ranges.sRange[0] && fragPos.s <= ranges.sRange[1]
            && fragPos.t >= ranges.tRange[0] && fragPos.t <= ranges.tRange[1]
            && fragPos.p >= ranges.pRange[0] && fragPos.p <= ranges.pRange[1]) {

        vec4 headColor = texture(texHead, fragPos.stp).rrrr;

        fragColor = headColor;
/*
        if (headColor.r > 0.01) {
            vec4 normal = sobel3(fragPos.stp);

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

            fragColor = (headMaterial.emissive + light.ambientIntensity + diffuse + specular) * headColor;

            //if (headColor.r > 0.75 && headColor.r < 0.85 && fragPos.p > 0.45 && fragPos.p < 0.8) {
           if (headColor.r > 0.82) {//&& fragPos.p < 0.8 && fragPos.s > 0.33 && fragPos.s < 0.67) {
                fragColor.a *= (0.05 * headColor.r);
            }
            else {
                fragColor.a *= 0.02;
            }
        }
        else {
            discard;
        }*/
    }
    else {
        discard;
    }

}

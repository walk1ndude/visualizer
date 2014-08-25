#version 410
in highp vec4 fragPos;

struct Ranges {
   vec2 xRange;
   vec2 yRange;
   vec2 zRange;
};

uniform highp Ranges ranges;

uniform highp sampler3D texHead;

uniform highp mat4 normalMatrix;

struct Material {
    vec4 emissive;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform highp Material headMaterial;

struct LightSource {
    vec4 position;
    vec4 color;
    float ambientIntensity;
};

uniform highp LightSource lightSource;

uniform highp vec3 stepSlices;

out highp vec4 fragColor;

vec4 sobel3(vec3 position) {
    vec3 p = vec3(0.0f);
    vec3 n = vec3(0.0f);

    float fragElem[27];

    fragElem[0] = -1.0f * texture(texHead, position + vec3(-stepSlices.x, -stepSlices.y, -stepSlices.z)).r;
    fragElem[1] = -3.0f * texture(texHead, position + vec3(-stepSlices.x, 0.0f, -stepSlices.z)).r;
    fragElem[2] = -1.0f * texture(texHead, position + vec3(-stepSlices.x, stepSlices.y, -stepSlices.z)).r;
    fragElem[3] = -3.0f * texture(texHead, position + vec3(-stepSlices.x, -stepSlices.y, 0.0f)).r;
    fragElem[4] = -6.0f * texture(texHead, position + vec3(-stepSlices.x, 0.0f, 0.0f)).r;
    fragElem[5] = -3.0f * texture(texHead, position + vec3(-stepSlices.x, stepSlices.y, 0.0f)).r;
    fragElem[6] = -1.0f * texture(texHead, position + vec3(-stepSlices.x, -stepSlices.y, stepSlices.z)).r;
    fragElem[7] = -3.0f * texture(texHead, position + vec3(-stepSlices.x, 0.0f, stepSlices.z)).r;
    fragElem[8] = -1.0f * texture(texHead, position + vec3(-stepSlices.x, stepSlices.y, stepSlices.z)).r;

    fragElem[9] = 0.0f;
    fragElem[10] = 0.0f;
    fragElem[11] = 0.0f;
    fragElem[12] = 0.0f;
    fragElem[13] = 0.0f;
    fragElem[14] = 0.0f;
    fragElem[15] = 0.0f;
    fragElem[16] = 0.0f;
    fragElem[17] = 0.0f;

    fragElem[18] = 1.0f * texture(texHead, position + vec3(stepSlices.x, -stepSlices.y, -stepSlices.z)).r;
    fragElem[19] = 3.0f * texture(texHead, position + vec3(stepSlices.x, 0.0f, -stepSlices.z)).r;
    fragElem[20] = 1.0f * texture(texHead, position + vec3(stepSlices.x, stepSlices.y, -stepSlices.z)).r;
    fragElem[21] = 3.0f * texture(texHead, position + vec3(stepSlices.x, -stepSlices.y, 0.0f)).r;
    fragElem[22] = 6.0f * texture(texHead, position + vec3(stepSlices.x, 0.0f, 0.0f)).r;
    fragElem[23] = 3.0f * texture(texHead, position + vec3(stepSlices.x, stepSlices.y, 0.0f)).r;
    fragElem[24] = 1.0f * texture(texHead, position + vec3(stepSlices.x, -stepSlices.y, stepSlices.z)).r;
    fragElem[25] = 3.0f * texture(texHead, position + vec3(stepSlices.x, 0.0f, stepSlices.z)).r;
    fragElem[26] = 1.0f * texture(texHead, position + vec3(stepSlices.x, stepSlices.y, stepSlices.z)).r;

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

    return vec4(normalize(n - p), 0.0f);
}

bool needToRender(vec3 point, vec2 xRange, vec2 yRange, vec2 zRange, vec2 xab, vec2 yab, vec2 zab);

void main(void) {
    if (needToRender(vec3(fragPos), ranges.xRange, ranges.yRange, ranges.zRange,
                     vec2(0.5, 0.5), vec2(0.5, 0.5), vec2(0.5, 0.5))) {
        vec4 headColor = texture(texHead, fragPos.stp).rrrr;

        if (headColor.r > 0.05f) {
            vec4 normal = sobel3(fragPos.stp);

            vec4 N = normalize(normalMatrix * normal);
            vec4 L = normalize(lightSource.position - fragPos);

            float NdotL = max(dot(N, L), 0.0f);
            vec4 diffuse =  NdotL * lightSource.color * headMaterial.diffuse;

            vec4 V = normalize(normal - fragPos);
            vec4 H = normalize(L + V);
            vec4 R = reflect(-L, N);

            float RdotV = max(dot(R, V), 0.0f);
            float NdotH = max(dot(N, H), 0.0f);

            vec4 specular = pow(RdotV, headMaterial.shininess) * lightSource.color * headMaterial.specular;

            fragColor = (headMaterial.emissive + lightSource.ambientIntensity + diffuse + specular) * headColor;

            //fragColor.a = 1.0;
/*
            if (headColor.r > 0.75 && headColor.r < 0.85 && fragPos.p > 0.45 && fragPos.p < 0.8) {
            //if (headColor.r > 0.82) {//&& fragPos.p < 0.8 && fragPos.s > 0.33 && fragPos.s < 0.67) {
                fragColor.a *= (0.01 * headColor.r);
            }
            else {
                fragColor.a *= 0.05;
            }
*/
            //fragColor.a = clamp(fragColor.a, 0.0, 1.0);

        }
        else {
            discard;
        }
    }
    else {
        discard;
    }
}
